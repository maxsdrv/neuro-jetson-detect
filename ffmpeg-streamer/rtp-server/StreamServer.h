#ifndef VIDEO_STREAMER_STREAMSERVER_H
#define VIDEO_STREAMER_STREAMSERVER_H

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;

class StreamServer {
public:
    explicit StreamServer(const std::shared_ptr<Http::Endpoint> &endpoint);
    ~StreamServer();

    void start();
    void shutdown();

private:
    std::shared_ptr<Http::Endpoint> _endpoint;
    std::unique_ptr<std::jthread> _thread;
    std::chrono::seconds _interval;
    std::atomic<bool> _shutdown;
};


#endif //VIDEO_STREAMER_STREAMSERVER_H
