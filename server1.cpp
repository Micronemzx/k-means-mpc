#include "server1.h"
#include "transfer.h"
#include "secure_proto.h"
#include "NTL/xdouble.h"
#include "boost/timer/timer.hpp"

using namespace osuCrypto;

void server1::run()
{
    crypto1.keyGen(512);
    if (config::getInstance()->serverid == 1)
    {
        ep = std::make_shared<Session>(ios, "127.0.0.1", config::getInstance()->port, SessionMode::Server);
        chl = ep->addChannel();
        chl.waitForConnection();
    }
    else
    {
        ep = std::make_shared<Session>(ios, "127.0.0.1", config::getInstance()->port, SessionMode::Client);
        chl = ep->addChannel();
        chl.onConnect([](const error_code &ec)
                      {
            if (ec)
                std::cout << "chl0 failed to connect: " << ec.message() << std::endl; });
    }

    k_means_init();
    do
    {
        boost::timer::auto_cpu_timer t;
        k_means_update();

        output_k_means();
    } while (!check());
    output_k_means();
}

void server1::k_means_init()
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
            tmp = tmp * 1000000; // 小数位放大
            data[i][j] = to_ZZ(tmp);
        }
    }
    fin.close();
    // 读取三元组文件
    tripleStream.open(cfg->trpleFile);
    if (!tripleStream.is_open())
    {
        throw std::runtime_error("k_means_init: triple file not open");
    }
    // 读取随机数分享
    randomShareStream.open(cfg->randomshare);
    if (!randomShareStream.is_open())
    {
        throw std::runtime_error("k_means_init: random share file not open");
    }
    // 读取随符号分享
    rSignShareStream.open(cfg->rsignshare);
    if (!rSignShareStream.is_open())
    {
        throw std::runtime_error("k_means_init: random sign share file not open");
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

void server1::output_k_means()
{
    for (int i = 0; i < k; ++i)
    {
        // 交换质心分享，恢复秘密
        for (int j = 0; j < d; ++j)
        {
            sendZZ(chl, centroids[i][j]);
        }
        std::cout << "centrid " << i << ": ";
        xdouble tmp;
        ZZ centroid;
        for (int j = 0; j < d; ++j)
        {
            recvZZ(chl, centroid);
            centroids[i][j] = centroids[i][j] + centroid;
            tmp = to_xdouble(centroids[i][j]);
            tmp /= 1000000;
            std::cout << tmp << " ";
        }
        ZZ cc;
        sendZZ(chl, cnt[i]);
        recvZZ(chl, cc);
        cc += cnt[i];
        std::cout << "node num: " << cc;
        std::cout << "\n";
    }
    std::cout << "triple use: " << triple_num << std::endl;
}

bool server1::check()
{
    ZZ sum(0);
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < d; ++j)
        {
            triple t;
            get_triple(t);
            sum += SMul(chl, (centroids[i][j] - new_centroids[i][j]), (centroids[i][j] - new_centroids[i][j]), t, serverid);
            centroids[i][j] = new_centroids[i][j];
        }
    }
    std::vector<triple> tri;
    get_triple(tri, 2);
    ZZ r, r_sign;
    get_random_share(r);
    get_r_sign_share(r_sign);
    ZZ f = SComp(chl, sum, tolerence * 1000000 * 1000000, tri, r, r_sign, serverid);
    sendZZ(chl, f);
    ZZ f1;
    recvZZ(chl, f1);
    f = f + f1;
    std::cout << sum << "\n";
    if (f == 1)
        return true;
    return false;
}
void server1::k_means_update()
{
    // std::cout << "compute distence between node and cluster\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            Dist[i][j] = 0;
            for (int l = 0; l < d; ++l)
            {
                triple t;
                get_triple(t);
                Dist[i][j] += SMul(chl, data[i][l] - centroids[j][l], data[i][l] - centroids[j][l], t, serverid);
            }
            ZZ dist;
            sendZZ(chl, Dist[i][j]);
            recvZZ(chl, dist);
            // std::cout << Dist[i][j] + dist << " ";
        }
        // std::cout << "\n";
    }
    std::vector<ZZ> vec_r, vec_r_sign;
    std::vector<triple> tri;
    for (int i = 0; i < n; ++i)
    {
        get_random_share(vec_r, k - 1);
        get_r_sign_share(vec_r_sign, k - 1);
        get_triple(tri, 2 * (k - 1));
        SMink(chl, Dist[i], E[i], k, tri, vec_r, vec_r_sign, serverid, crypto1, crypto2);
    }

    for (int i = 0; i < k; ++i)
    {
        cnt[i] = 0;
        for (int j = 0; j < d; ++j)
            new_centroids[i][j] = 0;
    }
    ZZ f;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            cnt[i] += E[j][i];
            for (int l = 0; l < d; ++l)
            {
                triple tri;
                get_triple(tri);
                f = SMul(chl, data[j][l], E[j][i], tri, serverid);
                new_centroids[i][l] += f;
            }
        }
    }
    std::vector<triple> vec_tri;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < d; ++j)
        {
            get_triple(vec_tri, 2);
            new_centroids[i][j] = SDiv(chl, cnt[i], new_centroids[i][j], vec_tri, serverid);
        }
    }
}

void server1::get_triple(triple &t)
{
    triple_num++;
    tripleStream >> t.a >> t.b >> t.c;
}

void server1::get_triple(std::vector<triple> &t, int m)
{
    triple_num += m;
    t.resize(m);
    for (int i = 0; i < m; ++i)
    {
        tripleStream >> t[i].a >> t[i].b >> t[i].c;
    }
}
void server1::get_random_share(ZZ &r)
{
    randomShareStream >> r;
}
void server1::get_random_share(std::vector<ZZ> &r, int m)
{
    r.resize(m);
    for (int i = 0; i < m; ++i)
    {
        randomShareStream >> r[i];
    }
}
void server1::get_r_sign_share(ZZ &r_sign)
{
    rSignShareStream >> r_sign;
}
void server1::get_r_sign_share(std::vector<ZZ> &r_sign, int m)
{
    r_sign.resize(m);
    for (int i = 0; i < m; ++i)
    {
        rSignShareStream >> r_sign[i];
    }
}