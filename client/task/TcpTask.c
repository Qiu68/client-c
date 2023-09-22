//
// Created by 24546 on 2023/9/22.
//
#include <winsock2.h>
#include "stdio.h"
#include <sys/time.h>
#pragma comment(lib, "wsock32.lib")
#include <stdio.h>
#include <pthread.h>
#include "../../command/CommandType.h"
extern SOCKET sock;
void *tcpListener(void* args);
struct timeval getTimeStamp();

int tcpListenerFlag = 1;

void tcpListenerInit(){
    printf("111");
    pthread_t t1;
    pthread_create(&t1,NULL,tcpListener,NULL);
}

void *tcpListener(void* args){
    printf("------tcp监听线程开启------");
    char recBuf[1500];
    char rev[1];
    int length;
    while(tcpListenerFlag)
    {
        //printf("读取消息:");
        recBuf[0] = '\0';
        length = recv(sock, rev, 1, 0); //接收服务端发来的消息
        rev[length] = '\0';
        struct timeval revTime;
        struct timeval nowTime;
        int type = rev[0];
        int readLength;
        switch (type) {

            case DESCRIBE_RESP:
                readLength = 31;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("1");
                fflush(stdout);
                break;

            case START_RESP:
                readLength = 13;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("2");
                fflush(stdout);
                break;

            case PAUSE_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("3");
                fflush(stdout);
                break;

            case NACK_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("4");
                fflush(stdout);
                break;

            case CLIENT_INIT_RESP:
                readLength = 17;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("5");
                fflush(stdout);
                break;

            case PING:
                revTime = getTimeStamp();
                //剩余字节数
                readLength = 16;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("6");
                fflush(stdout);
                break;

            default:
                printf("error");
        }
        //printf("%s\n", recBuf);
//        break;
    }

}