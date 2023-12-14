#ifndef STREAMER_RTPSERVER_HPP
#define STREAMER_RTPSERVER_HPP

#include <mutex>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;

class RTPServer {
public:
    explicit RTPServer(Address addr);

    void init(size_t thr = 2);
    void start();

private:
    void  setupRoutes();
    void handleHeartBeat(const Rest::Request& request, Http::ResponseWriter response);
    void play(const Rest::Request& request, Http::ResponseWriter response);
    void stop(const Rest::Request& request, Http::ResponseWriter response);

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};


#endif //STREAMER_RTPSERVER_HPP
