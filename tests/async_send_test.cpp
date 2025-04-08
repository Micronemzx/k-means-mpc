#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Network/Session.h>
#include <cryptoTools/Network/IOService.h>
#include <string>
using namespace osuCrypto;
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "usage: ./async_send_test 1/2" << std::endl;
        return 0;
    }
    std::string serverid = argv[1];
    if (serverid == "1")
    {
        IOService ios;
        Session ep(ios, "127.0.0.1", 8081, SessionMode::Server);
        Channel chl = ep.addChannel();
        chl.waitForConnection();
        std::vector<int> buf = {1, 2, 3, 4, 5, 114514};
        std::vector<int> buffer = {10, 9, 8, 7, 6};
        chl.asyncSend(buf);
        chl.asyncSend(buffer);
    }
    else
    {
        IOService ios;
        Session ep(ios, "127.0.0.1", 8081, SessionMode::Client);
        Channel chl = ep.addChannel();
        chl.onConnect([](const error_code &ec)
                      {
            if (ec)
                std::cout << "chl0 failed to connect: " << ec.message() << std::endl; });
        std::vector<int> buf;
        std::vector<int> buffer;
        sleep(2);
        chl.recv(buf);
        std::cout << buf.size() << "\n";
        chl.recv(buffer);
        std::cout << buffer.size() << "\n";
    }
    return 0;
}