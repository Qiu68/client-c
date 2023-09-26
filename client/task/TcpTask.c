//
// Created by 24546 on 2023/9/22.
//
#include <winsock2.h>
#include "stdio.h"
#include <sys/time.h>
#pragma comment(lib, "wsock32.lib")
#include <pthread.h>
#include "../../command/CommandType.h"
#include "../../command/PingCommand.h"
#include "../../command/ClientInitRespCommand.h"
#include "../../command/DescribeRespCommand.h"

extern SOCKET sock;
void *tcpListener(void* args);
struct timeval getTimeStamp();
struct PingInfo* pingDecode(char data[]);
int sendPong(int sequence,long long timestamp,int processTimeMs,
              int receiveCount,char delayChangeLevel);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);
struct ClientInitRespInfo* clientInitTcpDecode(char data[]);
struct ClientInitRespInfo* desRespDecode(char data[]);
long getLeadTime(struct timeval *start,struct timeval *end);

int tcpListenerFlag = 1; //tcp监听任务flag
int RTT = 0;

extern pthread_t tcpTask;
extern int tcpPort;

//分包最大字节数
int packageSize = 0;
//路由器tcp端口
int tcpPort = 0;
char localHost[4]={'\0'};
//总帧数
int fileFrameCount = 0;
//文件字节数
long long  fileLength = 0;


void tcpListenerInit(){
    pthread_create(&tcpTask,NULL,tcpListener,NULL);
}

void *tcpListener(void* args){
    printf("------tcp监听线程开启------\n");
    char recBuf[1500];
    char rev[1];
    int length;
    long long nowPingTimestamp,prevPingTimestamp=0l;
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

            case DESCRIBE_RESP: {
                struct DescribeRespCommand *info;
                readLength = 31;
                printf("------ rev describe resp msg ------\n");
                fflush(stdout);

                char data[readLength];
                recv(sock,data,readLength,0);
                recBuf[0] = rev[0];
                arrCopy(data,0, recBuf,1,readLength);
                info = desRespDecode(recBuf);

                fileLength = info->length;
                fileFrameCount = info->frameCount;
                break;
            }

            case START_RESP:
                readLength = 13;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("------ rev start resp msg ------\n");
                fflush(stdout);


                break;

            case PAUSE_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("------ rev start resp msg ------\n");
                fflush(stdout);
                break;

            case NACK_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                printf("------ rev start nack resp ------\n");
                fflush(stdout);
                break;

            case CLIENT_INIT_RESP: {
                struct ClientInitRespInfo *info;
                printf("------ rev client init resp ------\n");
                fflush(stdout);

                readLength = 17;
                char data[readLength];
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                data[readLength] = '\0';
                recBuf[0] = rev[0];
                arrCopy(data,0,recBuf,1,readLength);

                info = clientInitTcpDecode(recBuf);
                arrCopy(info->clientHost,0,localHost,0,4);
                tcpPort = info->clientPort;
                printf("packageSize = %d\n",info->packetSize);
                break;
            }

            case PING: {
                char data[16];
                struct PingInfo *p;
                data[0] = rev[0];
                revTime = getTimeStamp();
                //剩余字节数
                readLength = 16;
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                recBuf[0] = rev[0];
                arrCopy(data,0,recBuf,1,readLength);
//                recBuf[readLength + 1] = '\0';
                printf("------- rec ping msg ------\n");
                fflush(stdout);

                p = pingDecode(recBuf);
                prevPingTimestamp = nowPingTimestamp;
                nowPingTimestamp = p->timestamp;
                RTT = p->rtt;
                printf("------ rtt = %d ------ \n",RTT);

                //阻塞一个rtt时间
                struct timeval time;
                while(1){
                    time = getTimeStamp();
                    if (time.tv_usec < (revTime.tv_usec) + (RTT) * 1000){
                        continue;}
                    else{
                        break;
                    }
                }
                time = getTimeStamp();
                long timestamp = getLeadTime(&revTime,&time);
                printf("处理耗时=%ld",timestamp);
                sendPong(p->sequence,p->timestamp,timestamp,0,'\0');
                break;
            }
            default:
                printf("error");
        }
        //printf("%s\n", recBuf);
//        break;
    }

}