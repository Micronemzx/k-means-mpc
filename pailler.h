#pragma once
#include <NTL/ZZ.h>
#include <cstdint>

using namespace NTL;

/*
pailler公钥
*/
struct PublicKey
{
    // 模数m，底数g
    ZZ n, g;
};

/*
pailler私钥
*/
struct PrivateKey
{
    ZZ lambda, mu;
};

class pailler
{
public:
    pailler() {}
    pailler(PublicKey publickey) : pubkey(publickey) {}
    ~pailler() {}
    // 密钥生成
    void keyGen(const uint32_t len);
    // 加密
    ZZ encrypt(const ZZ &m);
    // 解密
    ZZ decrypt(const ZZ &c);
    // 加法同态加密
    ZZ add(const ZZ &c1, const ZZ &c2);

private:
    PublicKey pubkey;
    PrivateKey prikey;
};