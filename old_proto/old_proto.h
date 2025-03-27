#pragma once
#include "NTL/ZZ.h"
#include <vector>
#include "transfer.h"
#include "pailler.h"
#include "boost/timer/timer.hpp"
#include "NTL/xdouble.h"
struct triple
{
    ZZ a, b, c;
};

ZZ SMul(boost::asio::ip::tcp::socket &sock, const ZZ &x, const ZZ &y, const triple &t, int serverid);

ZZ SecCom(boost::asio::ip::tcp::socket &sock, const ZZ &x, const ZZ &y, std::vector<triple> &tri, int serverid);

void SMink(boost::asio::ip::tcp::socket &sock, const ZZ *x, ZZ *result, const uint32_t k,
           const std::vector<triple> &tri, int serverid, pailler &mycrypto, pailler &other);
ZZ SDiv_a(boost::asio::ip::tcp::socket &sock, const ZZ &x_b, const ZZ &y_b, const std::vector<triple> &t, pailler &paler);
ZZ SDiv_b(boost::asio::ip::tcp::socket &sock, const std::vector<triple> &t, pailler &paler);