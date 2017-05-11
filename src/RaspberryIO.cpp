#include "RaspberryIO.h"

namespace Raspberry{

RaspberryIO::RaspberryIO()
{
    bzero(&serverAddr, sizeof(serverAddr));
    serverSockfd = socket(PF_INET, SOCK_DGRAM,0);
    if(serverSockfd == -1)
    {
        printf("error[%d]\n",__LINE__);
        exit(-1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(6677);

    int reuseraddr = 1;
    setsockopt (serverSockfd, SOL_SOCKET, SO_REUSEADDR, &reuseraddr,  sizeof(reuseraddr));

    if(-1 == bind(serverSockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        printf("error[%d]\n",__LINE__);
        exit(-1);
    }


    bzero(&clientAddr, sizeof(clientAddr));
    clientSockfd = socket(PF_INET, SOCK_DGRAM,0);
    if(clientSockfd == -1)
    {
        printf("error[%d]\n",__LINE__);
        exit(-1);
    }

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(6678);
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    reuseraddr = 1;
    setsockopt (clientSockfd, SOL_SOCKET, SO_REUSEADDR, &reuseraddr,  sizeof(reuseraddr));

    isClassTerminate = false;
    SocketThread = new std::thread(LoopSocketThread , this);
}


RaspberryIO::~RaspberryIO()
{
    isClassTerminate = true;
    shutdown (serverSockfd, SHUT_RDWR);
    SocketThread->join ();
    SocketThread = NULL;
    close(serverSockfd);
    close(clientSockfd);

}

void RaspberryIO::LoopSocketThread(RaspberryIO* pdata)
{
    while(!pdata->isClassTerminate)
    {
        char recvline[1024] = {0};
        ssize_t count = recvfrom(pdata->serverSockfd, recvline, 1024, 0,  NULL, NULL);
        if(count > 0)
        {
            printf("%s\n", recvline);
        }
    }

}


}
