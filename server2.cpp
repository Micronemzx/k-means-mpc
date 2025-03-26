#include "server2.h"
void server2::run()
{
    connect_to_server1();

    auto config = config::getInstance();
    dataFile = config->dataFile;
    trpleFile = config->trpleFile;
    k_means_init();
    while (check())
    {
        k_means_update();
    }
    output_k_means();
}

void server2::k_means_init()
{
    // 读取数据文件
    std::ifstream fin(dataFile);
    if (!fin.is_open())
    {
        throw std::runtime_error("k_means_init: file not open");
    }
    uint32_t m, d;
    fin >> m >> d;
    data = new ZZ *[m];
    for (uint32_t i = 0; i < m; i++)
    {
        data[i] = new ZZ[d];
        for (uint32_t j = 0; j < d; j++)
        {
            fin >> data[i][j];
        }
    }
    fin.close();
    // 读取三元组文件
    std::ifstream fin2(trpleFile);
    if (!fin2.is_open())
    {
        throw std::runtime_error("k_means_init: file not open");
    }
}