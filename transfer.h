#pragma once
#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Network/Endpoint.h>
#include <cryptoTools/Network/IOService.h>
#include "NTL/ZZ.h"
using namespace NTL;
using namespace osuCrypto;
// inline void sendAll(boost::asio::ip::tcp::socket &socket, const unsigned char *data, int len)
// {
//     // int sent = 0, res = 0;
//     // while (sent < len)
//     // {
//     //     res = socket.send(boost::asio::buffer(data + sent, len - sent));
//     //     sent += res;
//     // }
//     // boost::asio::async_write(socket, boost::asio::buffer(data, len));
// }

// inline void sendInt(boost::asio::ip::tcp::socket &socket, const int *data, int len)
// {
//     int sent = 0, res = 0;
//     while (sent < len)
//     {
//         res = socket.send(boost::asio::buffer(data + sent, len - sent));
//         sent += res;
//     }
// }

// inline void recvAll(boost::asio::ip::tcp::socket &socket, unsigned char *data, int len)
// {
//     int received = 0, res = 0;
//     while (received < len)
//     {
//         res = socket.receive(boost::asio::buffer(data + received, len - received));
//         received += res;
//     }
// }

// inline void recvInt(boost::asio::ip::tcp::socket &socket, int *data, int len)
// {
//     int received = 0, res = 0;
//     while (received < len)
//     {
//         res = socket.receive(boost::asio::buffer(data + received, len - received));
//         received += res;
//     }
// }

inline void sendZZ(Channel &chl, const ZZ &x)
{
    int len = NumBytes(x);
    std::vector<uint8_t> buf(len + 1);
    int8_t len_sign = 1;
    if (x < 0)
        len_sign = -1;
    memcpy(buf.data(), &len_sign, sizeof(signed char));
    BytesFromZZ(buf.data() + 1, x, len);
    chl.asyncSend(std::move(buf));
}

inline void recvZZ(Channel &chl, ZZ &x)
{
    std::vector<uint8_t> buffer;
    chl.recv(buffer);
    int8_t len_sign;
    memcpy(&len_sign, buffer.data(), sizeof(signed char));
    ZZFromBytes(x, buffer.data() + 1, buffer.size() - 1);
    x = x * len_sign;
}