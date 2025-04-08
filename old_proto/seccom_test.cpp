#include <fstream>
#include "transfer.h"
#include "pailler.h"
#include "old_proto.h"
// #include "boost/asio.hpp"
#include "boost/timer/timer.hpp"
using namespace osuCrypto;
using namespace NTL;
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " 1/2" << std::endl;
        return 0;
    }
    IOService ios;
    Channel chl;
    std::shared_ptr<Session> ep;
    std::string serverid = argv[1];
    if (serverid == "1")
    {
        // wait_for_connect
        // io_context io;
        // ip::tcp::acceptor acptr(io, ip::tcp::endpoint(ip::tcp::v4(), 8001));
        // ip::tcp::socket sock(io);
        // acptr.accept(sock);
        // std::cout << sock.remote_endpoint().address() << std::endl;
        ep = std::make_shared<Session>(ios, "127.0.0.1", 8001, SessionMode::Server);
        chl = ep->addChannel();
        chl.waitForConnection();
        ZZ x, y;
        RandomBits(x, 64);
        RandomBits(y, 64);
        std::cout << x << " " << y << std::endl;
        std::vector<triple> vec_tri;
        vec_tri.resize(20);
        std::ifstream tripleStream("triple1");
        for (int i = 0; i < 20; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }
        boost::timer::auto_cpu_timer t;
        for (int i = 0; i < 500; i++)
            ZZ f = SecCom(chl, x, y, vec_tri, 1);
        // std::cout << f << std::endl;
        // std::cout << (f >> 10) << std::endl;
    }
    else
    {
        // connect to server 1
        // io_context io;
        // ip::tcp::socket sock(io);
        // sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));
        ep = std::make_shared<Session>(ios, "127.0.0.1", 8001, SessionMode::Client);
        chl = ep->addChannel();
        chl.onConnect([](const error_code &ec)
                      {
            if (ec)
                std::cout << "chl0 failed to connect: " << ec.message() << std::endl; });
        ZZ x, y;
        RandomBits(x, 64);
        RandomBits(y, 64);
        std::cout << x << " " << y << std::endl;
        std::vector<triple> vec_tri;
        vec_tri.resize(20);
        std::ifstream tripleStream("triple2");
        for (int i = 0; i < 20; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }

        boost::timer::auto_cpu_timer t;
        for (int i = 0; i < 500; i++)
        {
            ZZ f = SecCom(chl, x, y, vec_tri, 2);
            // std::cout << (f > 0) << std::endl;
        }
        // std::cout << (f >> 10) << std::endl;
    }
    return 0;
}