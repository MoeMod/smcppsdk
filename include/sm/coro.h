#pragma once

// !!! Requires C++20 
#include <coroutine>
#include <concepts>

#include "sourcemod_timers.h"

namespace sm {
	namespace coro {
		template<class Fn = void(*)(void(*)())>
		auto CreateAwaitable(Fn &&fn) // requires std::invocable<Fn, std::function<void()>>
		{
			struct AwaitableResult
			{
				bool await_ready() noexcept {
					return false;
				}
				void await_suspend(std::coroutine_handle<> h) {
					std::invoke(m_callback, std::bind(&std::coroutine_handle<>::resume, h));
				}
				void await_resume() noexcept {}

				typename std::decay<Fn>::type m_callback;
			};
			return AwaitableResult{ std::forward<Fn>(fn) };
		}

		inline auto CreateTimer(float interval)
		{
			return CreateAwaitable([interval](auto f) { sm::CreateTimer(interval, f); });
		}

		class Task
		{
		public:
			struct promise_type {
				auto get_return_object() { return Task{}; }
				auto initial_suspend() { return std::suspend_never{}; }
				auto final_suspend() noexcept { return std::suspend_never{}; }
				void unhandled_exception() { std::terminate(); }
				void return_void() {}
			};
		};
	}
}