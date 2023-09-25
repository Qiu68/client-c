//
// Created by 24546 on 2023/9/25.
//
#include "stdio.h"
#include <pthread.h>
#include <ws2tcpip.h>

extern struct sockaddr_in addr;
extern int udpSockFd;
int udpListenerFlag = 1;

void *udpListener(void* args);

extern pthread_t udpTask;

void udpListenerInit(){
    printf("------ udp 监听开启 ------ \n");

    pthread_create(&udpTask,NULL,udpListener,NULL);
}

void *udpListener(void* args){
    char buf[1500];
    while (udpListenerFlag){
        //printf("udp 111");
        socklen_t len=sizeof(addr);
        recvfrom(udpSockFd,&buf,sizeof(buf),0,(struct sockaddr*)&addr,&len);
        //printf("udp 222");
    }
}