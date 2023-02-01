#include <iostream>
#include "ThreadPool.h"

using namespace tinyRpc;

int main()
{
    auto func1 = [](int n1, double n2)->double
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return n1 * n2;
    };
    auto func2 = [](const char* str) -> int
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "print str:" << str << " in thread:" << std::this_thread::get_id() << "\n";
        return 123;
    };

    ThreadPool pool;
    pool.Start();
    std::cout << "begin to run \n";

    auto res1 = pool.Run(func1, 2, 1.24);
    auto res2 = pool.Run(func1, 3, 3.1);
    auto res3 = pool.Run(func2, "tinyRPC::ThreadPool testing");

    std::cout << "try to get res\n";
    std::cout << "res1:" << res1->get() << "\n";
    std::cout << "res2:" << res2->get() << "\n";
    std::cout << "res3:" << res3->get() << "\n";

    std::cout << "after res\n";
    pool.ShutDown();
    std::cout << "end" << std::endl;
}