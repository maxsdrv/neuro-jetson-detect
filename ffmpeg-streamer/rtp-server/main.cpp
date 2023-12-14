#include "RTPServer.hpp"

int main (int argc, char* argv[]) {
    Address addr{Ipv4::any().toString(), Pistache::Port(5000)};
    auto server = std::make_shared<RTPServer>(addr);

    std::cout << "Cores = " << hardware_concurrency() << std::endl;

    server->init();
    server->start();

    return 0;
}