#include "tinyRpcChannel.h"

// #define LOG_DEBUG

#include "Logger.h"
#include "rpcHeader.pb.h"
#include "Application.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Socket.h"
#include "EventLoopThread.h"
#include <string>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace tinyRpc;
using namespace asyncLogger;

tinyRpcChannel::tinyRpcChannel(bool nonblocking) : nonblocking_(nonblocking)
{
    if (nonblocking_)
    {
        asyncLoop_ = std::make_unique<EventLoopThread>();
        loop_ = asyncLoop_->startLoop();
    }
}

void tinyRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                                google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                                google::protobuf::Message *response, google::protobuf::Closure *done)
{
    if (nonblocking_)
    {
        controller->NotifyOnCancel(nullptr);
    }

    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    std::string args_str;
    uint32_t args_size = 0;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        log_error("serializa request failed");
        controller->SetFailed("serializa request failed");
        return;
    }

    tinyRpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        log_error("serialize rpc header failed");
        controller->SetFailed("serialize rpc header failed");
        return;
    }

    std::string send_rpc_str = std::string((char *)&header_size, 4);
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // for debug
    log_debug("====================================");
    log_debug("header_size:{}", header_size);
    log_debug("rpc_header_str:{}", rpc_header_str);
    log_debug("service_name:{}", service_name);
    log_debug("method_name:{}", method_name);
    log_debug("args_str:{}", args_str);
    log_debug("send_rpc_str:{}", send_rpc_str);
    log_debug("====================================\n");

    if (!nonblocking_)
    {
        sendBlock(send_rpc_str, response, controller);
    }
    else
    {
        loop_->runInLoop(std::bind(&tinyRpc::tinyRpcChannel::sendInLoop, this, send_rpc_str, response, controller));
    }
}

void tinyRpcChannel::sendBlock(const std::string &send_rpc_str, google::protobuf::Message *response, google::protobuf::RpcController *controller)
{
    std::string ip = Application::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = static_cast<uint16_t>(std::stoi(Application::GetInstance().GetConfig().Load("rpcsercerport")));
    InetAddress serverAddr = InetAddress(port, ip);

    int clientFd = connectBlock(serverAddr, controller);
    Socket socket_(clientFd);
    if (clientFd < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (Socket::isSelfConnect(clientFd))
    {
        controller->SetFailed("connect to itself");
        log_error("connect to itself");
        return;
    }

    // send data
    size_t remainLen = send_rpc_str.size();
    std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>();
    buffer->append(send_rpc_str);
    int saveErrno = 0;
    while (remainLen != 0)
    {
        auto n = buffer->writeFd(clientFd, &saveErrno);
        if (n < 0)
        {
            controller->SetFailed("send error:" + std::to_string(saveErrno));
            log_error("send error");
            return;
        }
        remainLen -= n;
    }

    // clean buffer
    // buffer = std::make_unique<Buffer>();
    buffer->retrieveAll();
    auto n = buffer->readFd(clientFd, &saveErrno);

    if (n < 0)
    {
        controller->SetFailed("recv error:" + std::to_string(saveErrno));
        log_error("recv error:{}", saveErrno);
        return;
    }

    // std::string response_str(recv_buf, 0, recv_size); bug for '\000'
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(buffer->peek(), n))
    {
        std::string recv_str(buffer->retrieveAsString(n));
        log_error("parse error, response_str:{}", recv_str);
        controller->SetFailed("parse error, response_str:" + recv_str);
        return;
    }
}

int tinyRpc::tinyRpcChannel::connectBlock(const InetAddress &serverAddr, google::protobuf::RpcController *controller)
{
    socklen_t len = sizeof(*serverAddr.getSockAddr());

    for (int i = 0; i < 10; i++) // retry times:10
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            log_error("init client socket error");
            controller->SetFailed("init client socket error");
            continue;
        }
        int ret = ::connect(sockfd, (const sockaddr *)serverAddr.getSockAddr(), len);
        int saveErrno = (ret == 0) ? 0 : errno;
        switch (saveErrno)
        {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            return sockfd;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            ::close(sockfd);
            controller->SetFailed("connect error:" + std::to_string(saveErrno));
            log_error("connect error:{}", saveErrno);
            return -1;

        default:
            ::close(sockfd);
            controller->SetFailed("connect error:" + std::to_string(saveErrno));
            log_error("unexpected error:{}", saveErrno);
            return -1;
        }
    }
    return -1;
}

void tinyRpc::tinyRpcChannel::sendInLoop(const std::string &send_rpc_str, google::protobuf::Message *response, google::protobuf::RpcController *controller)
{
    assert(loop_ != nullptr);
    // ensure call controller->notify in nonblocking mode when leave this func
    auto defer = [](google::protobuf::RpcController *controller)
    {
        controller->StartCancel();
    };
    std::unique_ptr<google::protobuf::RpcController, decltype(defer)> guard(controller, defer);

    sendBlock(send_rpc_str, response, controller);
}