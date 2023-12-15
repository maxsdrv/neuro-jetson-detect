#include "RTPServer.hpp"

int main (int argc, char* argv[]) {
    Address addr{Ipv4::any(), Pistache::Port(5001)};

    auto server = std::make_shared<RTPServer>(addr);

    std::cout << "Cores = " << hardware_concurrency() << std::endl;

    server->init();
    server->start();

    return 0;
}

