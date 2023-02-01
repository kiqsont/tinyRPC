#ifndef KIQSONT_TINYRPC_CONFIG
#define KIQSONT_TINYRPC_CONFIG

#include <unordered_map>
#include <string>
#include <string_view>

namespace tinyRpc
{
    class tinyRpcConfig
    {
    public:
        void LoadConfig(const char *config_file);
        std::string Load(const std::string &key) const;
        std::string trimString(std::string_view str) const;
        const std::unordered_map<std::string, std::string> &configMap() const
        {
            return configMap_;
        }

    private:
        void parseConfigLine(std::string_view line);
        bool commentCheck(const std::string &str) const;

    private:
        std::unordered_map<std::string, std::string> configMap_;
    };
}

#endif // KIQSONT_TINYRPC_CONFIG