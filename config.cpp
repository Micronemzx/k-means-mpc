#include "config.h"
#include <iostream>
void config::init(std::string configFile)
{
    YAML::Node config = YAML::LoadFile(configFile);
    serverid = config["server"]["id"].as<int>();
    port = config["server"]["port"].as<int>();
    std::string dataname = config["DataFile"]["name"].as<std::string>();
    int n = config["DataFile"]["n"].as<int>();
    int d = config["DataFile"]["dimision"].as<int>();
    dataFile.name = dataname;
    dataFile.num_of_data = n;
    dataFile.num_of_dim = d;
    host = config["server"]["host"].as<std::string>();
    cluster = config["clusters"].as<int>();
    trpleFile = config["BeaverTripleFile"].as<std::string>();
    randomshare = config["randomShareFile"].as<std::string>();
    rsignshare = config["rsignShareFile"].as<std::string>();
    tolerance = config["tolerance"].as<int>();
    if ((serverid != 1 && serverid != 2) || dataname.empty() || trpleFile.empty() || randomshare.empty() || rsignshare.empty())
    {
        std::cout << "config.yaml is not valid" << std::endl;
        throw std::runtime_error("config.yaml is not valid");
    }
    if (access(dataname.c_str(), F_OK))
    {
        std::cout << "dataFile[" << dataname << "] is not exist" << std::endl;
        throw std::runtime_error("dataFile is not exist");
    }
    if (access(trpleFile.c_str(), F_OK))
    {
        std::cout << "trpleFile[" << trpleFile << "] is not exist" << std::endl;
        throw std::runtime_error("trpleFile is not exist");
    }
    if (access(randomshare.c_str(), F_OK))
    {
        std::cout << "randomshare[" << randomshare << "] is not exist" << std::endl;
        throw std::runtime_error("randomshare is not exist");
    }
    if (access(rsignshare.c_str(), F_OK))
    {
        std::cout << "rsignshare[" << rsignshare << "] is not exist" << std::endl;
        throw std::runtime_error("rsignshare is not exist");
    }
}