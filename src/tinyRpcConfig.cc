#include "tinyRpcConfig.h"

#include <fstream>
#include <string>
#include "Logger.h"

using namespace tinyRpc;
using namespace asyncLogger;

void tinyRpcConfig::LoadConfig(const char *config_file)
{

    std::ifstream fin(config_file, std::ios::in);
    if (!fin.is_open())
    {
        fin.close();
        log_fatal("Can not open file {}", config_file);
        exit(EXIT_FAILURE);
    }
    string line;
    while (getline(fin, line))
    {
        if (!commentCheck(line))
            parseConfigLine(line);
    }

    fin.close();
}

std::string tinyRpcConfig::Load(const std::string &key) const
{
    auto it = configMap_.find(key);
    if (it == configMap_.end())
        return "";
    return it->second;
}

void tinyRpcConfig::parseConfigLine(std::string_view line)
{
    auto pos = line.find('=');
    if (std::string::npos == pos)
    {
        log_error("can't find '=' when parse config line:{}", line);
        return;
    }
    std::string_view rawK = line.substr(0, pos);
    std::string_view rawV = line.substr(pos + 1, line.size() - pos - 1);
    log_debug("parse line:{}", line);
    log_debug("rawK:{}", rawK);
    log_debug("rawV:{}", rawV);
    std::string key = trimString(rawK);
    std::string value = trimString(rawV);
    log_debug("key:{}", key);
    log_debug("value:{}", value);
    if (key != "")
        configMap_.insert({key, value});
}

std::string tinyRpcConfig::trimString(std::string_view str) const
{
    auto pos = str.size() - 1;
    while (pos >= 0 && std::isspace(static_cast<unsigned char>(str[pos])))
    {
        pos -= 1;
    }
    if (pos < 0)
        return "";
    str = str.substr(0, pos + 1);
    pos = 0;
    while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos])))
    {
        pos += 1;
    }
    if (pos >= str.size())
        return "";

    return std::string(str.substr(pos, str.size() - pos));
}

bool tinyRpcConfig::commentCheck(const std::string &str) const
{
    long index = 0;
    while (index < str.size() && str[index] == ' ')
        index += 1;
    return (index == str.size()) || (str[index] == '#');
}