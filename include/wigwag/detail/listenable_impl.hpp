#ifndef WIGWAG_DETAIL_LISTENABLE_IMPL_HPP
#define WIGWAG_DETAIL_LISTENABLE_IMPL_HPP

// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


#include <wigwag/detail/at_scope_exit.hpp>
#include <wigwag/detail/config.hpp>
#include <wigwag/detail/enabler.hpp>
#include <wigwag/detail/intrusive_list.hpp>
#include <wigwag/detail/intrusive_ptr.hpp>
#include <wigwag/detail/intrusive_ref_counter.hpp>
#include <wigwag/detail/storage_for.hpp>
#include <wigwag/handler_attributes.hpp>
#include <wigwag/token.hpp>


namespace wigwag {
namespace detail
{

#include <wigwag/detail/disable_warnings.hpp>

    template < typename ShouldBeConstructible_, typename Arg_, typename... ShouldNotBeConstructible_ >
    struct check_constructible;

    template < typename ShouldBeConstructible_, typename Arg_, typename ShouldNotBeConstructibleHead_, typename... ShouldNotBeConstructible_ >
    struct check_constructible<ShouldBeConstructible_, Arg_, ShouldNotBeConstructibleHead_, ShouldNotBeConstructible_...>
    { static const bool value = !std::is_constructible<ShouldNotBeConstructibleHead_, Arg_>::value && check_constructible<ShouldBeConstructible_, Arg_, ShouldNotBeConstructible_...>::value; };

    template < typename ShouldBeConstructible_, typename Arg_ >
    struct check_constructible<ShouldBeConstructible_, Arg_>
    { static const bool value = std::is_constructible<ShouldBeConstructible_, Arg_>::value; };


    template <
            typename HandlerType_,
            typename ExceptionHandlingPolicy_,
            typename ThreadingPolicy_,
            typename StatePopulatingPolicy_,
            typename LifeAssurancePolicy_,
            typename RefCounterPolicy_
        >
    class listenable_impl
        :   private intrusive_ref_counter<RefCounterPolicy_, listenable_impl<HandlerType_, ExceptionHandlingPolicy_, ThreadingPolicy_, StatePopulatingPolicy_, LifeAssurancePolicy_, RefCounterPolicy_>>,
            protected LifeAssurancePolicy_::shared_data,
            protected ExceptionHandlingPolicy_,
            protected ThreadingPolicy_::lock_primitive,
            protected StatePopulatingPolicy_::template handler_processor<HandlerType_>
    {
        friend class intrusive_ref_counter<RefCounterPolicy_, listenable_impl<HandlerType_, ExceptionHandlingPolicy_, ThreadingPolicy_, StatePopulatingPolicy_, LifeAssurancePolicy_, RefCounterPolicy_>>;
        using ref_counter_base = intrusive_ref_counter<RefCounterPolicy_, listenable_impl<HandlerType_, ExceptionHandlingPolicy_, ThreadingPolicy_, StatePopulatingPolicy_, LifeAssurancePolicy_, RefCounterPolicy_>>;

    public:
        using handler_type = HandlerType_;

        using exception_handler = ExceptionHandlingPolicy_;
        using lock_primitive = typename ThreadingPolicy_::lock_primitive;
        using handler_processor = typename StatePopulatingPolicy_::template handler_processor<handler_type>;

        using life_assurance = typename LifeAssurancePolicy_::life_assurance;
        using life_checker = typename LifeAssurancePolicy_::life_checker;
        using execution_guard = typename LifeAssurancePolicy_::execution_guard;

    protected:
        class handler_node : public token::implementation, private life_assurance, private detail::intrusive_list_node
        {
            friend class detail::intrusive_list<handler_node>;

        private:
            intrusive_ptr<listenable_impl>  _listenable_impl;
            storage_for<handler_type>       _handler;

        public:
            template < typename MakeHandlerFunc_ >
            handler_node(intrusive_ptr<listenable_impl> impl, const MakeHandlerFunc_& mhf)
                : _listenable_impl(std::move(impl)), _handler(mhf(life_checker(*_listenable_impl, *this)))
            { _listenable_impl->get_handlers_container().push_back(*this); }

            handler_node(intrusive_ptr<listenable_impl> impl, handler_type handler)
                : _listenable_impl(std::move(impl)), _handler(std::move(handler))
            { _listenable_impl->get_handlers_container().push_back(*this); }

            virtual ~handler_node()
            { }

            virtual void release_token_impl()
            {
                life_assurance::release_life_assurance(*_listenable_impl);

                if (!suppress_populator() && _listenable_impl->get_handler_processor().has_withdraw_state())
                {
                    _listenable_impl->get_lock_primitive().lock_nonrecursive();
                    auto sg = detail::at_scope_exit([&] { _listenable_impl->get_lock_primitive().unlock_nonrecursive(); } );
                    _listenable_impl->get_handler_processor().withdraw_state(_handler.ref());
                }

                _handler.ref().~handler_type();

                if (life_assurance::release_node())
                {
                    {
                        _listenable_impl->get_lock_primitive().lock_nonrecursive();
                        auto sg = detail::at_scope_exit([&] { _listenable_impl->get_lock_primitive().unlock_nonrecursive(); } );
                        _listenable_impl->get_handlers_container().erase(*this);
                    }
                    delete this;
                }
            }

            bool should_be_finalized() const
            { return life_assurance::node_should_be_released(); }

            void finalize_node()
            {
                if (life_assurance::release_node())
                {
                    _listenable_impl->get_handlers_container().erase(*this);
                    delete this;
                }
            }

            handler_type& get_handler() { return _handler.ref(); }
            const life_assurance& get_life_assurance() const { return *this; }

        protected:
            virtual bool suppress_populator()
            { return false; }
        };

        class handler_node_with_attributes : public handler_node
        {
        private:
            handler_attributes      _attributes;

        public:
            template < typename... Args_ >
            handler_node_with_attributes(handler_attributes attributes, Args_&&... args)
                : handler_node(std::forward<Args_>(args)...), _attributes(attributes)
            { }

        protected:
            virtual bool suppress_populator()
            { return contains_flag(_attributes, handler_attributes::suppress_populator); }
        };

        using handlers_container = detail::intrusive_list<handler_node>;

        handlers_container                  _handlers;

    public:
        template <
                bool E_ =
                    std::is_constructible<exception_handler>::value &&
                    std::is_constructible<lock_primitive>::value &&
                    std::is_constructible<handler_processor>::value,
                typename = typename std::enable_if<E_>::type
            >
        listenable_impl() { }

        virtual ~listenable_impl() { }

#define DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(...) typename std::enable_if<__VA_ARGS__, basic_enabler<__LINE__>>::type = basic_enabler<__LINE__>()

        template < typename T_ > listenable_impl(T_ eh, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<exception_handler, T_&&, lock_primitive>::value)) : exception_handler(std::move(eh)) { }
        template < typename T_ > listenable_impl(T_ lp, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<lock_primitive, T_&&, exception_handler>::value)) : lock_primitive(std::move(lp)) { }
        template < typename T_ > listenable_impl(T_ hp, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<handler_processor, T_&&, lock_primitive, exception_handler>::value)) : handler_processor(std::move(hp)) { }


        template < typename T_, typename U_ >
        listenable_impl(T_ eh, U_ lp, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<exception_handler, T_&&>::value && check_constructible<lock_primitive, U_&&>::value))
            : exception_handler(std::move(eh)), lock_primitive(std::move(lp))
        { }

        template < typename T_, typename U_ >
        listenable_impl(T_ eh, U_ hp, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<exception_handler, T_&&>::value && check_constructible<handler_processor, U_&&, lock_primitive>::value))
            : exception_handler(std::move(eh)), handler_processor(std::move(hp))
        { }

        template < typename T_, typename U_ >
        listenable_impl(T_ lp, U_ hp, DETAIL_LISTENABLE_IMPL_CTOR_ENABLER(check_constructible<lock_primitive, T_&&>::value && check_constructible<handler_processor, U_&&>::value))
            : lock_primitive(std::move(lp)), handler_processor(std::move(hp))
        { }


