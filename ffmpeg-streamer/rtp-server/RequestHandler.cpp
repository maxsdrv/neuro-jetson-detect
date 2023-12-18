#include <nlohmann/json.hpp>

#include "RequestHandler.hpp"
#include "StreamWorker.h"
#include "GstreamerAdapter.h"

RequestHandler::RequestHandler() {
    std::cout << __func__ << std::endl;
    setupRoutes();
}

RequestHandler::~RequestHandler() {
    std::cout << __func__ << std::endl;
}

void RequestHandler::onRequest(const Http::Request &request, Http::ResponseWriter response) {
    auto it = routerHandler.find(request.resource());
    if (it != routerHandler.end()) {
        it->second(request, &response);
    } else {
        response.send(Http::Code::Not_Found, "Not Found");
    }
}

void RequestHandler::setupRoutes() {
    routerHandler["/heartbeat"] = [this](const Http::Request &request, Http::ResponseWriter *response) {
        handleHeartBeat(request, response);
    };

    routerHandler["/play"] = [this](const Http::Request &request, Http::ResponseWriter *response) {
        play(request, response);
    };
}

void RequestHandler::handleHeartBeat(const Http::Request &request, Http::ResponseWriter *response) {
    nlohmann::json j;
    j["type"] = "heartbeat";
    j["data"] = {{"message", "Start Jetson Nano Server"}};

    response->send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RequestHandler::play(const Http::Request &request, Http::ResponseWriter *response) {
    nlohmann::json j;
    j["type"] = "play";
    j["data"] = {{"message", "Play video successfully"}};

#ifdef ENABLE_GSTREAMER
    std::string ipAddr {"10.10.3.206"};
    unsigned short port {5000};
    StreamWorker<GstreamerAdapter> gstreamerWorker(ipAddr, port, WGstSender::STREAM_QUALITY::HIGH);
    gstreamerWorker.runCaptureStream();
#endif

    response->send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RequestHandler::stop(const Http::Request &request, Http::ResponseWriter *response) {

}

void RequestHandler::onTimeout(const Http::Request &request, Http::ResponseWriter response) {
    response
    .send(Http::Code::Request_Timeout, "Timeout")
    .then([=](ssize_t) {}, PrintException());
}









