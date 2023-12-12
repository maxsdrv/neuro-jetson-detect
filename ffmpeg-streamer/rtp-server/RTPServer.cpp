#include <nlohmann/json.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include "RTPServer.hpp"
#include "StreamWorker.h"

RTPServer::RTPServer(Address addr) : httpEndpoint{std::make_shared<Http::Endpoint>(addr)} {

}

void RTPServer::init(size_t thr) {
    auto opts = Http::Endpoint::options()
            .threads(static_cast<int>(thr));
    httpEndpoint->init(opts);
    setupRoutes();
}

void RTPServer::start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
}

void RTPServer::onRequest(const Http::Request &request, Http::ResponseWriter writer) {
    writer.send(Http::Code::Ok, "Server started\n");
    std::cout << "Has request.\n";
}

void RTPServer::setupRoutes() {
    Rest::Routes::Get(router, "/heartbeat", Rest::Routes::bind(&RTPServer::handleHeartBeat, this));
    Rest::Routes::Get(router, "/play", Rest::Routes::bind(&RTPServer::play, this));
    Rest::Routes::Get(router, "/stop", Rest::Routes::bind(&RTPServer::stop, this));
}

void RTPServer::handleHeartBeat(const Rest::Request &request, Http::ResponseWriter response) {
    nlohmann::json j;
    j["type"] = "heartbeat";
    j["data"] = {{"message", "Start Jetson Nano Server"}};

    response.send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RTPServer::play(const Rest::Request &request, Http::ResponseWriter response) {
    nlohmann::json j;
    j["type"] = "play";
    j["data"] = {{"message", "Play video successfully"}};

    std::string rtp_destination {"rtp://localhost:5000"};

    auto ffmpegStream = std::make_unique<StreamWorker>(rtp_destination);
    ffmpegStream->runCaptureStream();

    response.send(Http::Code::Ok, j.dump(), MIME(Application, Json));
}

void RTPServer::stop(const Rest::Request &request, Http::ResponseWriter response) {

}

/*void RTPServer::start(const std::string &addr) {
    Http::listenAndServe<RTPServer>(Address(addr.c_str()));
}*/


