
#include <thread>
#include <functional>
#include <chrono>
#include <utility>
#include <memory>
#include <queue>
#include <list>
#include <vector>

#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>

#include "noncopyable.h"

namespace tinyRpc
{
    class ThreadPool : public noncopyable
    {
    public:
        using PoolSeconds = std::chrono::seconds;

        // configure for thread pool
        struct ThreadPoolConfig
        {
            size_t core_threads     = std::thread::hardware_concurrency();
            size_t max_threads      = static_cast<size_t>(1.5 * core_threads);
            size_t max_tasks        = 128;
            PoolSeconds time_out    = std::chrono::seconds(1);
        };

        // thread state
        enum class ThreadState
        {
            kInit       = 0,
            kWaiting    = 1,
            kRunning    = 2,
            kStop       = 3
        };

        // thread flag
        enum class ThreadFlag
        {
            kInit   = 0,
            kCore   = 1,
            kCache  = 2
        };

        using ThreadPtr         = std::shared_ptr<std::thread>;
        using THreadId          = std::atomic_int;
        using ThreadStateAtomic = std::atomic<ThreadState>;
        using ThreadFlagAtomic  = std::atomic<ThreadFlag>;

        struct ThreadWrapper
        {
            ThreadPtr ptr = nullptr;
            THreadId  id  = 0;
            ThreadStateAtomic state{ThreadState::kInit};
            ThreadFlagAtomic flag{ThreadFlag::kInit};
        };

        using ThreadWrapperPtr  = std::shared_ptr<ThreadWrapper>;
        using ThreadPoolLock    = std::unique_lock<std::mutex>;

    public:
        ThreadPool();
        ThreadPool(ThreadPoolConfig config);
        ~ThreadPool() noexcept;

        bool Start();
        void ShutDown();
        void ShutDownNow();
        
        // result_of is deprecated since C++17
        template<typename F,typename... Args>
        auto Run(F&& f,Args&& ...args) -> 
            std::shared_ptr<std::future<std::invoke_result_t<F,Args...>>>
        {
            if(!this->is_available.load() || this->is_shutdown.load() || this->is_shutdown_now.load())
                return nullptr;
            
            if(total_function_num.load() < m_Config.max_threads && waiting_thread_num.load() == 0)
                AddThread(GetNextThreadID(), ThreadFlag::kCache);

            using return_type = std::invoke_result_t<F, Args...>;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f),std::forward<Args>(args)...)
            );
            total_function_num += 1;
            std::future<return_type> res = task->get_future();
            {
                ThreadPoolLock lock(this->m_task_mutex);
                this->m_tasks.emplace([task](){ (*task)(); });
            }
            this->m_task_cv.notify_one();
            return std::make_shared<std::future<return_type>>(std::move(res));
        }

    private:
        bool IsVaildConfig(ThreadPoolConfig config);
        void AddThread(int threadId, ThreadFlag flag = ThreadFlag::kCore);
        int GetNextThreadID();
        void ShutDown(bool is_now);

    private:
        ThreadPoolConfig m_Config;
        std::list<ThreadWrapperPtr> m_worker_threads;
        std::queue<std::function<void()>> m_tasks;
        std::mutex m_task_mutex;
        std::condition_variable m_task_cv;

        std::atomic_int total_function_num{0};
        std::atomic_int waiting_thread_num{0};
        std::atomic_int thread_id{0};

        std::atomic_bool is_shutdown_now{false};
        std::atomic_bool is_shutdown{false};
        std::atomic_bool is_available{true};
    };
}