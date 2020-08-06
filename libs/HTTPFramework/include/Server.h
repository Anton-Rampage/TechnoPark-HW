#ifndef TECHNOPARK_SERVER_H
#define TECHNOPARK_SERVER_H

#include "Connection.h"
#include "Coroutine.h"
#include "Descriptor.h"
#include "Epoll.h"
#include "HttpRequest.h"
#include "Logger.h"

#include <HttpResponse.h>
#include <atomic>
#include <csignal>
#include <functional>
#include <thread>

namespace tp {
namespace http {

class HttpRequest;
class HttpResponse;
class Epoll;

class Server {
 public:
    explicit Server(size_t number_of_thread = 4);
    virtual ~Server();

    void run();
    void stop();

 private:
    virtual HttpResponse on_request(const HttpRequest& req);
    void loop();
    void connection_routine(Epoll& epoll, int server_fd);
    Descriptor create_server();
    Connection accept(int server_fd);
    std::string read_full();

 private:
    std::vector<std::thread> _thread_pool;
    size_t _number_of_threads;
    logger::Logger& logger;
};

}  // namespace http
}  // namespace tp

#endif  // TECHNOPARK_SERVER_H
