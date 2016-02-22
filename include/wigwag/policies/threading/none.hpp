#ifndef WIGWAG_POLICIES_THREADING_NONE_HPP
#define WIGWAG_POLICIES_THREADING_NONE_HPP

// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


namespace wigwag {
namespace threading
{

#include <wigwag/detail/disable_warnings.hpp>

	struct none
	{
		class lock_primitive
		{
		public:
			void get_primitive() const noexcept { }

			void lock_nonrecursive() const noexcept { }
			void unlock_nonrecursive() const noexcept { }

			void lock_recursive() const noexcept { }
			void unlock_recursive() const noexcept { }
		};
	};

#include <wigwag/detail/enable_warnings.hpp>

}}

#endif