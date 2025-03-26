#pragma once
#include <yaml-cpp/yaml.h>
#include <string>
#include <unistd.h>
#include <cstdio>

struct DataInfo
{
    std::string name;
    int num_of_data;
    int num_of_dim;
};

class config
{
public:
    static config *getInstance()
    {
        static config instance;
        return &instance;
    }
    void init(std::string configFile);

private:
    config() = default;
    ~config() = default;
    YAML::Node config_;

public:
    int serverid;
    DataInfo dataFile;
    std::string trpleFile;
    std::string randomshare;
    std::string rsignshare;
    std::string host;
    int port;
    int cluster;
    int tolerance;
};