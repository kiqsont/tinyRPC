#include "tinyRpcController.h"
#include "Logger.h"

using namespace tinyRpc;
using namespace asyncLogger;

void tinyRpcController::Reset()
{
    failed_ = false;
    errText_ = "";
    if (nonBlocking_)
    {
        errText_ = "response is not ready";
        promise_ = std::make_unique<std::promise<void>>();
    }
}

bool tinyRpcController::Failed() const
{
    return failed_;
}

std::string tinyRpcController::ErrorText() const
{
    return errText_;
}

// notify
void tinyRpcController::StartCancel()
{
    notify();
}

void tinyRpcController::SetFailed(const std::string &reason)
{
    failed_ = true;
    errText_ = reason;
}

bool tinyRpcController::IsCanceled() const
{
    return false;
}

// set to nonblocking
void tinyRpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
    nonBlocking_ = true;
    errText_ = "response is not ready";
    promise_ = std::make_unique<std::promise<void>>();
}

void tinyRpc::tinyRpcController::notify()
{
    if (nonBlocking_)
    {
        log_debug("promise notify");
        promise_->set_value();
    }
}

void tinyRpc::tinyRpcController::wait()
{
    if (nonBlocking_)
    {
        auto sp = promise_->get_future().share();
        sp.wait();
    }
}
