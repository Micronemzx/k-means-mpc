#pragma once
#include "config.h"
#include "pailler.h"
#include "NTL/ZZ.h"
#include <string>
#include <iostream>
#include <fstream>

class server2
{
public:
    server2() = default;
    ~server2() = default;
    void run();

private:
    void k_means_init();
    void connect_to_server1();
    void output_k_means();
    bool check();
    void k_means_update();

    std::string dataFile;
    std::string trpleFile;
    ZZ **data;
    pailler fhe;
};