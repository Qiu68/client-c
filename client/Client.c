//
// Created by 24546 on 2023/9/25.
//
#include <stdio.h>
#include "../command/CommandLenght.h"
#define pf printf


void tcpListenerInit();
int tcpInit();
void udpListenerInit();
void encode(char data[],int sequence);

int sendTcpMsg(char data[],int length);
void desReqEcode(char data[], int sequence, long long resId);
void getDescribe(long long resId);
void sendTcpInit();
void sendUdpInit();
void setStartFrameIndex(int frameIndex);



int startFrameIndex;
int udpRoutePort = 0;

int clientInit(){
    int stat = tcpInit();
    if(stat < 0){
        printf("连接失败！");
        return -1;
    }

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

    //TODO 一直发送udp初始化请求，直到收到udpRoutePort
    while (1){
        if(udpRoutePort != 0){
            //已经接收到udpRoutePort
            break;
        }
    }
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
