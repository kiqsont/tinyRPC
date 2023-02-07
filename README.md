# C++ Tiny RPC


| Part Ⅰ | Part Ⅱ| Part Ⅲ |
| :----: |:----:| :----:   |
| [项目背景](#项目背景) | [项目代码流程](#项目代码流程) | [运行案例](#运行案例) |



项目背景
----------

本项目是由C++17开发的一个轻量级RPC框架，基于施磊老师的开源项目mqrpc,网络通信服务基于 muduo 库重写的 tinyWebLib 实现，服务端与客户端间数据的交互由 protobuf 进行序列化和反序列化，服务配置中心使用 zookeeper，配置文件读取键值对进行加载。    
本项目对接受rpc请求数据进行可靠性优化，添加了非阻塞rpc调用以增强实用性，增加可扩展的线程池以提高易用性。


项目代码流程
----

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/flowchart.png)


运行案例
----
> 安装好 zookeeper 和 protobuf    
> 运行 autobuild.sh 构建项目    
> 在zookeeper/bin下开启 zkServer.sh start

调用部分测试程序需要提供配置文件参数

- Login/Register，RPC调用本地机器服务

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/login_and_register.png)

- getFriends，RPC调用本地机器服务

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/get_friends.png)

- 配置文件读取

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/loadConfig.png)

- ThreadPool使用实例

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/threadPool_testing.png)

- Login/Register，RPC调用远程华为云服务

![](./%E9%A1%B9%E7%9B%AE%E4%BB%8B%E7%BB%8D/vm_call_huaweicloud.png)
注意：要给配置文件加上bindThisMachine = true，不然直接绑定公网IP会bind失败