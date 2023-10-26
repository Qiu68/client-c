//
// Created by 24546 on 2023/9/22.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include "../../log/log.h"

//#define ServerHost "47.103.69.132"
#define ServerHost "192.168.50.100"
#define ServerTcpPort  8173
int sock;
int tcpInit(){
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    unsigned optVal;
    int optLen = sizeof(int);
    int bufSize = 1024 * 1024 * 40;
    //设置接收缓冲区
    setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&bufSize,sizeof(int));
    //向服务器发起请求
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(ServerHost);
    sockAddr.sin_port = htons(8172);
    int stat = connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));

    //getsockname(sock, &sockAddr, (int *) sizeof(SOCKADDR));
    return stat;

//    //接收服务器传回的数据
//    char szBuffer[MAXBYTE] = {0};
//    recv(sock, szBuffer, MAXBYTE, 0);
//    //输出接收到的数据
//    printf("Message form server: %s\n", szBuffer);
//    //关闭套接字
//    closesocket(sock);
//    //终止使用 DLL
//    WSACleanup();
//    system("pause");
//    return 0;

}