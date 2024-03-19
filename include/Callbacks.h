#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <functional>
#include <memory>
using std::function;
using std::shared_ptr;

class Buffer;
class TcpConnection;
class Timestamp;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using ConnectionCallback = function<void(const TcpConnectionPtr &)>;
using CloseCallback = function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = function<void(const TcpConnectionPtr &)>;
using MessageCallback = function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;
using HighWaterMarkCallback = function<void(const TcpConnectionPtr &, size_t)>;

#endif