//
// Created by 24546 on 2023/9/25.
//

#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

extern SOCKET sock;

int sendTcpMsg(char data[]){
    int length = send(sock,data,sizeof(data),0);
    return length;
}
