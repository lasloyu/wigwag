// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


// Disabling some warnings here because fixing them has a performance penalty :(

#if defined(__GNUC__) || defined(__clang)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Weffc++"
#endif

#if defined(_MSC_VER)
#   if _MSC_VER >= 1900
#       pragma warning(disable: 5031) // Sorry :(
#   endif
#   pragma warning(push)
#   pragma warning(disable: 4355 4625 4626)
#   if _MSC_VER >= 1700 && _MSC_VER < 1900
#       pragma warning(disable: 4127)
#   endif
#   if _MSC_VER >= 1900
#       pragma warning(disable: 5026 5027)
#   endif
#endif

#if defined(WIGWAG_WARNINGS_DISABLED)
#   error Inconsistent disable_warnings.hpp/enable_warnings.hpp  includes!
#endif

#define WIGWAG_WARNINGS_DISABLED
