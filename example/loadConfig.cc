#include "Application.h"

#include <iostream>

int main(int argc, char **argv)
{
    tinyRpc::Application::Init(argc, argv);
    auto map = tinyRpc::Application::GetConfig().configMap();
    for (const auto &[key, value] : map)
    {
        std::cout << key << ":" << value << "\n";
    }

    std::cout << std::endl;
}