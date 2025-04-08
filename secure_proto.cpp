#include "secure_proto.h"
#include "transfer.h"
#include "NTL/xdouble.h"
#include <cstdlib>
#include "boost/timer/timer.hpp"
#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Network/Endpoint.h>
#include <cryptoTools/Network/IOService.h>
// 协议运行时统计
// #define TIMERECORD
using namespace osuCrypto;
ZZ SMul(Channel &sock, const ZZ &x, const ZZ &y, const triple &t, int serverid)
{
#ifdef TIMERECORD
    boost::timer::auto_cpu_timer t1;
#endif
    ZZ d = x - t.a;
    ZZ e = y - t.b;
    sendZZ(sock, d);
    sendZZ(sock, e);
    ZZ d_, e_;
    recvZZ(sock, d_);
    recvZZ(sock, e_);
    d = d + d_;
    e = e + e_;
#ifdef TIMERECORD
    std::cout << "SMul time: ";
#endif
    // 因为定点数放缩，所以乘法需要去小数位
    if (serverid == 1)
        return (t.c + d * t.b + e * t.a + d * e);
    else
        return (t.c + d * t.b + e * t.a);
}

ZZ SDiv(Channel &sock, const ZZ &x, const ZZ &y, const std::vector<triple> &t, int serverid)
{
#ifdef TIMERECORD
    boost::timer::auto_cpu_timer t1;
#endif
    ZZ r;
    RandomBits(r, 64);
    ZZ px = SMul(sock, x, r, t[0], serverid);
    ZZ py = SMul(sock, y, r, t[1], serverid);
    sendZZ(sock, px);
    ZZ px_;
    recvZZ(sock, px_);
    px = px + px_;
#ifdef TIMERECORD
    std::cout << "SDiv time: ";
#endif
    return py / px;
}

ZZ SComp(Channel &sock, const ZZ &x, const ZZ &y, const std::vector<triple> &t, const ZZ &r, const ZZ &r_sign, int serverid)
{
#ifdef TIMERECORD
    boost::timer::auto_cpu_timer t1;
#endif
    // 连乘可优化
    ZZ us = 2 * (x - y) + serverid - 1;
    ZZ tmp = SMul(sock, us, r, t[0], serverid);
    ZZ c = SMul(sock, r_sign, tmp, t[1], serverid);
    sendZZ(sock, c);
    ZZ c_;
    recvZZ(sock, c_);
    c = c + c_;
    ZZ usign = sign(c) * r_sign;
#ifdef TIMERECORD
    std::cout << "SComp time: ";
#endif
    return (1 - usign) / 2;
}

void SMink(Channel &sock, const ZZ *x, ZZ *result, const uint32_t k,
           const std::vector<triple> &tri, const std::vector<ZZ> &vec_r, const std::vector<ZZ> &vec_r_sign,
           int serverid, pailler &mycrypto, pailler &other)
{
#ifdef TIMERECORD
    boost::timer::auto_cpu_timer t1;
#endif
    srand(time(NULL));
    // 初始化
    if (serverid == 1)
    {
        ZZ d_[k], p[k], y[k], e[k];
        int s[k]; // 转置矩阵
        for (int i = 0; i < k; ++i)
        {
            recvZZ(sock, p[i]);
            s[i] = i;
        }
        for (int i = 0; i < k; ++i)
        {
            swap(s[rand() % k], s[i]);
        }
        for (int i = 0; i < k; ++i)
        {
            d_[i] = p[s[i]];
            y[i] = x[s[i]];
        }
        ZZ r, u1_, u1, u2_;
        RandomBits(r, NumBits(y[0]) - 1);
        ZZ v1 = y[0] - r;
        ZZ v2_ = other.add(d_[0], other.encrypt(r));
        ZZ v1_ = mycrypto.encrypt(v1);
        sendZZ(sock, v1_);
        sendZZ(sock, v2_);
        std::vector<triple> tmp(2);
        int id = 0, idr = 0, idrsign = 0;
        for (int i = 1; i < k; ++i)
        {
            RandomBits(r, NumBits(y[i]) - 1);
            u1 = y[i] - r;
            u2_ = other.add(d_[i], other.encrypt(r));
            u1_ = mycrypto.encrypt(u1);
            sendZZ(sock, u1_);
            sendZZ(sock, u2_);
            tmp[0] = tri[id++];
            tmp[1] = tri[id++];
            ZZ f1 = SComp(sock, v1, u1, tmp, vec_r[idr++], vec_r_sign[idrsign++], serverid);
            sendZZ(sock, f1);
            recvZZ(sock, v1_);
            v1 = mycrypto.decrypt(v1_);
        }
        for (uint32_t i = 0; i < k; i++)
        {
            RandomBits(result[i], 64);
            recvZZ(sock, e[s[i]]);
        }
        for (uint32_t i = 0; i < k; i++)
        {
            e[i] = other.add(e[i], other.encrypt(-result[i]));
            sendZZ(sock, e[i]);
        }
    }
    else
    {
        // 加密x
        ZZ d_[k];
        for (uint32_t i = 0; i < k; i++)
        {
            d_[i] = mycrypto.encrypt(x[i]);
            sendZZ(sock, d_[i]);
        }
        // 初始化
        int t = 0;
        ZZ u1_, u2_, u2, v1_, v2_;
        recvZZ(sock, v1_);
        recvZZ(sock, v2_);
        ZZ v2 = mycrypto.decrypt(v2_);
        if (v2 * 2 > mycrypto.getPublicKey().n)
            v2 -= mycrypto.getPublicKey().n;
        std::vector<triple> tmp(2);
        // 迭代k-1轮求最小值
        int id = 0, idr = 0, idrsign = 0;
        for (int i = 1; i < k; ++i)
        {
            recvZZ(sock, u1_);
            recvZZ(sock, u2_);
            u2 = mycrypto.decrypt(u2_);
            if (u2 * 2 > mycrypto.getPublicKey().n)
                u2 -= mycrypto.getPublicKey().n;

            tmp[0] = tri[id++];
            tmp[1] = tri[id++];
            ZZ f2 = SComp(sock, v2, u2, tmp, vec_r[idr++], vec_r_sign[idrsign++], serverid);
            ZZ f1;
            recvZZ(sock, f1);
            ZZ f = f1 + f2;
            if (f != 0)
            {
                ZZ r_;
                RandomBits(r_, NumBits(v2) - 1);
                v2 = v2 - r_;
                v1_ = other.add(v1_, other.encrypt(r_));
                sendZZ(sock, v1_);
            }
            else
            {
                ZZ r_;
                RandomBits(r_, NumBits(u2) - 1);
                v2 = u2 - r_;
                v1_ = other.add(u1_, other.encrypt(r_));
                sendZZ(sock, v1_);
                t = i;
            }
        }
        // 生成结果
        ZZ e_[k], e[k];
        for (uint32_t i = 0; i < k; i++)
        {
            e[i] = (t == i);
            e_[i] = mycrypto.encrypt(e[i]);
            sendZZ(sock, e_[i]);
            // std::cout << e[i] << " ";
        }
        for (uint32_t i = 0; i < k; i++)
        {
            recvZZ(sock, result[i]);
            result[i] = mycrypto.decrypt(result[i]);
            result[i] -= mycrypto.getPublicKey().n;
        }
    }
#ifdef TIMERECORD
    std::cout << "SMink time: ";
#endif
}