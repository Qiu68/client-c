//
// Created by 24546 on 2023/9/25.
//
#include <stdio.h>
#include <winsock2.h>
#include "stdio.h"
#pragma comment(lib, "wsock32.lib")
#include "../command/CommandLenght.h"
#include "../packet/PacketType.h"
#include "../packet/PacketLength.h"
#define pf printf


void tcpListenerInit();
int tcpInit();
void udpListenerInit();
void encode(char data[],int sequence);
void clientInitEcode(char data[], char clientHost[4],int clientPort);
void startReqEcode(char data[], int sequence, long long resId,
                   char protocol,int receivePort,int startFrameIndex);

int sendTcpMsg(char data[],int length);
void desReqEcode(char data[], int sequence, long long resId);
void getDescribe(long long resId);
void sendTcpInit();
void sendUdpInit();
int udpInit();
void setStartFrameIndex(int frameIndex);

extern  struct sockaddr_in addr;
extern int udpSockFd;
extern char localHost[4];
extern int tcpPort;

int startFrameIndex;
int udpRoutePort = 0;
long long resourceId;


int sendUdpMsg(char msg[]){
    sendto(udpSockFd,msg,sizeof(msg) + 1,0,(struct sockaddr *)&addr,sizeof(addr));
}


int clientInit(){
    int stat = tcpInit();
    if(stat < 0){
        printf("连接失败！");
        return -1;
    }

    udpInit();


    //开启tcp监听
    tcpListenerInit();
    //开启udp监听
    udpListenerInit();

    //发送tcp初始化指令
    sendTcpInit();

    //发送udp初始化指令
    sendUdpInit();

}


void sendUdpInit(){
    char data[CLIENT_UDP_INIT_REQ_LENGTH];
    //TODO 一直发送udp初始化请求，直到收到udpRoutePort
    while (1){
        printf("1111111");
        if(udpRoutePort == 0) {
            clientInitEcode(data,localHost,tcpPort);
            sendUdpMsg(data);
            Sleep(500);
        }
        else{
            //已经接收到udpRoutePort
                break;
            }
        }
    printf("------  udp route port %d ------\n",udpRoutePort);
    }



void sendTcpInit(){
    char data[CLIENT_INIT_REQ_LENGTH];
    encode(data,1);
    printf("------ send client tcp init msg ------\n");
    int status = sendTcpMsg(data,CLIENT_INIT_REQ_LENGTH);

    if(status > 0){
        printf("------ send client tcp init msg success ------\n");
    }
    else{
        printf("------- send client tcp init msg error ------\n");
    }

    printf("------ data size is %d\n",sizeof(data));
}

void getDescribe(long long resId){
    //设置消息长度
    char data[DESCRIBE_REQ_LENGTH];
    //消息编码
    desReqEcode(data,12,resId);
    //通过tcp发送消息
    //printf("sizeof = %d",sizeof(data));

    int status = sendTcpMsg(data,DESCRIBE_REQ_LENGTH);

    if(status > 0){
        printf("------ send describe msg success ------\n");
    }
    else{
        printf("------- send describe msg error ------\n");
    }

    printf("------ data size is %d\n",sizeof(data));
}

void setStartFrameIndex(int frameIndex){
    startFrameIndex = frameIndex;
}

void setResourceId(long long id){
    resourceId = id;
}

void startPlay(int frameIndex){
    startFrameIndex = frameIndex;

    /**
     * 推流步骤
     * 1.首先确保客户端接收到tcp远程端口
     * 2.然后确保客户端接收到udp远程端口
     * 3.最后开始推流
     */


    // 1. 确保客户端接收到tcp远程端口
    while(TRUE){
        if (tcpPort != 0){
            break;
        }
        else{
            sendTcpInit();
            Sleep(500);
        }
    }

    // 2. 确保客户端接收到udp远程端口
    while(TRUE){
        if (udpRoutePort != 0){
            break;
        }
        else{
            sendUdpInit();
        }
    }

    // 3. 开始推流
    char data[START_REQ_LENGTH];
    startReqEcode(data,0,resourceId,1,udpRoutePort,startFrameIndex);
    sendTcpMsg(data,START_REQ_LENGTH);
    printf("------ send start play msg ------");
}
