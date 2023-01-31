#ifndef KIQSONT_TINYRPC_PROVIDER
#define KIQSONT_TINYRPC_PROVIDER

#include "EventLoop.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "Timestamp.h"
#include <google/protobuf/service.h>
#include <unordered_map>

namespace tinyRpc
{
    class Provider
    {
    public:
        void NotifyService(google::protobuf::Service *service);
        void Run();

    private:
        void onConnection(const TcpConnectionPtr &conn);
        void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp);
        void SendRpcResponse(const TcpConnectionPtr &conn, google::protobuf::Message *response);

    private:
        struct ServiceInfo
        {
            google::protobuf::Service *service_;
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> methodMap_;
        };
        std::unordered_map<std::string, ServiceInfo> serviceMap_;
        EventLoop loop_;
    };
}

#endif // KIQSONT_TINYRPC_PROVIDER