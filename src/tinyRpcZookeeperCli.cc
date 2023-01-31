#include "tinyRpcZookeeperCli.h"

#include "Application.h"
#include "Logger.h"

using namespace tinyRpc;
using namespace asyncLogger;

static void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::~ZkClient() noexcept
{
    if (nullptr != zhandle_)
    {
        zookeeper_close(zhandle_);
    }
}

void ZkClient::Start()
{
    // load config
    std::string host = Application::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = Application::GetInstance().GetConfig().Load("zookeeperport");
    std::string connStr = host + ":" + port;

    zhandle_ = zookeeper_init(connStr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    // time_out:30s
    // it calls pthread_create to poll for web IO and watcher callback
    if (nullptr == zhandle_)
    {
        log_fatal("zookeeper_init error"); // abort
    }

    // semaphore to wait for zoo_session_event and connected
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem);

    sem_wait(&sem);
    log_info("zookeeper_init success");
}

void ZkClient::Create(const char *path, const char *data, int dataLen, int state)
{
    char path_buf[128]{0};
    int bufLen = sizeof path_buf;
    int flag = 0;
    flag = zoo_exists(zhandle_, path, 0, nullptr);
    if (ZNONODE == flag) // create a new node when node is not exist
    {
        flag = zoo_create(zhandle_, path, data, dataLen, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, bufLen);
        if (ZOK == flag)
        {
            log_info("created an znode with path:{}", path);
        }
        else
        {
            log_fatal("znode create error for flag:{} in path:{}", flag, path); // abort
        }
    }
}

std::string ZkClient::GetData(const char *path)
{
    char buf[64]{0};
    int bufLen = sizeof buf;
    int flag = zoo_get(zhandle_, path, 0, buf, &bufLen, nullptr);
    if (ZOK != flag)
    {
        log_error("get znode error in path:{}", path);
        return "";
    }
    else
    {
        return buf;
    }
}
