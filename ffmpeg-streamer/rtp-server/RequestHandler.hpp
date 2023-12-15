#ifndef STREAMER_RTPSERVER_HPP
#define STREAMER_RTPSERVER_HPP

#include <mutex>
#include <unordered_map>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;

class RequestHandler : public Http::Handler {
    using RouterCallback = std::function<void(const Http::Request &request, Http::ResponseWriter *response)>;

public:
    RequestHandler();
    ~RequestHandler() override;

    HTTP_PROTOTYPE(RequestHandler)
    void onRequest(const Http::Request &request, Http::ResponseWriter response) override;
    void onTimeout(const Http::Request &request, Http::ResponseWriter response) override;

private:
    void  setupRoutes();
    void handleHeartBeat(const Http::Request& request, Http::ResponseWriter *response);
    void play(const Http::Request& request, Http::ResponseWriter *response);
    void stop(const Http::Request& request, Http::ResponseWriter *response);

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    std::unordered_map<std::string, RouterCallback> routerHandler;
};


#endif //STREAMER_RTPSERVER_HPP
