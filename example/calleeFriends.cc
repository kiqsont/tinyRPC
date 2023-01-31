#include <iostream>
#include <string>
#include <vector>
#include "proto/Friends.pb.h"

#include "Application.h"
#include "Provider.h"

struct User
{
    std::string name;
    uint32_t uid;
    bool isMale;
};

class FriendsService : public protoTest2::FriendsServiceRPC
{
public:
    std::vector<User> GetFriends(uint32_t uid)
    {
        std::cout << "query and set the data for uid:" << uid << "\n";
        std::vector<User> res;
        res.emplace_back(User{"UserA", 1, true});
        res.emplace_back(User{"UserB", 2, false});
        res.emplace_back(User{"UserC", 3, true});
        res.emplace_back(User{"UserD", 4, false});
        res.emplace_back(User{"UserE", 5, true});
        return res;
    }

    virtual void
    GetFriends(::google::protobuf::RpcController *controller,
               const ::protoTest2::GetFriendsRequest *request,
               ::protoTest2::GetFriendsResponse *response,
               ::google::protobuf::Closure *done) override
    {
        uint32_t uid = request->uid();

        std::vector<User> friends = GetFriends(uid);

        for (const auto &e : friends)
        {
            protoTest2::User *friend_ = response->add_friends();
            friend_->set_name(e.name);
            friend_->set_uid(e.uid);
            friend_->set_sex(e.isMale ? protoTest2::User_Sex::User_Sex_MALE : protoTest2::User_Sex::User_Sex_FEMALE);
        }

        done->Run();
    }
};

int main(int argc, char **argv)
{
    tinyRpc::Application::Init(argc, argv);
    tinyRpc::Provider provider;
    provider.NotifyService(new FriendsService);
    provider.Run();
}