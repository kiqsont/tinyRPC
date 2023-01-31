#include <iostream>
#include <string>
#include "proto/User.pb.h"

#include "Application.h"
#include "Provider.h"

class UserService : public protoTest1::UserServiceRPC
{
public:
    bool Login(std::string name, std::string pwd)
    {
        // do Login
        std::cout << "username:" << name << "\n";
        std::cout << "password:" << pwd << "\n\n";
        if (pwd == "123456")
            return true;
        else
            return false;
    }

    virtual void
    Login(::google::protobuf::RpcController *controller,
          const ::protoTest1::LoginRequest *request,
          ::protoTest1::LoginResponse *response,
          ::google::protobuf::Closure *done) override
    {
        const std::string &name = request->username();
        const std::string &pwd = request->password();

        bool success = Login(name, pwd);
        response->set_success(success);
        protoTest1::ErrInfo *errCode = response->mutable_errinfo();

        if (success)
        {
            errCode->set_code(0);
            errCode->set_errmsg("login success");
        }
        else
        {
            errCode->set_code(1);
            errCode->set_errmsg("login failed");
        }

        done->Run();
    }

    bool Register(uint32_t &uid, const std::string &username, const std::string &password, const std::string &email, bool isMale, uint32_t age)
    {
        // do Register
        uid = 9;
        std::cout << "Register user\n";
        std::cout << "Username:" << username << "\n";
        std::cout << "Password" << password << "\n";
        std::cout << "Email:" << email << "\n";
        std::cout << "Sex:" << (isMale ? "Male" : "Female") << "\n";
        std::cout << "Age:" << age << "\n";
        std::cout << std::endl;
        return true;
    }

    virtual void
    Register(::google::protobuf::RpcController *controller,
             const ::protoTest1::RegisterRequest *request,
             ::protoTest1::RegisterResponse *response,
             ::google::protobuf::Closure *done) override
    {
        uint32_t uid = 0;
        const std::string &username = request->username();
        const std::string &password = request->password();
        const std::string &email = request->email();
        bool isMale = (request->sex() == protoTest1::RegisterRequest_Sex::RegisterRequest_Sex_MALE);
        uint32_t age = request->age();

        bool success = Register(uid, username, password, email, isMale, age);

        response->set_success(success);
        response->set_uid(uid);
        protoTest1::ErrInfo *errInfo = response->mutable_errinfo();
        if (success)
        {
            errInfo->set_code(0);
            errInfo->set_errmsg("register success");
        }
        else
        {
            errInfo->set_code(1);
            errInfo->set_errmsg("register failed");
        }

        done->Run();
    }
};

int main(int argc, char **argv)
{
    tinyRpc::Application::Init(argc, argv);
    tinyRpc::Provider provider;
    provider.NotifyService(new UserService());
    provider.Run();
}