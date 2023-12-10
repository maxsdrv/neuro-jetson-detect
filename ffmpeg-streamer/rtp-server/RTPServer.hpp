#ifndef FFMPEG_STREAMER_RTPSERVER_HPP
#define FFMPEG_STREAMER_RTPSERVER_HPP

#include <pistache/endpoint.h>
#include <pistache/router.h>

using namespace Pistache;

class RTPServer : public Http::Handler {
public:
    explicit RTPServer(Address addr);

    HTTP_PROTOTYPE(RTPServer)
    void onRequest(const Http::Request &request, Http::ResponseWriter writer) override;

    void init(size_t thr = 2);
    void start();

private:
    void  setupRoutes();
    void handleHeartBeat(const Rest::Request& request, Http::ResponseWriter response);

private:
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

};


#endif //FFMPEG_STREAMER_RTPSERVER_HPP
