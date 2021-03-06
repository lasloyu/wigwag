#ifndef WIGWAG_POLICIES_THREADING_OWN_RECURSIVE_MUTEX_HPP
#define WIGWAG_POLICIES_THREADING_OWN_RECURSIVE_MUTEX_HPP

// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


#include <wigwag/detail/config.hpp>
#include <wigwag/policies/threading/tag.hpp>

#include <thread>


namespace wigwag {
namespace threading
{

#include <wigwag/detail/disable_warnings.hpp>

    struct own_recursive_mutex
    {
        using tag = threading::tag<api_version<2, 0>>;

        class lock_primitive
        {
        private:
            mutable std::recursive_mutex    _mutex;

        public:
            std::recursive_mutex& get_primitive() const WIGWAG_NOEXCEPT { return _mutex; }

            void lock_nonrecursive() const { _mutex.lock(); }
            void unlock_nonrecursive() const { _mutex.unlock(); }

            void lock_recursive() const { _mutex.lock(); }
            void unlock_recursive() const { _mutex.unlock(); }
        };
    };

#include <wigwag/detail/enable_warnings.hpp>

}}

#endif