#undef DETAIL_LISTENABLE_IMPL_CTOR_ENABLER

        listenable_impl(exception_handler eh, lock_primitive lp, handler_processor hp)
            : exception_handler(std::move(eh)), lock_primitive(std::move(lp)), handler_processor(std::move(hp))
        { }

        listenable_impl(const listenable_impl&) = delete;
        listenable_impl& operator = (const listenable_impl&) = delete;


        void finalize_nodes()
        {
            for (auto it = _handlers.begin(); it != _handlers.end();)
                (it++)->finalize_node();
        }

        void add_ref() { ref_counter_base::add_ref(); }
        void release() { ref_counter_base::release(); }

        token connect(handler_type handler, handler_attributes attributes)
        {
            get_lock_primitive().lock_nonrecursive();
            auto sg = detail::at_scope_exit([&] { get_lock_primitive().unlock_nonrecursive(); } );

            if (!contains_flag(attributes, handler_attributes::suppress_populator) && get_handler_processor().has_populate_state())
                get_exception_handler().handle_exceptions([&] { get_handler_processor().populate_state(handler); });

            return create_node(attributes, std::move(handler));
        }

        template < typename InvokeListenerFunc_ >
        void invoke(InvokeListenerFunc_&& invoke_listener_func)
        {
            get_lock_primitive().lock_recursive();
            auto sg = detail::at_scope_exit([&] { get_lock_primitive().unlock_recursive(); } );

            if (this->_handlers.empty())
                return;
            auto it = this->_handlers.begin(), e = this->_handlers.pre_end();

            bool last_iter = false;
            while (!last_iter)
            {
                last_iter = it == e;

                if (it->should_be_finalized())
                {
                    (it++)->finalize_node();
                    continue;
                }

                execution_guard g(get_life_assurance_shared_data(), it->get_life_assurance());
                if (g.is_alive())
                    get_exception_handler().handle_exceptions(invoke_listener_func, it->get_handler());
                ++it;
            }
        }

        const lock_primitive& get_lock_primitive() const { return *this; }

    protected:
        template < typename... Args_>
        token create_node(handler_attributes attributes, Args_&&... args)
        {
            add_ref();
            intrusive_ptr<listenable_impl> self(this);

            if (attributes == handler_attributes::none)
                return token::create<handler_node>(self, std::forward<Args_>(args)...);
            else
                return token::create<handler_node_with_attributes>(attributes, self, std::forward<Args_>(args)...);
        }

        const typename LifeAssurancePolicy_::shared_data& get_life_assurance_shared_data() const { return *this; }

        handlers_container& get_handlers_container() { return _handlers; }
        const handlers_container& get_handlers_container() const { return _handlers; }

        const exception_handler& get_exception_handler() const { return *this; }
        const handler_processor& get_handler_processor() const { return *this; }
    };

#include <wigwag/detail/enable_warnings.hpp>

}}

#endif
