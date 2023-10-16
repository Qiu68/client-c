//
// Created by root on 10/13/23.
//
//
// Created by 24546 on 2023/10/11.
//

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")


#include "../../command/CommandType.h"
#include "../../command/PongCommand.h"
#include "../../command/CommandLenght.h"

extern int sock;

int sendNackReq(char *buf,int length){
    send(sock,buf,length,0);
    //printf("------ send pong msg success ------\n");
    return 1;
}