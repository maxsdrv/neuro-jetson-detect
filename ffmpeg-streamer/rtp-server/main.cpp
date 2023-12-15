#include <csignal>

#include "RequestHandler.hpp"
#include "StreamServer.h"

int main (int argc, char* argv[]) {
    Address addr{Ipv4::any(), Pistache::Port(5001)};
    const size_t th = 2;

    auto endpoint = std::make_shared<Http::Endpoint>(addr);
    auto opts = Http::Endpoint::options()
            .threads(th);
    endpoint->init(opts);
    endpoint->setHandler(Http::make_handler<RequestHandler>());
    StreamServer server(endpoint);
    server.start();

    std::cout << "Cores = " << hardware_concurrency() << std::endl;

    sigset_t signals;
    if (sigemptyset(&signals) != 0
        || sigaddset(&signals, SIGTERM) != 0
        || sigaddset(&signals, SIGINT) != 0
        || sigaddset(&signals, SIGHUP) != 0
        || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0)
    {
        perror("install signal handler failed");
        return 1;
    }

    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0) {
        std::cout << "received signal " << signal << std::endl;
        server.shutdown();
    }
    else {
        std::cerr << "sigwait returns " << status << std::endl;
    }

    return 0;
}

