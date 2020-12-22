#pragma once

#include <atomic>
#include <tuple>
#include <utility>

class ThinkQueue
{
public:
    template<class Fn, class...BoundArgs>
    void AddTask(Fn &&fn, BoundArgs&&...args)
    {
        BaseTask * task = new Task<typename std::decay<Fn>::type, typename std::decay<BoundArgs>::type...>(std::forward<Fn>(fn), std::forward<BoundArgs>(args)...);
        AddTask(task);
    }
    
    void CallAndClear();
    void Clear();
    ~ThinkQueue() { Clear(); }

private:
    class BaseTask
    {
    public:
        virtual ~BaseTask() = default;
        virtual void operator()() && = 0; // only need to execute once

        BaseTask *next = nullptr;
    };

    template<class Fn, class...BoundArgs>
    class Task : public BaseTask
    {
    public:
        template<class FnArg, class...Args>
        explicit Task(FnArg &&f, Args &&...args) : m_func(std::forward<FnArg>(f)), m_args(std::forward<Args>(args)...) {}
        Task(const Task &) = delete;
        Task(Task &&) = delete;

        void operator()() && override {
            std::apply(std::move(m_func), std::move(m_args));
        }
        Fn m_func;
        std::tuple<BoundArgs...> m_args;
    };

    void AddTask(BaseTask * task);

    std::atomic<BaseTask *> head = nullptr;
};

inline void ThinkQueue::AddTask(ThinkQueue::BaseTask* task) {

    BaseTask* old_head = head.load();
    do
        task->next = old_head;
    while (!head.compare_exchange_weak(old_head, task));
}

inline void ThinkQueue::CallAndClear() {
    BaseTask* cur_head = head.exchange(nullptr);
    while (cur_head != nullptr) {
        std::move(*cur_head)();
        delete std::exchange(cur_head, cur_head->next);
    }
}

inline void ThinkQueue::Clear()
{
    BaseTask* cur_head = head.exchange(nullptr);
    while (cur_head != nullptr) {
        delete std::exchange(cur_head, cur_head->next);
    }
}