#include <csignal>
#include <future>

#include "StreamServer.h"
#include "RequestHandler.hpp"

StreamServer::StreamServer(const std::shared_ptr<Http::Endpoint> &endpoint) :
        _endpoint{endpoint}, _interval{std::chrono::seconds(1)}, _shutdown{false}
{
    _endpoint->setHandler(Http::make_handler<RequestHandler>());
}

StreamServer::~StreamServer() {
    shutdown();
    std::cout << __func__ << std::endl;
}

void StreamServer::start() {
//    _shutdown = false;
    _thread = std::make_unique<std::jthread>([this](std::stop_token stoken) {
        run(std::move(stoken));
    });

    _signalThread = std::make_unique<std::jthread>([this]() {
        setupSignalHandler();
        waitForSignal();
    });

    _endpoint->serve();
}

void StreamServer::shutdown() {
    _endpoint->shutdown();
//    _shutdown = true;
    /*if (!_shutdown.exchange(true)) {
        _endpoint->shutdown();
    }*/
}

void StreamServer::run(std::stop_token stoken) {
    Tcp::Listener::Load old;

    while (!stoken.stop_requested()) {
        if (!_endpoint->isBound()) {
            std::cerr << "Server not bound" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(_interval));
    }

    std::cout << "Server stopped" << std::endl;
}

void StreamServer::setupSignalHandler() {
    sigset_t signals;
    if (sigemptyset(&signals) != 0
        || sigaddset(&signals, SIGTERM) != 0
        || sigaddset(&signals, SIGINT) != 0
        || sigaddset(&signals, SIGHUP) != 0
        || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0)
    {
        perror("install signal handler failed");
    }
}

void StreamServer::waitForSignal() {
    sigset_t signals;
    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0) {
        std::cout << "received signal " << signal << std::endl;
        shutdown();
    }
    else {
        std::cerr << "sigwait returns " << status << std::endl;
    }
}





