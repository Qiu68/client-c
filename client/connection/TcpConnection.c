//
// Created by 24546 on 2023/9/22.
//

#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include<string.h>

#define ServerHost "127.0.0.1"
#define ServerTcpPort  8173
SOCKET sock;
int tcpInit(){
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //向服务器发起请求
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(ServerHost);
    sockAddr.sin_port = htons(8172);
    int stat = connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

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