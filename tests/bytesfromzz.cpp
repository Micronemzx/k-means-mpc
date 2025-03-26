#include "NTL/ZZ.h"
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;
using namespace NTL;
int main()
{
    ZZ a(123456789);
    ZZ b(987654321);
    ZZ c = a * b;
    c = c * c * c;
    std::cout << NumBytes(c) << endl;
    // c = c * c * c;
    unsigned char *bytes = new unsigned char[128];
    BytesFromZZ(bytes, c, 128);
    for (int i = 0; i < 128; i++)
    {
        printf("%x ", bytes[i]);
    }

    std::cout << endl;
    std::cout << sizeof(ZZ) << "\n";

    std::ifstream fin("../DataSet/triple1");
    std::ifstream fin2("../DataSet/triple2");
    ZZ a1, b1, c1;
    ZZ a2, b2, c2;
    fin >> a1 >> b1 >> c1;
    fin2 >> a2 >> b2 >> c2;
    std::cout << a1 << " " << b1 << " " << c1 << "\n";
    std::cout << a2 << " " << b2 << " " << c2 << "\n";
    std::cout << (a1 + a2) * (b1 + b2) << " " << c1 + c2 << "\n";
    return 0;
}