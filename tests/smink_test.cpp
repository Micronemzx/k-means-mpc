#include <fstream>
#include "pailler.h"
#include "secure_proto.h"
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

        ZZ *x, *y;
        int k = 4;
        x = new ZZ[k];
        y = new ZZ[k];
        for (int i = 0; i < k; i++)
        {
            RandomBits(x[i], 128);
            std::cout << x[i] << " ";
        }
        std::cout << "\n";
        std::vector<triple> vec_tri;
        vec_tri.resize(2 * k);
        std::ifstream tripleStream("triple1");
        for (int i = 0; i < 2 * k; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }
        std::vector<ZZ> r, r_sign;
        r.resize(k);
        r_sign.resize(k);
        std::ifstream randomShareStream("randomshare1");
        std::ifstream rSignShareStream("rsignshare1");
        for (int i = 0; i < k; i++)
        {
            randomShareStream >> r[i];
            rSignShareStream >> r_sign[i];
        }
        pailler mycrypto, other;
        mycrypto.keyGen(1024);
        sendZZ(sock, mycrypto.getPublicKey().n);
        PublicKey pub;
        pub.n = recvZZ(sock);
        pub.g = pub.n + 1;
        other.setPublicKey(pub);
        {
            boost::timer::auto_cpu_timer t;
            for (int i = 0; i < 10; ++i)
                SMink(sock, x, y, k, vec_tri, r, r_sign, 1, mycrypto, other);
        }
        // for (int i = 0; i < k; i++)
        // {
        //     std::cout << y[i] << std::endl;
        // }
    }
    else
    {
        // connect to server 1

        io_context io;
        ip::tcp::socket sock(io);
        sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));

        ZZ *x, *y;
        int k = 4;
        x = new ZZ[k];
        y = new ZZ[k];
        for (int i = 0; i < k; i++)
        {
            RandomBits(x[i], 128);
            std::cout << x[i] << " ";
        }
        std::cout << "\n";
        std::vector<triple> vec_tri;
        vec_tri.resize(2 * k);
        std::ifstream tripleStream("triple2");
        for (int i = 0; i < 2 * k; i++)
        {
            tripleStream >> vec_tri[i].a >> vec_tri[i].b >> vec_tri[i].c;
        }
        std::vector<ZZ> r, r_sign;
        r.resize(k);
        r_sign.resize(k);
        std::ifstream randomShareStream("randomshare2");
        std::ifstream rSignShareStream("rsignshare2");
        for (int i = 0; i < k; i++)
        {
            randomShareStream >> r[i];
            rSignShareStream >> r_sign[i];
        }
        pailler mycrypto, other;
        mycrypto.keyGen(1024);
        sendZZ(sock, mycrypto.getPublicKey().n);
        PublicKey pub;
        pub.n = recvZZ(sock);
        pub.g = pub.n + 1;
        other.setPublicKey(pub);

        boost::timer::auto_cpu_timer t;
        for (int i = 0; i < 10; ++i)
            SMink(sock, x, y, k, vec_tri, r, r_sign, 2, mycrypto, other);
    }
    return 0;
}