#include "StreamServer.h"

StreamServer::StreamServer(const std::shared_ptr<Http::Endpoint> &endpoint) :
    _endpoint{endpoint}, _interval(std::chrono::seconds(1))
{
    std::cout << "Server listening on " << _endpoint->getPort().toString() << " port" << std::endl;
}

StreamServer::~StreamServer() {
    _shutdown = true;
    std::cout << __func__ << std::endl;
}

void StreamServer::start() {
    _endpoint->serve();
}

void StreamServer::shutdown() {

}
