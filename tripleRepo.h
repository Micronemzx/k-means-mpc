#include <cstdio>
#include "NTL/ZZ.h"
#include <cstdint>
#include <fstream>

using namespace NTL;

struct triple
{
    ZZ z, b, c;
};

template <typename T>
class tripleRepo
{
public:
    tripleRepo(std::string &file_, int size_) : size(size_), fin(file_)
    {
        if (!fin.is_open())
        {
            throw std::runtime_error("tripleRepo: file not open");
        }
    }
    tripleRepo() = delete;
    ~tripleRepo() {}
    bool pop(std::queue<T> &in, uint32_t n)
    {
        while (n--)
        {
            if (fin.eof())
            {
                return false;
            }
        }
    }

private:
    int size;
    // std::string file;
    std::ifstream fin;
};