#include "StreamServer.h"

int main (int argc, char* argv[]) {
    Address addr{Ipv4::any(), Pistache::Port(5001)};
    const size_t th = 2;

    auto endpoint = std::make_shared<Http::Endpoint>(addr);
    auto opts = Http::Endpoint::options()
            .threads(th);
    endpoint->init(opts);
    StreamServer server(endpoint);
    server.setupSignalHandler();
    server.start();

    return 0;
}

