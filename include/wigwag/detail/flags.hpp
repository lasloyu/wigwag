#ifndef WIGWAG_DETAIL_FLAGS_HPP
#define WIGWAG_DETAIL_FLAGS_HPP

// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


#include <type_traits>


namespace wigwag {
namespace detail
{

#include <wigwag/detail/disable_warnings.hpp>

#define WIGWAG_DECLARE_ENUM_BITWISE_OPERATORS(EnumClass_) \
    inline EnumClass_ operator | (EnumClass_ l, EnumClass_ r) \
    { \
        using underlying = std::underlying_type<EnumClass_>::type; \
        return static_cast<EnumClass_>(static_cast<underlying>(l) | static_cast<underlying>(r)); \
    } \
    inline EnumClass_ operator & (EnumClass_ l, EnumClass_ r) \
    { \
        using underlying = std::underlying_type<EnumClass_>::type; \
        return static_cast<EnumClass_>(static_cast<underlying>(l) & static_cast<underlying>(r)); \
    }

    template < typename T_ >
    inline bool contains_flag(T_ val, T_ flag)
    { return (val & flag) != T_(); }

#include <wigwag/detail/enable_warnings.hpp>

}}

#endif
