#ifndef STREAMER_RTPSERVER_HPP
#define STREAMER_RTPSERVER_HPP

#include <mutex>
#include <unordered_map>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;

class RTPServer : public Http::Handler, public std::enable_shared_from_this<RTPServer> {
    using RouterCallback = std::function<void(const Http::Request &request, Http::ResponseWriter *response)>;

public:
    explicit RTPServer(Address addr);

    HTTP_PROTOTYPE(RTPServer)
    void onRequest(const Http::Request &request, Http::ResponseWriter response) override;

    void init(size_t thr = 2);
    void start();

private:
    void  setupRoutes();
    void handleHeartBeat(const Http::Request& request, Http::ResponseWriter *response);
    void play(const Rest::Request& request, Http::ResponseWriter response);
    void stop(const Rest::Request& request, Http::ResponseWriter response);

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    std::unordered_map<std::string, RouterCallback> routerHandler;
};


#endif //STREAMER_RTPSERVER_HPP
