#pragma once
#include "boost/asio.hpp"
#include "NTL/ZZ.h"
using namespace NTL;
inline void sendAll(boost::asio::ip::tcp::socket &socket, const unsigned char *data, int len)
{
    int sent = 0, res = 0;
    while (sent < len)
    {
        res = socket.send(boost::asio::buffer(data + sent, len - sent));
        sent += res;
    }
}

inline void sendInt(boost::asio::ip::tcp::socket &socket, const int *data, int len)
{
    int sent = 0, res = 0;
    while (sent < len)
    {
        res = socket.send(boost::asio::buffer(data + sent, len - sent));
        sent += res;
    }
}

inline void recvAll(boost::asio::ip::tcp::socket &socket, unsigned char *data, int len)
{
    int received = 0, res = 0;
    while (received < len)
    {
        res = socket.receive(boost::asio::buffer(data + received, len - received));
        received += res;
    }
}

inline void recvInt(boost::asio::ip::tcp::socket &socket, int *data, int len)
{
    int received = 0, res = 0;
    while (received < len)
    {
        res = socket.receive(boost::asio::buffer(data + received, len - received));
        received += res;
    }
}

inline void sendZZ(boost::asio::ip::tcp::socket &sock, const ZZ &x, int maxlen = 1024)
{
    unsigned char buf[maxlen] = {0};
    int len = NumBytes(x);
    BytesFromZZ(buf, x, len);
    int len_sign = sign(x) * len;
    sendInt(sock, &len_sign, sizeof(int));
    sendAll(sock, buf, len);
}

inline ZZ recvZZ(boost::asio::ip::tcp::socket &sock, int maxlen = 1024)
{
    unsigned char buf[maxlen] = {0};
    int len = 0;
    recvInt(sock, &len, sizeof(int));
    recvAll(sock, buf, abs(len));
    ZZ x;
    ZZFromBytes(x, buf, abs(len));
    if (len < 0)
        x = x * (-1);
    return x;
}