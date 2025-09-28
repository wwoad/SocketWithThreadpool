#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <atomic>
#include <arpa/inet.h>
#include <memory>
#include <unistd.h>
#include "thread_pool.h"

struct SockInfo
{
    int fd;
    sockaddr_in addr;
};

class SocketServer
{
public:
    SocketServer();
    ~SocketServer();

    void startServer();
    void stopServer();

private:
    void bindServer();
    void listenServer();
    void acceptServer();
    void workingServer(std::shared_ptr<SockInfo> pInfo);

private:
    int m_socketFD;
    int m_bindRet;
    int m_listenRet;
    std::atomic<int> m_clientNum;
    std::atomic<int> m_clientMax;
    std::atomic<bool> m_stopServer;
    std::unique_ptr<ThreadPool> m_pPool;
    sockaddr_in m_saddr;
};

#endif