#include <fstream>
#include "pailler.h"
#include "old_proto.h"
#include "boost/asio.hpp"
#include "boost/timer/timer.hpp"
using namespace boost::asio;
using namespace NTL;
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " 1/2" << std::endl;
        return 0;
    }
    std::string serverid = argv[1];
    if (serverid == "1")
    {
        // wait_for_connect
        io_context io;
        ip::tcp::acceptor acptr(io, ip::tcp::endpoint(ip::tcp::v4(), 8001));
        ip::tcp::socket sock(io);
        acptr.accept(sock);
        std::cout << sock.remote_endpoint().address() << std::endl;
        ZZ x, y;
        RandomBits(x, 128);
        RandomBits(y, 128);
        std::cout << x << " " << y << std::endl;
        std::vector<triple> vec_tri;
        vec_tri.resize(20);
        std::ifstream tripleStream("triple1");
        for (int i = 0; i < 20; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }
        boost::timer::auto_cpu_timer t;
        for (int i = 0; i < 1000; i++)
            ZZ f = SecCom(sock, x, y, vec_tri, 1);
        // std::cout << f << std::endl;
        // std::cout << (f >> 10) << std::endl;
    }
    else
    {
        // connect to server 1
        io_context io;
        ip::tcp::socket sock(io);
        sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));
        ZZ x, y;
        RandomBits(x, 128);
        RandomBits(y, 128);
        std::cout << x << " " << y << std::endl;
        std::vector<triple> vec_tri;
        vec_tri.resize(20);
        std::ifstream tripleStream("triple2");
        for (int i = 0; i < 20; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }

        boost::timer::auto_cpu_timer t;
        for (int i = 0; i < 1000; i++)
        {
            ZZ f = SecCom(sock, x, y, vec_tri, 2);
            // std::cout << (f > 0) << std::endl;
        }
        // std::cout << (f >> 10) << std::endl;
    }
    return 0;
}