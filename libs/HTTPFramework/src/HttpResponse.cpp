#include <HttpResponse.h>
#include <string>
#include <utility>

namespace tp {
namespace http {
HttpResponse::HttpResponse(std::string str) : _response(std::move(str)){}
const char* HttpResponse::get() { return _response.c_str(); }
}  // namespace http
}  // namespace tp