#include <iostream>
#include "Application.h"
#include "proto/Friends.pb.h"

#include "tinyRpcChannel.h"
#include "tinyRpcController.h"
#include "ThreadPool.h"

int main(int argc, char **argv)
{
    tinyRpc::Application::Init(argc, argv);
    protoTest2::FriendsServiceRPC_Stub stub(new tinyRpc::tinyRpcChannel);

    // set call args
    protoTest2::GetFriendsRequest request;
    request.set_uid(0);
    protoTest2::GetFriendsResponse response;
    tinyRpc::tinyRpcController controller;

    tinyRpc::ThreadPool pool;
    pool.Start();

    auto ptr = pool.Run(&protoTest2::FriendsServiceRPC::GetFriends, &stub, &controller, &request, &response, nullptr);
    // stub.GetFriends(&controller, &request, &response, nullptr);

    // wait
    std::cout << "try to wait\n";
    ptr->get();

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        // show friends
        for (int i = 0; i < response.friends_size(); i++)
        {
            const protoTest2::User &friend_ = response.friends(i);

            std::cout << "username:" << friend_.name() << "\n";
            std::cout << "uid:" << friend_.uid() << "\n";
            std::cout << "sex:" << (friend_.sex() == protoTest2::User_Sex::User_Sex_MALE ? "Male" : "Female") << "\n";
            std::cout << std::endl;
        }
    }
}
