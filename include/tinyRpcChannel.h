#ifndef KIQSONT_TINYRPC_CHANNEL
#define KIQSONT_TINYRPC_CHANNEL

#include <google/protobuf/service.h>
#include <memory>
#include "Buffer.h"
#include "InetAddress.h"

class EventLoop;
class EventLoopThread;

namespace tinyRpc
{
    class tinyRpcChannel : public google::protobuf::RpcChannel
    {
    public:
        tinyRpcChannel(bool nonblocking = false);

        virtual void
        CallMethod(const google::protobuf::MethodDescriptor *method,
                   google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                   google::protobuf::Message *response, google::protobuf::Closure *done) override;

        void sendBlock(const std::string& method_path, const std::string &send_rpc_str, google::protobuf::Message *response, google::protobuf::RpcController *controller);
        void sendInLoop(const std::string &method_path, const std::string &send_rpc_str, google::protobuf::Message *response, google::protobuf::RpcController *controller);
        int connectBlock(const InetAddress &serverAddr, google::protobuf::RpcController *controller);

    private:
        bool nonblocking_;
        std::unique_ptr<EventLoopThread> asyncLoop_;
        EventLoop *loop_;
    };
}
#endif // KIQSONT_TINYRPC_CHANNEL