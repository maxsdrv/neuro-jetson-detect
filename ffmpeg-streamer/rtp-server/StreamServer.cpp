#include <csignal>
#include <future>

#include "StreamServer.h"

StreamServer::StreamServer(const std::shared_ptr<Http::Endpoint> &endpoint) :
        _endpoint{endpoint}, _interval{std::chrono::seconds(1)}, _shutdown{false}
{
    _endpoint->setHandler(Http::make_handler<RequestHandler>(_threadManager));
}

StreamServer::~StreamServer() {
    shutdown();
    std::cout << __func__ << std::endl;
}

void StreamServer::start() {
    _threadManager.addTask([this](std::stop_token stoken) { run(); });
    _threadManager.addTask([this](std::stop_token stoken) { setupSignalHandler(); waitForSignal(); });

    _endpoint->serve();
}

void StreamServer::shutdown() {
    _threadManager.shutdown();
    _endpoint->shutdown();
}

void StreamServer::run() {
    Tcp::Listener::Load old;

    while (!_threadManager.isShutdownRequested()) {
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





