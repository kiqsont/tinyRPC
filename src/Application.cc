#include "Application.h"

#include "Logger.h"

using namespace tinyRpc;
using namespace asyncLogger;

void Application::Init(int argc, char **argv)
{
    if (argc != 2)
    {
        log_fatal("need a configuration file");
        exit(EXIT_FAILURE);
    }
    const char *config_file = argv[1];
    log_debug("config_file:{}", config_file);
    config_.LoadConfig(config_file);
}

Application &Application::GetInstance()
{
    static Application ins;
    return ins;
}

tinyRpcConfig &Application ::GetConfig()
{
    return config_;
}