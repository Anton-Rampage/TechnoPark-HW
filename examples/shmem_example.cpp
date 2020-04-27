#include "Map.h"
#include <unistd.h>
#include <wait.h>
#include <StderrLogger.h>
#include "Logger.h"


int main() {
    shmem::SharedLinearMemory::get_instance(1024);
    logger::Logger &LOG = logger::Logger::get_instance();
    try {
        LOG.set_global_logger(create_stdout_logger(logger::Level::DEBUG));

        shmem::Map<int, shmem::string> map;

        pid_t child = fork();
        if (child < 0) {
            throw shmem::Exception("fork failed");
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
        for (const auto&[key, value] : map) {
            logger::debug(std::to_string(key) + ' ' + std::string(value));
        }
    } catch (shmem::Exception &e) {
        logger::StderrLogger{logger::Level::ERROR}.error(e.what());
    }
    return 0;

}

