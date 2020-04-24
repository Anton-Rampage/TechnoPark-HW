#include "Map.h"
#include <unistd.h>
#include <wait.h>
#include <StderrLogger.h>
#include "Logger.h"


int main() {
    logger::Logger& LOG = logger::Logger::get_instance();
    try {
        LOG.set_global_logger(create_stdout_logger(logger::Level::DEBUG));

        shmem::Map<int, shmem::string> map(1024);

        pid_t child = fork();
        if (child < 0) {
            throw shmem::Exception("fork failed");
        } else if (child) {
            shmem::SemLock lock(map.get_sem());
            map[3] = "tri";
            map[4] = "chetiri";
        } else {
            shmem::SemLock lock(map.get_sem());
            map[1] = "odin";
            map[2] = "dva";
            return 0;
        }
        waitpid(child, nullptr, 0);
        shmem::SemLock lock(map.get_sem());
        for (const auto& [key, value] : map) {
            logger::debug(std::to_string(key) + ' ' + std::string(value));
        }
    } catch (shmem::Exception &e) {
        logger::StderrLogger{logger::Level::ERROR}.error(e.what());
    }
    return 0;

}

