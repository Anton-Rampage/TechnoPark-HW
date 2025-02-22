#ifndef PROJECT_BASEEXCEPTION_H
#define PROJECT_BASEEXCEPTION_H

#include <exception>
#include <string>

namespace tp {
class BaseException : public std::exception {
 public:
    explicit BaseException(std::string error);
    [[nodiscard]] const char* what() const noexcept override;

 private:
    std::string _error;
};
}  // namespace tp

#endif  // PROJECT_BASEEXCEPTION_H
