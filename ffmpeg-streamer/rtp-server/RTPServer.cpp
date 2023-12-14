#include <nlohmann/json.hpp>

#include "RTPServer.hpp"
#include "StreamWorker.h"
#include "GstreamerAdapter.h"

RTPServer::RTPServer(Address addr) :
    httpEndpoint{std::make_shared<Http::Endpoint>(addr)} {

    std::cout << "Server listening on: " << addr.host() << " port: " << addr.port() << std::endl;
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

void RTPServer::setupRoutes() {
    /*Rest::Routes::Get(router, "/heartbeat", [this](const Rest::Request& request, Http::ResponseWriter response){
        handleHeartBeat(request, std::move(response));
        return Rest::Route::Result::Ok;
    });*/
    Rest::Routes::Get(router, "/heartbeat", Rest::Routes::bind(&RTPServer::handleHeartBeat, this));
    Rest::Routes::Get(router, "/play", Rest::Routes::bind(&RTPServer::play, this));
    Rest::Routes::Get(router, "/stop", Rest::Routes::bind(&RTPServer::stop, this));
}

void RTPServer::handleHeartBeat(const Rest::Request &request, Http::ResponseWriter response) {
    {
        std::lock_guard<std::mutex> lock(_mtx);
        /*nlohmann::json j;
        j["type"] = "heartbeat";
        j["data"] = {{"message", "Start Jetson Nano Server"}};*/

//    response.send(Http::Code::Ok, j.dump(), MIME(Application, Json));
        response.send(Http::Code::Ok, "Response is OK.");
    }
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




