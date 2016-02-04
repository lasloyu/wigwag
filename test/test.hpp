#ifndef TEST_TEST_HPP
#define TEST_TEST_HPP

// Copyright (c) 2016, Dmitry Koplyarov <koplyarov.da@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
// provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


#include <wigwag/signal.hpp>

#include <cxxtest/TestSuite.h>

#include <chrono>
#include <iostream>
#include <thread>


using namespace wigwag;


template < typename Signature_ >
using threadless_signal = basic_signal<Signature_, signal_policies::threading::threadless, signal_policies::handlers_storage::shared_list, signal_policies::life_assurance::none>;


class wigwag_tests : public CxxTest::TestSuite
{
private:
	template < typename ClockT_ >
	class basic_profiler
	{
		typedef std::chrono::time_point<ClockT_>		TimePoint;

	private:
		TimePoint		_start;

	public:
		basic_profiler() { _start = ClockT_::now(); }

		decltype(TimePoint() - TimePoint()) reset()
		{
			TimePoint end = ClockT_::now();
			auto delta = end - _start;
			_start = end;
			return delta;
		}
	};
	using profiler = basic_profiler<std::chrono::high_resolution_clock>;

public:
	static void test_default_life_assurance() { do_test_life_assurance<signal>(); }

private:
	template < template<typename> class Signal_ >
	static void do_test_life_assurance()
	{
		Signal_<void()> s;

		std::atomic<bool> alive(true);
		std::thread t(
			[&]()
			{
				while (alive)
				{
					s();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		);

		profiler p;

		{
			token t = s.connect([](){ std::this_thread::sleep_for(std::chrono::seconds(2)); });
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			p.reset();
		}
		TS_ASSERT_LESS_THAN(std::chrono::seconds(1), p.reset());

		alive = false;
		t.join();
	}
};

#endif