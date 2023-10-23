//
// Created by 24546 on 2023/9/25.
//

#include <sys/socket.h>


extern int sock;

int sendTcpMsg(char data[],int length){
    int size = send(sock,data,length,0);
    return size;
}
