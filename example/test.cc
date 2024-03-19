#include <functional>
#include <mymuduo/TcpServer.h>
#include <string>
using std::bind;
using std::string;
using namespace std::placeholders;
class EchoServer {
public:
    EchoServer(EventLoop *loop, const InetAddress &addr, const string &name) : loop_(loop), server_(loop, addr, name) {
        server_.setConnectionCallback(bind(&EchoServer::onConnection, this, _1));
        server_.setMessageCallback(bind(&EchoServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(3);
    }
    void start() {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG_INFO("Connection UP : %s\n", conn->peerAddress().toIpPort().c_str());
        } else {
            LOG_INFO("Connection DOWN : %s\n", conn->peerAddress().toIpPort().c_str());
        }
    }
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
        string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }
    EventLoop *loop_;
    TcpServer server_;
};

int main() {
    EventLoop baseloop;
    InetAddress addr(8888);
    EchoServer server(&baseloop, addr, "server");
    server.start();
    baseloop.loop();
    return 0;
}