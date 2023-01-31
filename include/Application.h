#ifndef KIQSONT_TINYRPC_APPLICATION
#define KIQSONT_TINYRPC_APPLICATION

#include "tinyRpcConfig.h"

namespace tinyRpc
{
    // for configruation init
    class Application
    {
    public:
        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

    public:
        static void Init(int argc, char **argv);
        static Application &GetInstance();
        static tinyRpcConfig &GetConfig();

    private:
        Application() = default;
        inline static tinyRpcConfig config_;
    };
}

#endif // KIQSONT_TINYRPC_APPLICATION
