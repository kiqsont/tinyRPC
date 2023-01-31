#ifndef KIQSONT_TINYRPC_ZOOKEEPER_CLIENT
#define KIQSONT_TINYRPC_ZOOKEEPER_CLIENT

#include <string>
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include "noncopyable.h"

namespace tinyRpc
{
    class ZkClient : public noncopyable
    {
    public:
        ZkClient() = default;
        ~ZkClient() noexcept;

    public:
        void Start();
        void Create(const char *path, const char *data, int dataLen, int state = 0);
        std::string GetData(const char *path);

    private:
        zhandle_t *zhandle_ = nullptr;
    };
}

#endif // KIQSONT_TINYRPC_ZOOKEEPER_CLIENT