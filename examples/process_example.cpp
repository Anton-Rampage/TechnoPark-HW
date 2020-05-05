#include "BaseException.h"
#include "Process.h"

#include <iostream>

int main() {
    try {
        tp::process::Process ps("./echo");
        std::string write_data{"abba"};

        for (size_t i = 0; i < 350; ++i) {
            ps.write_exact(write_data.c_str(), write_data.length());
        }
        ps.close_stdin();

        while (ps.is_readable()) {
            char read_data[256]{};
            ps.read_exact(read_data, sizeof(read_data));
            std::cout << read_data << std::endl;
        }
        ps.close();
    } catch (tp::BaseException& e) { std::cerr << e.what() << std::endl; }
    return 0;
}
