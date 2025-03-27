#pragma once
#include "config.h"
#include "pailler.h"
#include "NTL/ZZ.h"
#include <string>
#include <iostream>
#include <fstream>
#include "boost/asio.hpp"
#include "old_proto.h"

using namespace boost::asio;

class server
{
public:
    server() = default;
    ~server() = default;
    void run();

private:
    void k_means_init();
    void output_k_means();
    bool check();
    void k_means_update();
    void get_triple(triple &t);
    void get_triple(std::vector<triple> &t, int m);

    ZZ **data;          // 秘密分享数据
    ZZ **centroids;     // 秘密分享质心
    ZZ *cluster;        // 秘密分享聚类结果
    ZZ *count;          // 秘密分享聚类数量
    ZZ *sum;            // 秘密分享聚类和
    ZZ *cnt;            // 秘密分享聚类计数
    ZZ **new_centroids; // 秘密分享新质心
    ZZ **Dist;          // 秘密分享距离
    ZZ **E;             // 最小值结果
    ZZ tolerence;       // 聚类中心差额
    std::ifstream tripleStream;
    int n, k, d, serverid;

    pailler crypto1, crypto2;
    std::shared_ptr<ip::tcp::socket> sockptr;
};