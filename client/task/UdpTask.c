//
// Created by 24546 on 2023/9/25.
//
#include "stdio.h"
#include <pthread.h>
#include <ws2tcpip.h>
#include "../../packet/PacketType.h"
#include "../../packet/PacketLength.h"
#include "../../packet/ClientInitRespPacket.h"
#include "../../packet/FramePacket.h"



struct clientInitRespPacket* clientInitRespDecode(char data[]);
struct FramePacket *  FramePacketDecode(char data[],int length);


extern struct sockaddr_in addr;
extern int udpSockFd;

int udpListenerFlag = 1;
int revPackageCount = 0;


void *udpListener(void* args);
long long getSystemTimestamp();

extern pthread_t udpTask;
extern int udpRoutePort;

void udpListenerInit(){
    printf("------ udp 监听开启 ------ \n");

    pthread_create(&udpTask,NULL,udpListener,NULL);
}

void *udpListener(void* args){
    char buf[1500];

    while (udpListenerFlag){

        //printf("udp 111");
        socklen_t len=sizeof(addr);
        int length = recvfrom(udpSockFd,&buf,sizeof(buf),0,(struct sockaddr*)&addr,&len);
        //printf("udp 222");

        int type = buf[0];

        switch (type) {

            //接收打洞端口
            case CLIENT_UDP_INIT_RESP:{
                printf("------ rev udp init msg ------\n");
                struct clientInitRespPacket *respPacket;
                respPacket = clientInitRespDecode(buf);
                udpRoutePort = respPacket->clientPort;
                break;
            }

            //接收帧数据
            case FRAME:{
                printf("------ rev udp frame msg ------\n");
                struct FramePacket *framePacket;
                framePacket = FramePacketDecode(buf,length);
                printf("------ frameIndex = %d   packageIndex = %d ------\n",framePacket->frameIndex,framePacket->packageIndex);
                break;
            }
        }
    }
}