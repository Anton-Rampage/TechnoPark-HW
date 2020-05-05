#include "BaseException.h"

#include <cstring>

namespace tp {
BaseException::BaseException(std::string error) : _error(std::move(error)) {
    _error += std::string(" : ") + strerror(errno);
}

const char *BaseException::what() const noexcept { return _error.c_str(); }
}  // namespace tp
