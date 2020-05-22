#include "HttpRequest.h"

#include <iostream>
#include <regex>
#include <utility>

namespace tp {
namespace http {
HttpRequest::HttpRequest(std::string str) : _str(std::move(str)) {}

std::vector<std::string> HttpRequest::split(const std::string& str, const std::string& delim) {
    std::vector<std::string> result;

    size_t prev = 0;
    size_t pos = str.find(delim);
    bool is_empty_line = false;
    while (pos != std::string::npos) {
        if (pos == prev) {
            is_empty_line = true;
            break;
        }
        result.emplace_back(str.substr(prev, pos - prev));
        prev = pos + 2;
        pos = str.find(delim, prev);
    }
    if (is_empty_line) {
        return result;
    }

    return std::vector<std::string>();
}
std::pair<std::string, std::vector<std::string>> HttpRequest::split_line(const std::string& str,
                                                                         const std::string& delim) {
    size_t pos = str.find(delim);
    if (pos == std::string::npos) {
        return std::pair<std::string, std::vector<std::string>>();
    }
    std::string header = str.substr(0, pos);

    static const std::regex re("; ");
    std::vector<std::string> values(std::sregex_token_iterator{str.begin() + pos + 2, str.end(), re, -1},
                                    std::sregex_token_iterator{});

    return std::move(std::pair<std::string, std::vector<std::string>>{std::move(header), std::move(values)});
}
void HttpRequest::parse() {
    std::string delimiter = "\r\n";
    std::vector<std::string> lines = split(_str, delimiter);
    if (lines.empty()) {
        correct_request = false;
        return;
    }

    size_t pos = lines[0].find(" /");
    if (pos == std::string::npos) {
        correct_request = false;
        return;
    }

    method = lines[0].substr(0, pos);
    if (method != "GET" && method != "POST") {
        correct_request = false;
        return;
    }

    pos = lines[0].find(" HTTP/1");
    if (pos == std::string::npos) {
        correct_request = false;
        return;
    }

    uri = _str.substr(method.size() + 1, pos - method.size() - 1);
    version = _str.substr(pos + 6, 3);

    for (auto line = lines.begin() + 1; line != lines.end(); ++line) {
        auto pair = split_line(*line, ": ");
        if (pair.first.empty() || pair.second.empty()) {
            correct_request = false;
            return;
        }
        headers.insert(pair);
    }

    for (auto& it : headers["Connection"]) {
        if (it == "Keep-Alive") {
            is_continue = true;
        }
    }

    // TODO: get params from uri or in body
}
}  // namespace http
}  // namespace tp
