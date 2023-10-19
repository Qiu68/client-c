//
// Created by 24546 on 2023/9/22.
//
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include<string.h>
#include <pthread.h>
#include "stdio.h"
#include <sys/time.h>



#include "../../command/StartRespCommand.h"
#include "../../command/CommandType.h"
#include "../../command/PingCommand.h"
#include "../../command/ClientInitRespCommand.h"
#include "../../command/DescribeRespCommand.h"
#include "../pojo/Packet.h"
#include "../../log/log.h"

extern int sock;

extern pthread_mutex_t packetCountMutex;

void *tcpListener(void *args);

struct timeval getTimeStamp();

struct PingInfo *pingDecode(char data[]);

int sendPong(int sequence, long long timestamp, int processTimeMs,
             int receiveCount, char delayChangeLevel);

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);

struct Packet * removeNode(struct Packet *head,struct Packet *ptr);

struct ClientInitRespInfo *clientInitTcpDecode(char data[]);

struct ClientInitRespInfo *desRespDecode(char data[]);

struct StartRespInfo* startRespDecode(char data[]);

long getLeadTime(struct timeval *start, struct timeval *end);

long long getSystemTimestamp();

struct Packet *removeNode(struct Packet *head, struct Packet *ptr);


int tcpListenerFlag = 1; //tcp监听任务flag
int RTT = 0;


extern pthread_t tcpTask;
extern int tcpPort;
extern struct Packet *head;

//分包最大字节数
int packageSize = 0;
long long startTime = 0;
long long nowPingTimestamp = 0ll;
long long prevPingTimestamp = 0ll;
long long oldNowPingTimestamp = 0ll;
long long oldPrevPingTimestamp = 0ll;
//路由器tcp端口
int tcpPort = 0;
char localHost[4] = {'\0'};
//总帧数
int fileFrameCount = 0;
//文件字节数
long long fileLength = 0;


void tcpListenerInit() {
    pthread_create(&tcpTask, NULL, tcpListener, NULL);
}

void *tcpListener(void *args) {
    log_info("------tcp监听线程开启------\n");
    char recBuf[1500];
    char rev[1];
    int length;
    while (tcpListenerFlag) {
        //printf("读取消息:");

        recBuf[0] = '\0';
        length = recv(sock, rev, 1, 0); //接收服务端发来的消息

        rev[length] = '\0';
        long long  revTime = 0ll;
        long long nowTime = 0ll;
        long long processTime = 0ll;
        int type = rev[0];
        int readLength;

        switch (type) {



            case DESCRIBE_RESP: {
                struct DescribeRespCommand *info;
                readLength = 31;
                log_info("------ rev describe resp msg ------\n");

                char data[readLength];
                recv(sock, data, readLength, 0);
                recBuf[0] = rev[0];
                arrCopy(data, 0, recBuf, 1, readLength);
                info = desRespDecode(recBuf);

                fileLength = info->length;
                fileFrameCount = info->frameCount;
                break;
            }

            case START_RESP:{
                struct StartRespInfo *info;
                readLength = 13;
                char data[readLength];
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                recBuf[0] = rev[0];
                arrCopy(data, 0, recBuf, 1, readLength);
                info = startRespDecode(recBuf);
                startTime = info->time;
                log_info("startTime=%lld\n",startTime);
                log_info("------ rev start resp msg ------\n");

                break;}

            case PAUSE_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                log_info("------ rev start resp msg ------\n");
                break;

            case NACK_RESP:
                readLength = 5;
                length = recv(sock, recBuf, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                log_info("------ rev start nack resp ------\n");
                break;

            case CLIENT_INIT_RESP: {
                struct ClientInitRespInfo *info;
                log_info("------ rev client init resp ------\n");

                readLength = 17;
                char data[readLength];
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                data[readLength] = '\0';
                recBuf[0] = rev[0];
                arrCopy(data, 0, recBuf, 1, readLength);

                info = clientInitTcpDecode(recBuf);
                arrCopy(info->clientHost, 0, localHost, 0, 4);
                tcpPort = info->clientPort;
                packageSize = info->packetSize;
                log_info("packageSize = %d\n", info->packetSize);
                break;
            }

            case PING: {

                char data[16];
                struct PingInfo *p;
                revTime = getSystemTimestamp();
                //剩余字节数
                readLength = 16;
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                data[readLength] = '\0';
                recBuf[0] = rev[0];
                arrCopy(data, 0, recBuf, 1, readLength);
                //printf("------- rec ping msg ------\n");
                //fflush(stdout);

                p = pingDecode(recBuf);
                prevPingTimestamp = nowPingTimestamp;
                nowPingTimestamp = p->timestamp - startTime;
                //printf("timestamp=%lld -   startTime=%lld = %lld\n",p->timestamp,startTime,p->timestamp-startTime);
                RTT = p->rtt;
                log_info("------ seq=%d   rtt=%d   timestamp=%lld  nonPing=%lld------ \n", p->sequence,RTT,p->timestamp,nowPingTimestamp);
                //阻塞一个rtt时间
                while (1) {
                    nowTime = getSystemTimestamp();
                    if (nowTime < revTime + RTT) {
                        continue;
                    } else {
                        break;
                    }
                }
                struct Packet *point, *list, *ptr;
                point = head;
                int count = 0;

                //遍历链表 复制一份
                while (point != NULL) {
                    struct Packet *packet;
                    packet = (struct Packet *) malloc(sizeof(struct Packet));
                    packet->sendTime = point->sendTime;
                    packet->revTime = point->revTime;
                    if (list == NULL) {
                        list = packet;
                        ptr = packet;
                    } else {
                            ptr->next = packet;
                            ptr = ptr->next;
                    }
                    point = point->next;
                }
                if (ptr != NULL) {
                    ptr->next = NULL;
                }

                //point = NULL;


                int revPacketCount = 0;
                struct Packet *aidePoint;
                aidePoint = list;

                long long start = getSystemTimestamp();
               // printf("------ seq=%d   prevPing=%lld   nowPing=%lld",p->sequence,prevPingTimestamp,nowPingTimestamp);
                while(aidePoint != NULL){
                    //printf("%d\n",aidePoint->sendTime);
                    if (aidePoint->sendTime >= prevPingTimestamp && aidePoint->sendTime < nowPingTimestamp){
                        ++revPacketCount;
                    }
                    aidePoint = aidePoint->next;
                }
                //printf("time:%ld\n",(getSystemTimestamp() - start));
                log_info("------ revPacketCount %d------\n",revPacketCount);

                processTime = getSystemTimestamp() - revTime;
               //printf("------ rev packet count=%d ------\n", revPacketCount);
                //fflush(stdout);
                sendPong(p->sequence, p->timestamp, processTime, revPacketCount, '1' - 48);


                revPacketCount = 0;
                oldNowPingTimestamp = nowPingTimestamp;
                oldPrevPingTimestamp = prevPingTimestamp;
                free(list);
                list = NULL;
                break;
            }
            default:
                printf("error");
        }
        //printf("%s\n", recBuf);
//        break;

    }

}