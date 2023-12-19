#ifndef VIDEO_STREAMER_STREAMSERVER_H
#define VIDEO_STREAMER_STREAMSERVER_H

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "RequestHandler.hpp"

using namespace Pistache;

class StreamServer {
public:
    explicit StreamServer(const std::shared_ptr<Http::Endpoint> &endpoint);
    ~StreamServer();

    void start();
    void shutdown();
    void setupSignalHandler();
    void waitForSignal();

private:
    std::shared_ptr<Http::Endpoint> _endpoint;
    std::unique_ptr<std::jthread> _mainThread;
    std::unique_ptr<std::jthread> _signalThread;
    std::chrono::seconds _interval;
    std::atomic<bool> _shutdown;
    TaskManager<std::function<void(std::stop_token)>> _threadManager;

    void run();
};


#endif //VIDEO_STREAMER_STREAMSERVER_H
