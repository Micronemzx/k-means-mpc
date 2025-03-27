#include "config.h"
#include "server.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " config.yaml" << std::endl;
        return 0;
    }
    auto config = config::getInstance();
    config->init(argv[1]);

    server s1;
    s1.run();
    sleep(20);
    return 0;
}