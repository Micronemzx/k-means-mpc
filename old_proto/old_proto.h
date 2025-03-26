#pragma once
#include "NTL/ZZ.h"
#include <vector>
#include "transfer.h"
#include "pailler.h"
using namespace NTL;

struct triple
{
    ZZ a, b, c;
};

ZZ SMul(boost::asio::ip::tcp::socket &sock, const ZZ &x, const ZZ &y, const triple &t, int serverid)
{
#ifdef TIMERECORD
    boost::timer::auto_cpu_timer t1;
#endif
    ZZ d = x - t.a;
    ZZ e = y - t.b;
    sendZZ(sock, d);
    sendZZ(sock, e);
    ZZ d_, e_;
    d_ = recvZZ(sock);
    e_ = recvZZ(sock);
    d = d + d_;
    e = e + e_;
    std::cout << "SMul time: ";
    if (serverid == 1)
        return (t.c + d * t.b + e * t.a + d * e) >> 10;
    else
        return (t.c + d * t.b + e * t.a) >> 10;
}
ZZ SecCom(boost::asio::ip::tcp::socket &sock, const ZZ &x, const ZZ &y, std::vector<triple> &tri, int serverid)
{
    int l = 64;
    if (serverid == 1)
    {
        ZZ r, r_;
        RandomBits(r, l * 3);
        r += ((ZZ)1 << l);
        r_ = RandomBnd(r - 1) + 1;
        ZZ ra, rb;
        RandomBnd(ra, r);
        rb = r - ra;
        sendZZ(sock, rb);
        int id = 0;
        ZZ ua = SMul(sock, ra, x - y, tri[id++], 1);
        ZZ s;
        s = ((ZZ)1 << (NumBits(r) + 64)) + RandomBits_ZZ(NumBits(r) + 64);
        ZZ s_ = s + ua;
        sendZZ(sock, s_);
        int ld = rand() % (l - 1) + 1;
        sendInt(sock, &ld, sizeof(int));
        ZZ f = s - r_;
        ZZ f0 = f % ((ZZ)1 << ld);
        ZZ f1 = f - f0;
        ZZ g0a = recvZZ(sock);
        ZZ g1a = recvZZ(sock);
        ZZ w0, w1;
        RandomBits(w0, 3 * l);
        w1 = RandomBits_ZZ(3 * l) + w0 * (1ll << ld);
        ZZ w0a, w0b, w1a, w1b;
        RandomBnd(w0a, w0);
        w0b = w0 - w0a;
        RandomBnd(w1a, w1);
        w1b = w1 - w1a;
        sendZZ(sock, w0b);
        sendZZ(sock, w1b);
        ZZ g1a_ = SMul(sock, w1a, g1a, tri[id++], 1);
        ZZ g0a_ = SMul(sock, w0a, g0a, tri[id++], 1);
        ZZ ha = g1a_ + g0a_ - f1 * w1 - f0 * w0;
        sendZZ(sock, ha);
        return (ZZ)0;
    }
    else
    {
        int id = 0;
        ZZ rb = recvZZ(sock);
        ZZ ub = SMul(sock, rb, x - y, tri[id++], 2);
        ZZ s_ = recvZZ(sock);
        int ld;
        recvInt(sock, &ld, sizeof(int));
        ZZ g = s_ + ub;
        ZZ g0 = g % ((ZZ)1 << ld);
        ZZ g1 = g - g0;
        ZZ g1a, g1b;
        RandomBnd(g1a, g1);
        g1b = g1 - g1a;
        ZZ g0a, g0b;
        RandomBnd(g0a, g0);
        g0b = g0 - g0a;
        sendZZ(sock, g0a);
        sendZZ(sock, g1a);
        ZZ w0b, w1b;
        w0b = recvZZ(sock);
        w1b = recvZZ(sock);
        ZZ g1b_ = SMul(sock, w1b, g1b, tri[id++], 2);
        ZZ g0b_ = SMul(sock, w0b, g0b, tri[id++], 2);
        ZZ ha = recvZZ(sock);
        return g1b_ + g0b_ + ha;
    }
}

