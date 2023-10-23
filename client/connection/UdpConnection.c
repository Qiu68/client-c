//
// Created by 24546 on 2023/9/22.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../log/log.h"

#pragma comment(lib, "ws2_32.lib")
#define ServerHost  "47.103.69.132"
//#define ServerHost "192.168.50.100"
#define ServerTcpPort  8173

struct sockaddr_in addr;
int udpSockFd;
int udpInit() {
    udpSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned optVal;
    int optLen = sizeof(int);
    int bufSize = 1024 * 1024 * 20;
    setsockopt(udpSockFd,SOL_SOCKET,SO_RCVBUF,&bufSize,sizeof(int));
    getsockopt(udpSockFd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen);
    log_info("Udp Buffer length: %d\n", optVal);
    //创建网络通信对象
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8173);
    addr.sin_addr.s_addr = inet_addr(ServerHost);
    return udpSockFd;
}