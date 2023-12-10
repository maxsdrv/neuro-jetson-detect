#include "RTPServer.hpp"



int main (int argc, char* argv[]) {
    Address addr{Ipv4::any().toString(), 5001};
    auto server = std::make_unique<RTPServer>(addr);
    server->init();
    server->start();

    return 0;
}