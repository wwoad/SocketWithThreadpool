#include "server.h"

SocketServer::SocketServer()
    : m_socketFD(-1), m_bindRet(-1), m_listenRet(-1), m_stopServer(false)
{
    m_clientNum.store(0);
    m_clientMax.store(10);

    m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFD == -1)
    {
        perror("socket");
        return;
    }
    else
    {
        std::cout << "----Socket Create Success----" << endl;
    }
}

SocketServer::~SocketServer()
{
    if (m_socketFD != -1)
    {
        shutdown(m_socketFD, SHUT_RDWR);
        close(m_socketFD);
        m_pPool->stop();
    }
}

void SocketServer::startServer()
{
    std::cout << "----Server Start----" << std::endl;
    bindServer();
    listenServer();

    m_pPool = std::make_unique<ThreadPool>(4, 12);
    m_pPool->addTask(&SocketServer::acceptServer, this);

    return;
}

void SocketServer::stopServer()
{
    m_stopServer.store(true);
    shutdown(m_socketFD, SHUT_RDWR);
    m_socketFD = -1;
    return;
}

void SocketServer::bindServer()
{
    m_saddr.sin_family = AF_INET;
    m_saddr.sin_port = htons(8000);
    m_saddr.sin_addr.s_addr = INADDR_ANY;

    m_bindRet = bind(m_socketFD, (sockaddr *)&m_saddr, sizeof(m_saddr));
    if (m_bindRet == -1)
    {
        perror("bind");
        return;
    }
    else
    {
        std::cout << "----Socket Bind Success----" << endl;
    }
}

void SocketServer::listenServer()
{
    m_listenRet = listen(m_socketFD, m_clientMax.load());
    if (m_listenRet == -1)
    {
        perror("listen");
        return;
    }
    else
    {
        std::cout << "----Lestenning----" << endl;
    }
}

void SocketServer::acceptServer()
{
    socklen_t addrlen = sizeof(sockaddr_in);
    while (!m_stopServer.load())
    {
        if (m_clientNum.load() > m_clientMax.load())
        {
            continue;
        }

        std::shared_ptr<SockInfo> pInfo = std::make_shared<SockInfo>();

        pInfo->fd = accept(m_socketFD, (sockaddr *)&pInfo->addr, &addrlen);
        if (pInfo->fd == -1)
        {
            if (m_stopServer.load())
            {
                break;
            }
            perror("accpet");
            continue;
        }
        else
        {
            std::cout << "----Received Client Request----" << endl;
        }

        m_clientNum.fetch_add(1);
        m_pPool->addTask(std::bind(&SocketServer::workingServer, this, pInfo));
    }

    return;
}

void SocketServer::workingServer(std::shared_ptr<SockInfo> pInfo)
{
    std::shared_ptr<SockInfo> info(std::move(pInfo));

    char ip[32];
    std::cout << "Link Success!" << "Client IP: "
              << inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, ip, (socklen_t)sizeof(ip))
              << " Port: " << ntohs(info->addr.sin_port) << std::endl;
    while (!m_stopServer.load())
    {
        char buffer[1024];
        int len = recv(info->fd, buffer, sizeof(buffer), 0);
        if (len > 0)
        {
            std::cout << "client: " << buffer << std::endl;
            send(info->fd, buffer, len, 0);
        }
        else if (len == 0)
        {
            std::cout << "Client IP: " << inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, ip, (socklen_t)sizeof(ip))
                      << " was disconnected... " << std::endl;
            break;
        }
        else
        {
            perror("recv");
            break;
        }
    }

    std::cout << "Working Stop" << std::endl;
    std::cout << "----Lestenning----" << endl;

    close(info->fd);
    info->fd = -1;
    m_clientNum.fetch_sub(1);

    return;
}
