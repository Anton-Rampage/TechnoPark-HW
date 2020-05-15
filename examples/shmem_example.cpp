#include "BaseException.h"
#include "Logger.h"
#include "Map.h"

#include <StderrLogger.h>
#include <unistd.h>
#include <wait.h>

using tp::logger::debug;
int main() {
    tp::shmem::SharedLinearMemory::get_instance(1024);
    tp::logger::Logger &LOG = tp::logger::Logger::get_instance();
    try {
        LOG.set_global_logger(create_stdout_logger(tp::logger::Level::DEBUG));

        tp::shmem::Map<int, tp::shmem::string> map;

        pid_t child = fork();
        if (child < 0) {
            throw tp::BaseException("fork failed");
        } else if (child) {
            map[3] = "tri";
            map[4] = "chetiri";
            map[5] = "pyat`pyat`pyat`pyat`pyat`pyat`pyat`pyat`pyat`pyat`";

        } else {
            map[1] = "odin";
            map[2] = "dva";
            return 0;
        }
        waitpid(child, nullptr, 0);
        for (const auto &[key, value] : map) {
            debug(std::to_string(key) + ' ' + std::string(value));
        }
    } catch (tp::BaseException &e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
    return 0;
}
