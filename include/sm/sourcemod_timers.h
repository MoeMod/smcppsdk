#pragma once

#include <memory>
#include <type_traits>
#include <concepts>
#include <ITimerSystem.h>

namespace sm {
	inline namespace timers {
        #define TIMER_REPEAT TIMER_FLAG_REPEAT /**< Timer will repeat until it returns Plugin_Stop */
        // TIMER_FLAG_NO_MAPCHANGE /**< Timer will not carry over mapchanges */

        // concept TimerFunc
        // void func();
        // void func(std::shared_ptr<ITimer>);
        // void func(std::shared_ptr<ITimer>, AnyDataType);

        template<class T> concept AnyDataType_c = !std::is_void<T>::value;
        template<class F> concept TimerFunc1_c = std::invocable<F>;
        template<class F> concept TimerFunc2_c = std::invocable<F, std::shared_ptr<ITimer>>;
        template<class F, class AnyDataType> concept TimerFunc3_c = std::invocable<F, std::shared_ptr<ITimer>, AnyDataType&> && AnyDataType_c<AnyDataType>;
        template<class F, class AnyDataType> concept TimerFunc_c = TimerFunc1_c<F> || TimerFunc2_c<F> || TimerFunc3_c<F, AnyDataType>;
		
        namespace detail {
            template<std::size_t N> struct priority_tag : priority_tag<N-1> {};
            template<> struct priority_tag<0> {};
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler &rthis, priority_tag<3>)
                -> decltype(std::invoke(fn))
            {
                return std::invoke(fn);
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis, priority_tag<2>)
                -> decltype(std::invoke(fn, std::declval<std::shared_ptr<ITimer>>()))
            {
                return std::invoke(fn, std::shared_ptr<ITimer>(rthis.m_pSharedThis, rthis.m_pTimer));
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis, priority_tag<1>)
                -> decltype(std::invoke(fn, std::declval<std::shared_ptr<ITimer>>(), std::declval<typename std::decay<MyHandler>::type::data_type>()))
            {
                return std::invoke(fn, std::shared_ptr<ITimer>(rthis.m_pSharedThis, rthis.m_pTimer), rthis.m_data);
            }
            template<class MyFn, class MyHandler> auto CallTimerFunc(MyFn& fn, MyHandler& rthis)
            {
                return CallTimerFunc(fn, rthis, priority_tag<4>());
            }
        }

        template<AnyDataType_c AnyDataType = std::nullptr_t, TimerFunc_c<AnyDataType> Fn>
        std::weak_ptr<ITimer> CreateTimer(float fInterval, Fn&& func, AnyDataType &&data = nullptr, int flags = 0)
        {
            class MyHandler : public ITimedEvent {
            public:
                using data_type = AnyDataType;
                explicit MyHandler(Fn fn, ITimer* p, AnyDataType &&data) : m_fn(std::move(fn)), m_pTimer(p), m_data(std::forward<AnyDataType>(data)) {}
                ResultType OnTimer(ITimer* pTimer, void* pData) override
                {
                    detail::CallTimerFunc(m_fn, *this);
                    return Pl_Stop;
                }
                void OnTimerEnd(ITimer* pTimer, void* pData) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MyHandler()
                {
                    assert(m_pTimer != nullptr);
                    if (m_pSharedThis != nullptr)
                        timersys->KillTimer(m_pTimer);
                }
                typename std::decay<Fn>::type m_fn;
                ITimer* m_pTimer;
                std::shared_ptr<MyHandler> m_pSharedThis;
                AnyDataType m_data;
            };
            auto handler = std::make_shared<MyHandler>(std::forward<Fn>(func), nullptr, std::forward<AnyDataType>(data));
            handler->m_pTimer = timersys->CreateTimer(handler.get(), fInterval, nullptr, flags);
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<ITimer>(handler, handler->m_pTimer);
        }

        inline void KillTimer(const std::weak_ptr<ITimer> &timer)
        {
            if(auto sp = timer.lock())
                timersys->KillTimer(sp.get());
        }

        inline void TriggerTimer(const std::weak_ptr<ITimer> &timer, bool reset = false)
        {
            if (auto sp = timer.lock())
                timersys->FireTimerOnce(sp.get(), reset);
        }

        // Not implemeted. 
        template<class Fn>
        std::weak_ptr<ITimer> CreateDataTimer(float fInterval, Fn&& func, Handle_t& datapack, int flags = 0) = delete;

        template<class Fn, class AnyDataType = std::nullptr_t>
        std::shared_ptr<ITimer> CreateTimerRAII(float fInterval, Fn&& func, AnyDataType&& data = nullptr, int flags = 0)
        {
            auto sp = CreateTimer(fInterval, std::forward<Fn>(func), data, flags).lock();
            return std::shared_ptr<ITimer>(sp.get(), [sp](ITimer* timer) { timersys->KillTimer(timer); });
        }
	}
}