void SMink(boost::asio::ip::tcp::socket &sock, const ZZ *x, ZZ *result, const uint32_t k,
           const std::vector<triple> &tri, int serverid, pailler &mycrypto, pailler &other)
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
            p[i] = recvZZ(sock, 1024);
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
        std::cout << "\n";
        ZZ r, u1_, u1, u2_;
        RandomBits(r, NumBits(y[0]) - 1);
        ZZ v1 = y[0] - r;
        ZZ v2_ = other.add(d_[0], other.encrypt(r));
        ZZ v1_ = mycrypto.encrypt(v1);
        sendZZ(sock, v1_, 1024);
        sendZZ(sock, v2_, 1024);
        std::vector<triple> tmp(2);
        int id = 0, idr = 0, idrsign = 0;
        for (int i = 1; i < k; ++i)
        {
            RandomBits(r, NumBits(y[i]) - 1);
            u1 = y[i] - r;
            u2_ = other.add(d_[i], other.encrypt(r));
            u1_ = mycrypto.encrypt(u1);
            sendZZ(sock, u1_, 1024);
            sendZZ(sock, u2_, 1024);
            tmp[0] = tri[id++];
            tmp[1] = tri[id++];
            ZZ f1 = SecCom(sock, v1, u1, tmp, serverid);
            // std::cout << "compare(" << v1 << "," << u1 << " " << " " << vec_r[idr] << ' ' << vec_r_sign[idrsign] << ")=" << f1 << std::endl;
            sendZZ(sock, f1, 1024);
            v1_ = recvZZ(sock, 1024);
            v1 = mycrypto.decrypt(v1_);
        }
        for (uint32_t i = 0; i < k; i++)
        {
            e[s[i]] = recvZZ(sock, 1024);
        }
        ZZ z[k], y[k];
        for (uint32_t i = 0; i < k; i++)
        {
            RandomBits(z[i], 1);
            if (z[i] == 0)
                y[i] = e[i];
            else
            {
                y[i] = other.encrypt((ZZ)1) * InvMod(e[i], other.getPublicKey().n);
            }
            sendZZ(sock, y[i]);
            sendZZ(sock, z[i]);
            result[i] = e[i];
        }
    }
    else
    {
        // 加密x
        ZZ d_[k];
        for (uint32_t i = 0; i < k; i++)
        {
            d_[i] = mycrypto.encrypt(x[i]);
            sendZZ(sock, d_[i], 1024);
        }
        // 初始化
        int t = 0;
        ZZ u1_, u2_, u2;
        ZZ v1_ = recvZZ(sock, 1024);
        ZZ v2_ = recvZZ(sock, 1024);
        ZZ v2 = mycrypto.decrypt(v2_);
        std::vector<triple> tmp(2);
        // 迭代k-1轮求最小值
        int id = 0, idr = 0, idrsign = 0;
        for (int i = 1; i < k; ++i)
        {
            u1_ = recvZZ(sock, 1024);
            u2_ = recvZZ(sock, 1024);
            u2 = mycrypto.decrypt(u2_);
            tmp[0] = tri[id++];
            tmp[1] = tri[id++];
            ZZ f2 = SecCom(sock, v2, u2, tmp, serverid);
            ZZ f1 = recvZZ(sock, 1024);
            ZZ f = f1 + f2;
            // std::cout << "compare(" << v2 << "," << u2 << ")=" << f << std::endl;
            // std::cout << "compare(" << v2 << "," << u2 << " " << " " << vec_r[idr] << ' ' << vec_r_sign[idrsign] << ")=" << f << std::endl;

            if (f != 0)
            {
                ZZ r_;
                RandomBits(r_, NumBits(v2) - 1);
                v2 = v2 - r_;
                v1_ = other.add(v1_, other.encrypt(r_));
                sendZZ(sock, v1_, 1024);
            }
            else
            {
                ZZ r_;
                RandomBits(r_, NumBits(u2) - 1);
                v2 = u2 - r_;
                v1_ = other.add(u1_, other.encrypt(r_));
                sendZZ(sock, v1_, 1024);
                t = i;
            }
        }
        // 生成结果
        ZZ e_[k], e[k];
        for (uint32_t i = 0; i < k; i++)
        {
            e[i] = (t == i);
            e_[i] = mycrypto.encrypt(e[i]);
            sendZZ(sock, e_[i], 1024);
        }
        ZZ y[k], zi;
        for (uint32_t i = 0; i < k; i++)
        {
            y[i] = recvZZ(sock);
            zi = recvZZ(sock);
            y[i] = mycrypto.decrypt(y[i]);
            if (y[i] == 0)
            {
                result[i] = zi;
            }
            else
            {
                result[i] = other.encrypt((ZZ)1) * InvMod(zi, other.getPublicKey().n * other.getPublicKey().n);
            }
        }
    }
    std::cout << "SMink time: ";
}

