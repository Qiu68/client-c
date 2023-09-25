//
// Created by 24546 on 2023/9/25.
//
#include <stdio.h>
#include <pthread.h>
#include <ws2tcpip.h>
#include "../command/CommandLenght.h"
#define pf printf


void tcpListenerInit();
int tcpInit();
void udpListenerInit();
void encode(char data[],int sequence);

int sendTcpMsg(char data[]);

pthread_t udpTask;
pthread_t tcpTask;

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

    //发送客户端初始化指令
    char data[CLIENT_INIT_REQ_LENGTH];
    encode(data,1);
    printf("------ send client tcp init msg ------\n");
    int status = sendTcpMsg(data);

    if(status > 0){
        printf("------ send client tcp init msg success ------\n");
    }
    else{
        printf("------- send client tcp init msg error ------\n");
    }

    printf("------ data size is %d\n",sizeof(data));



    pthread_join(tcpTask,NULL);
    pthread_join(udpTask,NULL);
}

void getDescribe(){

}