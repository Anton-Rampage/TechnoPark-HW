#ifndef TECHNOPARK_HTTPRESPONSE_H
#define TECHNOPARK_HTTPRESPONSE_H

#include <string>

namespace tp {
namespace http {
class HttpResponse {
 public:
    explicit HttpResponse(std::string str);
    [[nodiscard]] const char *get();
 private:
    friend class Server;
    std::string _response;
};
}
}

#endif  // TECHNOPARK_HTTPRESPONSE_H
