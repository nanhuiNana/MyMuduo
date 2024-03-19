# MyMuduo
一个基于c++11实现的muduo网络库
## 项目描述
学习参考muduo网络库源码，使用C++11对muduo网络库进行重构，去除muduo对boost的依赖，并根据muduo的设计思想，实现基于多Reactor多线程模型的网络库，项目重点实现了TcpServer（Tcp服务器模块）、Acceptor（接收连接模块）、TcpConnection（Tcp连接模块）、EventLoop（事件循环模块）、Channel（通道模块）、Poller/EpollPoller（IO多路复用模块）、EventLoopThread（事件循环线程模块）、EventLoopThreadPool（事件循环线程池模块）、Buffer（缓冲区模块）、Logger（日志模块）等主要模块
## 项目实现
* 整体采用 *non-blocking + IO-multiplexing* 的设计框架，其中线程模型采用 *one loop per thread* 的多线程网络编程模型，结合主从Reactor模型进行实现。 
* 使用C++标准去掉了Muduo库中对Boost库的依赖：
    * 遵循RAII机制使用智能指针对内存资源进行管理，减小内存泄漏的风险，同时通过引用计数的方式对某些资源的生存周期进行管理或延长等。
    * 使用原子操作类型：atomic类型对相关成员变量进行保护，。。。
* 设计可自增长的用户缓冲区对接受数据和发送数据进行缓存，对外表现为一块连续的内存，以方便用户代码的编写