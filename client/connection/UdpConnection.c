//
// Created by 24546 on 2023/9/22.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "../../log/log.h"

//#define ServerHost  "47.103.69.132"
#define ServerHost "192.168.50.100"
#define ServerTcpPort  8173

struct sockaddr_in addr;
int udpSockFd;
int udpInit() {
    udpSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned optVal;
    int optLen = sizeof(int);
    int bufSize = 1024 * 1024 * 40;
    //设置接收缓冲区
    setsockopt(udpSockFd,SOL_SOCKET,SO_RCVBUF,&bufSize,sizeof(int));

    struct timeval timeout;
    timeout.tv_sec = 10; // 设置等待时间为10秒
    timeout.tv_usec = 0;
    setsockopt(udpSockFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    //创建网络通信对象
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8173);
    addr.sin_addr.s_addr = inet_addr(ServerHost);
    return udpSockFd;
}