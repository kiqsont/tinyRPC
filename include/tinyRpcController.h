#ifndef KIQSONT_TINYRPC_CONTROLLER
#define KIQSONT_TINYRPC_CONTROLLER

#include <string>
#include <google/protobuf/service.h>
#include <future>
#include <memory>

namespace tinyRpc
{
    class tinyRpcController : public google::protobuf::RpcController
    {
    public:
        tinyRpcController() = default;
        virtual void Reset() override;
        virtual bool Failed() const override;
        virtual std::string ErrorText() const override;
        virtual void StartCancel() override;
        virtual void SetFailed(const std::string &reason) override;
        virtual bool IsCanceled() const override;
        virtual void NotifyOnCancel(google::protobuf::Closure *callback) override;
        void notify();
        void wait();
        bool isNonBlocking() const { return nonBlocking_; }

    private:
        bool failed_ = false;
        std::string errText_;
        bool nonBlocking_ = false;
        std::unique_ptr<std::promise<void>> promise_;
    };
}

#endif // KIQSONT_TINYRPC_CONTROLLER