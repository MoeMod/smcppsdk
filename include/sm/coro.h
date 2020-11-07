#pragma once

// !!! Requires C++20 
#if __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
#endif

#include <concepts>

#include "sourcemod_timers.h"

namespace sm {
	namespace coro {
		namespace detail {
#if __has_include(<experimental/coroutine>)
			using std::experimental::coroutine_handle;
			using std::experimental::suspend_always;
			using std::experimental::suspend_never;
#else
#error "no coroutine lib support found"
#endif
		}
		template<class Fn = void(*)(void(*)())>
		auto CreateAwaitable(Fn &&fn) // requires std::invocable<Fn, std::function<void()>>
		{
			struct AwaitableResult
			{
				bool await_ready() noexcept {
					return false;
				}
				void await_suspend(detail::coroutine_handle<> h) {
					std::invoke(m_callback, std::bind(&detail::coroutine_handle<>::resume, h));
				}
				void await_resume() noexcept {}

				typename std::decay<Fn>::type m_callback;
			};
			return AwaitableResult{ std::forward<Fn>(fn) };
		}

		auto CreateTimer(float interval)
		{
			return CreateAwaitable([interval](auto f) { sm::CreateTimer(interval, f); });
		}

		class Task
		{
		public:
			struct promise_type {
				auto get_return_object() { return Task{}; }
				auto initial_suspend() { return detail::suspend_never{}; }
				auto final_suspend() noexcept { return detail::suspend_never{}; }
				void unhandled_exception() { std::terminate(); }
				void return_void() {}
			};
		};
	}
}