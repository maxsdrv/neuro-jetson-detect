#ifndef VIDEO_STREAMER_TASKMANAGER_H
#define VIDEO_STREAMER_TASKMANAGER_H

#include <vector>
#include <thread>
#include <functional>
#include <memory>

template<typename Task>
class TaskManager {
public:
    ~TaskManager() {
        shutdown();
    }

    void shutdown() {
        shutdownRequested.store(true);
        for (auto& thread : _threads) {
            if (thread.joinable())  {
                thread.request_stop();
            }
        }
    }

    void addTask(Task task) {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _threads.emplace_back(std::jthread(std::move(task)));
    }

    [[nodiscard]] bool isShutdownRequested() const {
        return shutdownRequested.load();
    }

private:
    std::vector<std::jthread> _threads;
    std::mutex _mutex;
    std::atomic_bool shutdownRequested{false};
};

#endif //VIDEO_STREAMER_TASKMANAGER_H
