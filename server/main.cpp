#include <termios.h>
#include "server.h"

// 设置终端为无缓冲模式（直接读取按键，无需按 Enter）
void setNonBlockingTerminal()
{
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // 获取当前终端设置
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // 禁用缓冲和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 应用新设置
}

int main()
{
    setNonBlockingTerminal(); // set terminal mode

    pid_t pid = getpid(); // 获取当前进程的 PID
    std::cout << "Current Process ID (PID): " << pid << std::endl;

    SocketServer server;
    server.startServer();

    char ch;
    while (true) // end control
    {
        if (read(STDIN_FILENO, &ch, 1) == 1)
        {
            if (ch == 'q') // get key Q
            {
                server.stopServer();
                std::cout << "Quitting..." << std::endl;
                break;
            }
            std::cout << "You pressed: " << ch << std::endl;
        }
    }

    return 0;
}