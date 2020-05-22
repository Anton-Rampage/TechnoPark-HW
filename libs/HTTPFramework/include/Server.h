#ifndef TECHNOPARK_SERVER_H
#define TECHNOPARK_SERVER_H

#include "Descriptor.h"
#include "Coroutine.h"
#include "HttpRequest.h"
#include <HttpResponse.h>

#include <thread>
#include <atomic>
#include <functional>
#include <csignal>


namespace tp {
namespace http {

class HttpRequest;
class HttpResponse;


class Server {
 public:
    typedef std::function<void(int)> handler;
    explicit Server(size_t number_of_thread = 4);
    virtual ~Server();

    void run();
    static auto& is_opened();
    void stop();

 private:

    static void signal_handler(int signal) {
        Server::is_opened = 0;
    }
    virtual HttpResponse on_request(const HttpRequest& req);
    void loop();
    void epoll_action(int epoll_fd, uint32_t cor, int fd, uint32_t events, int action);
    void connection_routine(int epoll_fd, int server_fd);
    Descriptor create_server();



 private:

    std::vector<std::thread> _thread_pool;
    size_t _number_of_threads;

};


}  // namespace http
}  // namespace tp

#endif  // TECHNOPARK_SERVER_H
