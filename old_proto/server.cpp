#include "server.h"
#include "boost/asio.hpp"
#include "transfer.h"
#include "NTL/xdouble.h"
#include "boost/timer/timer.hpp"
using namespace boost::asio;

void server::run()
{
    crypto1.keyGen(512);
    if (config::getInstance()->serverid == 1)
    {
        // wait_for_connect
        io_context io;
        ip::tcp::acceptor acptr(io, ip::tcp::endpoint(ip::tcp::v4(), config::getInstance()->port));
        sockptr = std::make_shared<ip::tcp::socket>(io);
        acptr.accept(*sockptr);
        std::cout << sockptr->remote_endpoint().address() << std::endl;
    }
    else
    {
        // connect to server 1
        io_context io;
        ip::tcp::socket sock(io);
        sock.connect(ip::tcp::endpoint(ip::address::from_string(config::getInstance()->host), config::getInstance()->port));
        sockptr = std::make_shared<ip::tcp::socket>(std::move(sock));
    }

    k_means_init();
    do
    {
        boost::timer::auto_cpu_timer t;
        k_means_update();
    } while (!check());
    output_k_means();
}

void server::k_means_init()
{
    auto cfg = config::getInstance();
    // 读取数据文件
    std::ifstream fin(cfg->dataFile.name);
    if (!fin.is_open())
    {
        throw std::runtime_error("k_means_init: data file not open");
    }
    n = cfg->dataFile.num_of_data;
    d = cfg->dataFile.num_of_dim;
    k = cfg->cluster;
    serverid = cfg->serverid;
    tolerence = cfg->tolerance;
    data = new ZZ *[n];
    xdouble tmp;
    for (uint32_t i = 0; i < n; i++)
    {
        data[i] = new ZZ[d];
        for (uint32_t j = 0; j < d; j++)
        {
            fin >> tmp;
            tmp = tmp * 1024; // 小数位放大
            data[i][j] = to_ZZ(tmp);
            // fin >> data[i][j];
        }
    }
    fin.close();
    // 读取三元组文件
    tripleStream.open(cfg->trpleFile);
    if (!tripleStream.is_open())
    {
        throw std::runtime_error("k_means_init: triple file not open");
    }
    // 发送公钥
    auto pubkey = crypto1.getPublicKey();
    sendZZ(chl, pubkey.n);
    // 接收公钥
    ZZ pub_n;
    recvZZ(chl, pub_n);
    PublicKey pubkey2;
    pubkey2.n = pub_n;
    pubkey2.g = pub_n + 1;
    crypto2.setPublicKey(pubkey2);

    // 数组初始化
    centroids = new ZZ *[k];
    new_centroids = new ZZ *[k];
    for (uint32_t i = 0; i < k; i++)
    {
        centroids[i] = new ZZ[d];
        new_centroids[i] = new ZZ[d];
    }
    E = new ZZ *[n];
    Dist = new ZZ *[n];
    for (uint32_t i = 0; i < n; i++)
    {
        E[i] = new ZZ[k];
        Dist[i] = new ZZ[k];
    }
    sum = new ZZ[k];
    cnt = new ZZ[k];
    // TODO: 聚类中心初始化
    for (uint32_t i = 0; i < k; i++)
    {
        for (uint32_t j = 0; j < d; j++)
        {
            centroids[i][j] = data[i][j];
        }
    }
}

void server::output_k_means()
{
    for (int i = 0; i < k; ++i)
    {
        // 交换质心分享，恢复秘密
        for (int j = 0; j < d; ++j)
        {
            sendZZ(*sockptr, centroids[i][j]);
        }
        std::cout << "centrid " << i << ":";
        xdouble tmp;
        for (int j = 0; j < d; ++j)
        {
            ZZ centroid = recvZZ(*sockptr);
            centroids[i][j] = centroids[i][j] + centroid;
            tmp = to_xdouble(centroids[i][j]);
            tmp /= 1024;
            std::cout << tmp << " ";
        }
        std::cout << "\n";
    }
}

bool server::check()
{
    ZZ sum(0);
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < d; ++j)
        {
            triple t;
            get_triple(t);
            sum += SMul(*sockptr, (centroids[i][j] - new_centroids[i][j]), (centroids[i][j] - new_centroids[i][j]), t, serverid);
            centroids[i][j] = new_centroids[i][j];
        }
    }
    std::vector<triple> tri;
    get_triple(tri, 3);
    std::cout << "" << sum << "\n";
    ZZ f = SecCom(*sockptr, sum, tolerence, tri, serverid);
    sendZZ(*sockptr, f);
    ZZ f1 = recvZZ(*sockptr);
    f = f + f1;

    if (f <= 0)
        return true;
    return false;
}
void server::k_means_update()
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            for (int l = 0; l < d; ++l)
            {
                triple t;
                get_triple(t);
                Dist[i][j] += SMul(*sockptr, data[i][l] - centroids[j][l], data[i][l] - centroids[j][l], t, serverid);
            }
        }
    }
    std::vector<triple> tri;
    for (int i = 0; i < n; ++i)
    {
        get_triple(tri, 4 * (k - 1));
        SMink(*sockptr, Dist[i], E[i], k, tri, serverid, crypto1, crypto2);
    }
    for (int i = 0; i < k; ++i)
    {
        cnt[i] = 1;
        for (int j = 0; j < d; ++j)
            new_centroids[i][j] = 1;
    }
    ZZ f, mod = crypto2.getPublicKey().n * crypto2.getPublicKey().n;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            cnt[i] = cnt[i] * E[j][i] % mod;
            for (int l = 0; l < d; ++l)
            {
                new_centroids[i][l] = new_centroids[i][l] * PowerMod(E[j][i], data[j][l], mod) % mod;
            }
        }
    }
    std::vector<triple> vec_tri;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < d; ++j)
        {
            if (serverid == 1)
            {
                get_triple(vec_tri, 4);
                new_centroids[i][j] = SDiv_a(*sockptr, cnt[i], new_centroids[i][j], vec_tri, crypto2);
                get_triple(vec_tri, 4);
                SDiv_b(*sockptr, vec_tri, crypto1);
            }
            else
            {
                get_triple(vec_tri, 4);
                SDiv_b(*sockptr, vec_tri, crypto1);
                get_triple(vec_tri, 4);
                new_centroids[i][j] = SDiv_a(*sockptr, cnt[i], new_centroids[i][j], vec_tri, crypto2);
            }
        }
    }
}

void server::get_triple(triple &t)
{
    tripleStream >> t.a >> t.b >> t.c;
}

void server::get_triple(std::vector<triple> &t, int m)
{
    t.resize(m);
    for (int i = 0; i < m; ++i)
    {
        tripleStream >> t[i].a >> t[i].b >> t[i].c;
    }
}
