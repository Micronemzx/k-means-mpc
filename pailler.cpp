#include "pailler.h"

void pailler::keyGen(const uint32_t len)
{
    ZZ p, q;
    // 随机生成两个大素数p和q
    GenPrime(p, len);
    GenPrime(q, len);
    // 计算模数n和g
    pubkey.n = p * q;
    // pubkey.g = RandomBnd(pubkey.n * pubkey.n);
    pubkey.g = pubkey.n + 1;
    // 计算lambda和mu
    prikey.lambda = (p - 1) * (q - 1) / GCD(p - 1, q - 1);
    prikey.mu = InvMod(prikey.lambda, pubkey.n);
}

// 加密函数
ZZ pailler::encrypt(const ZZ &m)
{
    ZZ r = RandomBnd(pubkey.n);
    return PowerMod(pubkey.g, m, pubkey.n * pubkey.n) * PowerMod(r, pubkey.n, pubkey.n * pubkey.n) % (pubkey.n * pubkey.n);
}

ZZ LExp(const ZZ &x, const ZZ &n)
{
    return (x - 1) / n;
}

ZZ pailler::decrypt(const ZZ &c)
{
    // 解密函数
    return LExp(PowerMod(c, prikey.lambda, pubkey.n * pubkey.n), pubkey.n) * prikey.mu % pubkey.n;
}

ZZ pailler::add(const ZZ &c1, const ZZ &c2)
{
    // 加法同态加密
    return c1 * c2 % (pubkey.n * pubkey.n);
}