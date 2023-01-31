#include "Provider.h"

// #define LOG_DEBUG

#include "Logger.h"
#include "Application.h"
#include "tinyRpcZookeeperCli.h"
#include "InetAddress.h"
#include "TcpServer.h"
#include "rpcHeader.pb.h"
#include <functional>
#include <google/protobuf/descriptor.h>

using namespace tinyRpc;
using namespace asyncLogger;

void Provider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    std::string service_name = pserviceDesc->name();
    int methodCnt = pserviceDesc->method_count();

    for (int i = 0; i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        serviceInfo.methodMap_.insert({method_name, pmethodDesc});
    }

    serviceInfo.service_ = service;
    serviceMap_.insert({service_name, std::move(serviceInfo)});
}

void Provider::Run()
{
    std::string ip = Application::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(Application::GetInstance().GetConfig().Load("rpcsercerport").c_str());
    InetAddress address(port, ip);

    TcpServer server(&loop_, address, "RpcProvider");
    // server.setThreadNum(std::thread::hardware_concurrency());
    server.setConnectionCallback(std::bind(&Provider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&Provider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // register services to zookeeper
    ZkClient zkCli;
    zkCli.Start(); // heartbeat(ping) in every 1/3 time_out(10s)
    for (auto &[service_name, serviceInfo] : serviceMap_)
    {
        std::string service_path = "/" + service_name;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &[method_name, mdescriptor] : serviceInfo.methodMap_)
        {
            std::string method_path = service_path + "/" + method_name;
            std::string method_path_data = ip + ":" + std::to_string(port);
            zkCli.Create(method_path.c_str(), method_path_data.c_str(), method_path_data.size(), ZOO_EPHEMERAL); // ephemeral znode
        }
    }

    log_info("Start to Run RpcProvider in {}:{}", ip, port);
    server.start();
    loop_.loop();
}

void Provider::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

void Provider::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp)
{
    size_t bufReadSize = buffer->readableBytes();

    if (bufReadSize < 4)
    {
        log_warn("buffer is not enough to parse header_size for size:{}", bufReadSize);
        return;
    }
    // parse header_size
    uint32_t header_size = 0;
    memcpy(&header_size, buffer->peek(), 4);

    if (header_size + 4 > bufReadSize)
    {
        log_warn("buffer is not enough to parse rpc_header_str for bufSize:{} and header_size:{}", bufReadSize, header_size);
        return;
    }

    // parse rpc_header
    std::string rpc_header_str(buffer->peek() + 4, header_size);

    tinyRpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size = 0;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        log_error("onMessage::rpcHeader.ParseFromString failed for parse rpc_header_str={}", rpc_header_str);
        return;
    }

    // parse args
    if (4 + header_size + args_size > bufReadSize)
    {
        log_warn("buffer is not enough to parse args_str for bufSize:{} and args_size:{}", bufReadSize, args_size);
        return;
    }
    std::string args_str(buffer->peek() + 4 + header_size, args_size);
    buffer->retrieve(4 + header_size + args_size);

    log_debug("====================================");
    log_debug("buffer readable size:{}", bufReadSize);
    log_debug("header_size:{}", header_size);
    log_debug("rpc_header_str:{}", rpc_header_str);
    log_debug("service_name:{}", service_name);
    log_debug("method_name:{}", method_name);
    log_debug("args_str:{}", args_str);
    log_debug("====================================\n");

    // find iterator
    auto it_service = serviceMap_.find(service_name);
    if (it_service == serviceMap_.end())
    {
        log_error("onMessage::serviceMap_ not found service:{}", service_name);
        return;
    }
    auto it_method = it_service->second.methodMap_.find(method_name);
    if (it_method == it_service->second.methodMap_.end())
    {
        log_error("onMessage::serviceMap_ service:{} not found method:{}", service_name, method_name);
        return;
    }

    // parse args_str
    google::protobuf::Service *service = it_service->second.service_;
    const google::protobuf::MethodDescriptor *method = it_method->second;
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        log_error("onMessage::request.ParseFromString failed for parse args_str={}", args_str);
        return;
    }

    // call method
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    google::protobuf::Closure *done = google::protobuf::NewCallback<Provider, const TcpConnectionPtr &, google::protobuf::Message *>(this, &Provider::SendRpcResponse, conn, response);
    service->CallMethod(method, nullptr, request, response, done);
}

void Provider::SendRpcResponse(const TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        log_error("serialize reponse_str error");
    }
    conn->shutdown();
}