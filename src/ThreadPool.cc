#include "ThreadPool.h"

// #define LOG_DEBUG

#include "Logger.h"

using namespace tinyRpc;
using namespace asyncLogger;

tinyRpc::ThreadPool::ThreadPool()
{
    m_Config = ThreadPoolConfig();
}

tinyRpc::ThreadPool::ThreadPool(ThreadPoolConfig config)
    : m_Config(config)
{
    if(IsVaildConfig(m_Config))
        is_available.store(true);
    else
        is_available.store(false);
}

tinyRpc::ThreadPool::~ThreadPool() noexcept
{
    ShutDown();
}

bool tinyRpc::ThreadPool::Start()
{
    if(!is_available.load())
        return false;

    size_t core_thread_num = m_Config.core_threads;
    log_debug("Try to init threadPool threads");
    while(core_thread_num-- > 0)
    {
        AddThread(GetNextThreadID());
    }
    log_debug("Init threads end");
    return true;
}

void tinyRpc::ThreadPool::ShutDown()
{
    ShutDown(false);
    log_debug("shutdown");
}

void tinyRpc::ThreadPool::ShutDownNow()
{
    ShutDown(true);
    log_debug("shutdown now!");
}

bool tinyRpc::ThreadPool::IsVaildConfig(ThreadPoolConfig config)
{
    return !(config.core_threads >= config.max_threads || config.core_threads < 1 || config.time_out.count() < 1 || config.max_threads < 1);
}

void tinyRpc::ThreadPool::AddThread(int threadId, ThreadFlag flag)
{
    log_debug("try to add thread:{} in flag:", threadId, static_cast<int>(flag));
    ThreadWrapperPtr thread_ptr = std::make_shared<ThreadWrapper>();
    thread_ptr->id = threadId;
    thread_ptr->flag.store(flag);

    // worker func
    auto workerFunc = [this, thread_ptr]()
    {
        while(true)
        {
            std::function<void()> task;

            // Critical Section take task
            {
                ThreadPoolLock lock(this->m_task_mutex);
                if(thread_ptr->state.load() == ThreadState::kStop)
                    break;

                log_debug("thread id:{} start to wait", thread_ptr->id.load());
                thread_ptr->state.store(ThreadState::kWaiting);
                ++this->waiting_thread_num;
                bool is_timeout = false;
                auto lambdaCvWait = [this, thread_ptr]
                { return (this->is_shutdown.load() 
                        || this->is_shutdown_now.load() 
                        || !this->m_tasks.empty() 
                        || thread_ptr->state == ThreadState::kStop); };

                if (thread_ptr->flag.load() == ThreadFlag::kCore)
                {
                    this->m_task_cv.wait(lock, lambdaCvWait);
                }
                else
                {
                    this->m_task_cv.wait_for(lock, this->m_Config.time_out, lambdaCvWait);
                    is_timeout = !(this->is_shutdown 
                                || this->is_shutdown_now 
                                || !this->m_tasks.empty() 
                                || thread_ptr->state.load() == ThreadState::kStop);
                }
                --this->waiting_thread_num;
                log_debug("thread id:{} wait end", thread_ptr->id.load());

                // handle time out cache thread
                if(is_timeout)
                    thread_ptr->state.store(ThreadState::kStop);
                
                // stop
                if(thread_ptr->state.load() == ThreadState::kStop)
                {
                    log_debug("thread id:{} try to stop", thread_ptr->id.load());
                    break;
                }

                // shutdown
                if(this->is_shutdown.load() && this->m_tasks.empty())
                {
                    log_debug("thread id:{} shutdown", thread_ptr->id.load());
                    break;
                }

                // shutdown now
                if(this->is_shutdown_now.load())
                {
                    log_debug("thread id:{} shutdown now", thread_ptr->id.load());
                    break;
                }

                // take task
                thread_ptr->state.store(ThreadState::kRunning);
                task = std::move(this->m_tasks.front());
                this->m_tasks.pop();
            }

            // get out the Critical Section and run task
            task();
        }
        log_debug("thread id:{} end", thread_ptr->id.load());
    };

    // detach threads
    thread_ptr->ptr = std::make_shared<std::thread>(std::move(workerFunc));
    if(thread_ptr->ptr->joinable())
        thread_ptr->ptr->detach();

    this->m_worker_threads.emplace_back(std::move(thread_ptr));
}

int tinyRpc::ThreadPool::GetNextThreadID()
{
    int id = thread_id.load();
    ++thread_id;
    return id;
}

void tinyRpc::ThreadPool::ShutDown(bool is_now)
{
    if(is_available.load())
    {
        if(is_now)
            is_shutdown_now.store(true);
        else
            is_shutdown.store(true);

        m_task_cv.notify_all();
        is_available.store(false);
    }
}
