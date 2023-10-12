//
// Created by 24546 on 2023/10/11.
//
#include <WinSock2.h>
#include <Windows.h>
//加载lib
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include "stdio.h"
#include "../../command/CommandType.h"
#include "../../command/PongCommand.h"
#include "../../command/CommandLenght.h"

extern SOCKET sock;
void pauseEncode(char data[],int sequence);

int sendPause(int sequence){
    char data[PAUSE_REQ_LENGTH];
    pauseEncode(data,sequence);
    send(sock,data,PAUSE_REQ_LENGTH,0);
    //printf("------ send pong msg success ------\n");
    return 1;
}