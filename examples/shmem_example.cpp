#include "Map.h"
#include <unistd.h>
#include <wait.h>
#include <StderrLogger.h>
#include "Logger.h"


int main() {
    logger::Logger& LOG = logger::Logger::get_instance();
    try {
        LOG.set_global_logger(create_stdout_logger(logger::Level::DEBUG));

        shmem::Map<int, shmem::string> map(4);

        int child = fork();
        if (child < 0) {
            logger::StderrLogger{logger::Level::ERROR}.error("fork failed");
        } else if (child) {
            shmem::SemLock lock(map.get_sem());
            map.insert(std::pair<int, shmem::string>(3, "tri"));
            map.insert(std::pair<int, shmem::string>(4, "chetiri"));
        } else {
            shmem::SemLock lock(map.get_sem());
            map.insert(std::pair<int, shmem::string>(1, "odin"));
            map.insert(std::pair<int, shmem::string>(2, "dva"));
            return 0;
        }
        waitpid(child, nullptr, 0);
        shmem::SemLock lock(map.get_sem());
        for (const auto &i : *map) {
            logger::debug(std::to_string(i.first) + ' ' + std::string(i.second));
        }
    } catch (shmem::Exception &e) {
        logger::StderrLogger{logger::Level::ERROR}.error(e.what());
    }
    return 0;

}

