#include "Application.h"
#include "proto/User.pb.h"
#include "tinyRpcChannel.h"
#include "tinyRpcController.h"

#include <iostream>

int main(int argc, char **argv)
{
    tinyRpc::Application::Init(argc, argv);
    protoTest1::UserServiceRPC_Stub stub(new tinyRpc::tinyRpcChannel(true));

    // for Login
    protoTest1::LoginRequest request1;
    request1.set_username("userA");
    request1.set_password("123456");
    protoTest1::LoginResponse response1;
    tinyRpc::tinyRpcController controller1;

    // for Register
    protoTest1::RegisterRequest request2;
    request2.set_username("userB");
    request2.set_password("admin");
    request2.set_email("abcd@efg.com");
    request2.set_sex(protoTest1::RegisterRequest_Sex::RegisterRequest_Sex_MALE);
    request2.set_age(20);
    protoTest1::RegisterResponse response2;
    tinyRpc::tinyRpcController controller2;

    // call method in nonblocking
    stub.Login(&controller1, &request1, &response1, nullptr);
    stub.Register(&controller2, &request2, &response2, nullptr);

    // wait for method call done
    controller1.wait();
    controller2.wait();

    // handle Login call
    if (controller1.Failed())
    {
        std::cout << controller1.ErrorText() << std::endl;
    }
    else
    {
        if (!response1.success())
        {
            std::cout << "rpc login failed for err code:" << response1.errinfo().code() << " ,and errmsg:" << response1.errinfo().errmsg() << std::endl;
        }
        else
        {
            std::cout << "rpc login success" << std::endl;
            std::cout << "errInfo code:" << response1.errinfo().code() << ", msg:" << response1.errinfo().errmsg() << std::endl;
        }
    }

    // handle Register call
    if (controller2.Failed())
    {
        std::cout << controller2.ErrorText() << std::endl;
    }
    else
    {
        if (!response2.success())
        {
            std::cout << "rpc register failed for err code:" << response2.errinfo().code() << " ,and errmsg:" << response2.errinfo().errmsg() << std::endl;
        }
        else
        {
            std::cout << "rpc register success" << std::endl;
            std::cout << "errInfo code:" << response2.errinfo().code() << ", msg:" << response2.errinfo().errmsg() << "\n";
            std::cout << "get new account uid:" << response2.uid() << std::endl;
        }
    }
}
