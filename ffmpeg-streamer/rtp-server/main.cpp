#include "RTPServer.hpp"

class StatsEndpoint {
public:
    explicit StatsEndpoint(Address addr)
            : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(size_t thr = 24) {
        auto opts = Http::Endpoint::options()
                .threads(static_cast<int>(thr));
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }
private:
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

    void setupRoutes() {
        using namespace Rest;
        //Routes::Get(router, "/name", Routes::bind(&StatsEndpoint::handleHeartBeat, this));
        Routes::Get(router, "/heartbeat", [this](const auto& req, auto resp) { handleHeartBeat(req, std::move(resp));
            return Rest::Route::Result::Ok;
        });
    }

    void handleHeartBeat(const Rest::Request& request, Http::ResponseWriter response) {
        try {
            response.send(Http::Code::Ok);
        } catch (const std::exception& ex) {
            std::cerr << "Exception in " << __func__ << " " << ex.what();
        }
    }
};

int main (int argc, char* argv[]) {
    Address addr{Ipv4::any(), Pistache::Port(5001)};
    StatsEndpoint server =  StatsEndpoint(addr);

    std::cout << "Cores = " << hardware_concurrency() << std::endl;

    std::jthread serverThread([&server]() {
        server.init();
        server.start();
    });

    return 0;
}