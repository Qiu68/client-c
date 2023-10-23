//
// Created by 24546 on 2023/9/22.
//


#include <sys/socket.h>

#include "stdio.h"
#include "../../command/CommandType.h"
#include "../../command/PongCommand.h"
extern int sock;
void pongEcode(char data[], int sequence,  long long timestamp,
               int processTimeMs,int receiveCount,
               char delayChangeLevel);

int sendPong(int sequence,long long timestamp,int processTimeMs,
int receiveCount,char delayChangeLevel){
    char data[22];
    pongEcode(data,sequence,timestamp,processTimeMs,receiveCount,delayChangeLevel);
    send(sock,data,22,0);
    //printf("------ send pong msg success ------\n");
    return 1;
}