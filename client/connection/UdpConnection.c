//
// Created by 24546 on 2023/9/22.
//
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
//#define ServerHost  "47.103.69.132"
#define ServerHost "127.0.0.1"
#define ServerTcpPort  8173

struct sockaddr_in addr;
int udpSockFd;
int udpInit() {
    udpSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    //创建网络通信对象
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8173);
    addr.sin_addr.s_addr = inet_addr(ServerHost);
    return udpSockFd;
}