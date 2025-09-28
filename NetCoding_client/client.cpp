#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>

int INPUTNUM = 0;

int main()
{
    // 1 创建socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2 绑定本地 ip 和 port(端口)
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;   // 初始化协议
    saddr.sin_port = htons(8000); // 初始化端口, 使用htons由小端转换为大端

    inet_pton(AF_INET, "192.168.31.196", &saddr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connect");
        return -1;
    }
    else
    {
        std::cout << "Connect Succecc" << std::endl;
    }

    int num = 0;
    // 3 连接成功之后,开始通信
    while (1)
    {
        // 客户端发送数据
        char buffer[1024];
        sprintf(buffer, "hello%d... \n", num++);
        send(fd, buffer, strlen(buffer + 1), 0);

        // 接数据
        memset(buffer, 0, sizeof(buffer));
        int len = recv(fd, buffer, sizeof(buffer), 0);
        if (len > 0)
        {
            printf("server: %s\n", buffer);
        }
        else if (len == 0)
        {
            printf("服务器断开连接...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
        sleep(1);
    }

    // 6 通信结束
    close(fd);

    std::string str;

    return 0;
}
