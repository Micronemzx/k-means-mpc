#include <gtest/gtest.h>
#include "pailler.h"

TEST(PaillerTest, Decrypt)
{
    pailler pa;
    pa.keyGen(32);
    std::cout << pa.getPublicKey().n << "\n";
    ZZ m = ZZ(-123);
    ZZ c = pa.encrypt(m);
    ZZ dm = pa.decrypt(c);

    // 验证解密结果与原始消息相等
    EXPECT_EQ(dm, m);
}

TEST(PaillerTest, Add)
{
    pailler pa;
    pa.keyGen(32);

    ZZ m1 = ZZ(123);
    ZZ m2 = ZZ(-456);
    ZZ c1 = pa.encrypt(m1);
    ZZ c2 = pa.encrypt(m2);
    ZZ c3 = pa.add(c1, c2);
    ZZ dm = pa.decrypt(c3);

    // 验证加法同态加密结果的正确性
    EXPECT_EQ(dm, m1 + m2);
}
