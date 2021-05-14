#pragma once

// !!! Requires C++20 
#include <coroutine>
#include <concepts>
#include <future>

#include "sourcemod_functions.h"
#include "sourcemod_timers.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>

namespace sm {
	namespace coro {
		template<class Fn = void(*)(void(*)())>
		boost::asio::awaitable<void> CreateAwaitable(Fn &&fn) // requires std::invocable<Fn, std::function<void()>>
		{
			auto ioc = sm::functions::GetGameFrameContext();
			boost::asio::deadline_timer ddl(*ioc, boost::posix_time::ptime(boost::posix_time::pos_infin));
            std::invoke(fn, [&ddl]() mutable { ddl.cancel(); });
			return ddl.async_wait(boost::asio::use_awaitable);
		}

		inline auto CreateTimer(float interval)
		{
			return CreateAwaitable([interval](auto f) { sm::CreateTimer(interval, f); });
		}

		inline auto RequestFrame()
        {
            return CreateAwaitable([](auto f) { sm::RequestFrame(f);});
        }

		using Task = boost::asio::awaitable<void>;
		template<class T> using Future = boost::asio::awaitable<T>;

		template<class T>
		T SyncAwait(boost::asio::awaitable<T> aw)
        {
		    boost::asio::io_context local_ioc;
		    std::promise<T> pro;
            boost::asio::co_spawn(local_ioc, [&aw, &pro]() mutable -> boost::asio::awaitable<void>  {
                pro.set_value(co_await aw);
            }, boost::asio::detached);
            local_ioc.run();
		    return pro.get_future().get();
        }

        template<class Func>
        void AsyncAwait(boost::asio::awaitable<void> &&aw, Func &&f)
        {
            auto ioc = sm::functions::GetGameFrameContext();
            boost::asio::co_spawn(*ioc, [&aw, &f]() mutable -> boost::asio::awaitable<void> {
                co_await std::move(aw);
                f();
            }, boost::asio::detached);
        }

        template<class T, class Func>
        void AsyncAwait(boost::asio::awaitable<T> &&aw, Func &&f)
        {
            auto ioc = sm::functions::GetGameFrameContext();
            boost::asio::co_spawn(*ioc, [&aw, &f]() mutable -> boost::asio::awaitable<void> {
                T val = co_await std::move(aw);
                if constexpr (std::is_invocable_v<Func, T>)
                    f(val);
                else
                    f();
            }, boost::asio::detached);
        }

	}
}