#pragma once
#include "NTL/ZZ.h"
#include <vector>
#include "transfer.h"
#include "pailler.h"

#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Network/Endpoint.h>
#include <cryptoTools/Network/IOService.h>
using namespace osuCrypto;
using namespace NTL;

struct triple
{
    ZZ a, b, c;
};
// 安全距离协议
void SDist(Channel &sock, const ZZ **x, const ZZ **cen, const std::vector<triple> &t, int serverid);
// 安全乘法
ZZ SMul(Channel &sock, const ZZ &x, const ZZ &y, const triple &t, int serverid);
// 安全除法(y/x)
ZZ SDiv(Channel &sock, const ZZ &x, const ZZ &y, const std::vector<triple> &t, int serverid);
// 安全比较
ZZ SComp(Channel &sock, const ZZ &x, const ZZ &y, const std::vector<triple> &t, const ZZ &r, const ZZ &r_sign, int serverid);
// 安全k最小
void SMink(Channel &sock, const ZZ *x, ZZ *result, const uint32_t k,
           const std::vector<triple> &tri, const std::vector<ZZ> &vec_r, const std::vector<ZZ> &vec_r_sign,
           int serverid, pailler &mycrypto, pailler &other);
