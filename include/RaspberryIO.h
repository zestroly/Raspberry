#ifndef RASPBERRYIO_H
#define RASPBERRYIO_H
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

namespace Raspberry{

class RaspberryIO
{
public:
    RaspberryIO();
    ~RaspberryIO();
    void setIO(int pin, int value);
    int getIO(int pin);

    int setIoEdge(int channel, int edge, int Bouncetime);

    static void LoopSocketThread(RaspberryIO* pdata);
private:
    bool isClassTerminate;

    //建立本地udp服务，接受触发信号
    int serverSockfd;
    struct sockaddr_in serverAddr;

    //建立本地udp客户端，发送 gpio 配置
    int clientSockfd;
    struct sockaddr_in  clientAddr;

    std::thread  *SocketThread;
};

}
#endif // RASPBERRYIO_H
