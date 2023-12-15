#include <nlohmann/json.hpp>

#include "RTPServer.hpp"
#include "StreamWorker.h"
#include "GstreamerAdapter.h"

RTPServer::RTPServer(Address addr) :
    httpEndpoint{std::make_shared<Http::Endpoint>(addr)}
{
    std::cout << "Server listening on: " << addr.host() << " port: " << addr.port() << std::endl;
}

void RTPServer::onRequest(const Http::Request &request, Http::ResponseWriter response) {
    auto it = routerHandler.find(request.resource());
    if (it != routerHandler.end()) {
        it->second(request, &response);
    } else {
        response.send(Http::Code::Not_Found, "Not Found");
    }
}

void RTPServer::init(size_t thr) {
    auto opts = Http::Endpoint::options()
            .threads(static_cast<int>(thr));
    httpEndpoint->init(opts);
    setupRoutes();
}

void RTPServer::start() {
    httpEndpoint->setHandler(shared_from_this());
    httpEndpoint->serve();
}

void RTPServer::setupRoutes() {
    routerHandler["/heartbeat"] = [this](const Http::Request &request, Http::ResponseWriter *response) {
        handleHeartBeat(request, response);
    };
}

void RTPServer::handleHeartBeat(const Http::Request &request, Http::ResponseWriter *response) {
    nlohmann::json j;
    j["type"] = "heartbeat";
    j["data"] = {{"message", "Start Jetson Nano Server"}};

    response->send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RTPServer::play(const Rest::Request &request, Http::ResponseWriter response) {
    nlohmann::json j;
    j["type"] = "play";
    j["data"] = {{"message", "Play video successfully"}};

#ifdef ENABLE_GSTREAMER
    std::string ipAddr {"10.10.3.206"};
    unsigned short port {5000};
    StreamWorker<GstreamerAdapter> gstreamerWorker(ipAddr, port, WGstSender::STREAM_QUALITY::HIGH);
    gstreamerWorker.runCaptureStream();
#endif

#ifdef ENABLE_FFMPEG

#endif

    response.send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RTPServer::stop(const Rest::Request &request, Http::ResponseWriter response) {

}