// x/y
ZZ SDiv_a(boost::asio::ip::tcp::socket &sock, const ZZ &xb, const ZZ &yb, const std::vector<triple> &t, pailler paler)
{
    int l = 64, id = 0;
    ZZ xa, ya;
    RandomBits(xa, l);
    RandomBits(ya, l);
    ZZ xb = paler.add(xb, InvMod(paler.encrypt(xa), paler.getPublicKey().n * paler.getPublicKey().n));
    ZZ yb = paler.add(yb, InvMod(paler.encrypt(ya), paler.getPublicKey().n * paler.getPublicKey().n));
    sendZZ(sock, xb);
    sendZZ(sock, yb);
    ZZ a1, a2, p1, p2;
    do
    {
        RandomBits(a1, l);
        RandomBits(a2, l);
        RandomBits(p1, l);
        RandomBits(p2, l);
    } while (a1 * p2 - a2 * p1 != 0);
    ZZ a1a, a1b, a2a, a2b;
    RandomBnd(a1a, a1);
    a1b = a1 - a1a;
    RandomBnd(a2a, a2);
    a2b = a2 - a2a;
    ZZ p1a, p1b, p2a, p2b;
    RandomBnd(p1a, p1);
    p1b = p1 - p1a;
    RandomBnd(p2a, p2);
    p2b = p2 - p2a;
    sendZZ(sock, a1b);
    sendZZ(sock, a2b);
    sendZZ(sock, p1b);
    sendZZ(sock, p2b);
    ZZ u1a = SMul(sock, a1a, xa, t[id++], 1);
    ZZ u2a = SMul(sock, a2a, xa, t[id++], 1);
    ZZ v1a = SMul(sock, p1a, ya, t[id++], 1);
    ZZ v2a = SMul(sock, p2a, ya, t[id++], 1);
    ZZ y1a = u1a + v1a;
    ZZ y2a = u2a + v2a;
    sendZZ(sock, y1a);
    sendZZ(sock, y2a);
}

ZZ SDiv_b(boost::asio::ip::tcp::socket &sock, const std::vector<triple> &t, pailler paler)
{
    int id = 0;
    ZZ xb = paler.decrypt(recvZZ(sock));
    ZZ yb = paler.decrypt(recvZZ(sock));
    if (xb > paler.getPublicKey().n / 2)
        xb = xb - paler.getPublicKey().n / 2;
    if (yb > paler.getPublicKey().n / 2)
        yb = yb - paler.getPublicKey().n / 2;
    ZZ a1b = recvZZ(sock);
    ZZ a2b = recvZZ(sock);
    ZZ p1b = recvZZ(sock);
    ZZ p2b = recvZZ(sock);
    ZZ u1b = SMul(sock, a1b, xb, t[id++], 2);
    ZZ u2b = SMul(sock, a2b, xb, t[id++], 2);
    ZZ v1b = SMul(sock, p1b, yb, t[id++], 2);
    ZZ v2b = SMul(sock, p2b, yb, t[id++], 2);
    ZZ y1a = recvZZ(sock);
    ZZ y2a = recvZZ(sock);
    ZZ y1 = u1b + v1b + y1a;
    ZZ y2 = u2b + v2b + y2a;
}
