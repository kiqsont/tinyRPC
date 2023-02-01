# C++ Tiny RPC

项目背景
----------

本项目是由C++17开发的一个轻量级RPC框架，网络通信服务基于 muduo 库重写的 tinyWebLib 实现，服务端与客户端间数据的交互由 protobuf 进行序列化和反序列化，服务配置中心使用 zookeeper，配置文件读取键值对进行加载。



项目构建
----
> 安装好 zookeeper 和 protobuf

> 运行 autobuild.sh 构建项目