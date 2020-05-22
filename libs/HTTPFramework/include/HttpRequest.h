#ifndef TECHNOPARK_HTTPREQUEST_H
#define TECHNOPARK_HTTPREQUEST_H

#include <Server.h>
#include <map>
#include <string>
#include <vector>

namespace tp {
namespace http {

class HttpRequest {
 public:
    explicit HttpRequest(std::string str);
    void parse();

 private:
    friend class Server;
    std::vector<std::string> split(const std::string& str, const std::string& delim);
    std::pair<std::string, std::vector<std::string>> split_line(const std::string& str,
                                                                       const std::string& delim);

 private:
    std::string _str;
    std::string version;
    std::string method;
    std::string uri;
    std::map<std::string, std::string> params;
    std::map<std::string, std::vector<std::string>> headers;
    std::string body;
    bool is_continue = false;
    bool correct_request = true;
};

}  // namespace http
}  // namespace tp

#endif  // TECHNOPARK_HTTPREQUEST_H
