
#include "RTPServer.hpp"

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

}

void RTPServer::handleHeartBeat(const Rest::Request &request, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "Server is alive");
}

/*void RTPServer::start(const std::string &addr) {
    Http::listenAndServe<RTPServer>(Address(addr.c_str()));
}*/


