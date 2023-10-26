//
// Created by 24546 on 2023/9/22.
//
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#include<string.h>
#include <pthread.h>
#include "stdio.h"
#include <sys/time.h>
#include <sys/msg.h>


#include "../../command/StartRespCommand.h"
#include "../../command/CommandType.h"
#include "../../command/PingCommand.h"
#include "../../command/ClientInitRespCommand.h"
#include "../../command/DescribeRespCommand.h"
#include "../pojo/Packet.h"
#include "../../log/log.h"
#include "../pojo/PacketGroup.h"
#include "../../trendline/PacketGroupDelay.h"
#include "../pojo/PacketSendTime.h"

extern int sock;

extern int packetGroupMsgId;
extern int packetSendTimeMsgId;

extern pthread_mutex_t packetCountMutex;

void *tcpListener(void *args);

struct timeval getTimeStamp();

struct PingInfo *pingDecode(char data[]);

int sendPong(int sequence, long long timestamp, int processTimeMs,
             int receiveCount, char delayChangeLevel);

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);

struct Packet *removeNode(struct Packet *head, struct Packet *ptr);

struct ClientInitRespInfo *clientInitTcpDecode(char data[]);

struct ClientInitRespInfo *desRespDecode(char data[]);

struct StartRespInfo *startRespDecode(char data[]);

long getLeadTime(struct timeval *start, struct timeval *end);

long long getSystemTimestamp();

int revPacketGroupMsg(int msgId,void* data);
int  udpRevMsg(int msgId,void* data);
int sendMsg(int msgID,void* data,int size);


int delPacketGroup(int groupIndex);

int addPacketGroupDelay(struct PacketGroupDelay *packetGroup);

char calculate(struct PacketGroupDelay *groupDelays);

extern struct PacketGroup *packetGroupList;
extern struct PacketGroupDelay *groupDelayList;



int tcpListenerFlag = 1; //tcp监听任务flag
int RTT = 0;
int tcpThreadMsgId = -1;


extern pthread_t tcpTask;
extern int tcpPort;
extern struct Packet *head;
extern int udpThreadMsgId;


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

      tcpThreadMsgId = msgget((key_t) 1006, 0664 | IPC_CREAT);//获取消息队列
    if (tcpThreadMsgId == -1) {
        log_error("tcpThreadMsgId err");
        exit(-1);
    }
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
        long long revTime = 0ll;
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

            case START_RESP: {
                struct StartRespInfo *info;
                readLength = 13;
                char data[readLength];
                length = recv(sock, data, readLength, 0); //接收服务端发来的消息
                recBuf[readLength] = '\0';
                recBuf[0] = rev[0];
                arrCopy(data, 0, recBuf, 1, readLength);
                info = startRespDecode(recBuf);
                startTime = info->time;
            
                log_info("startTime=%lld", startTime);
                log_info("------ rev start resp msg ------\n");

                break;
            }

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
                log_info("------ seq=%d   rtt=%d   timestamp=%lld  nonPing=%lld------ \n", p->sequence, RTT,
                         p->timestamp, nowPingTimestamp);
                //阻塞一个rtt时间
                while (1) {
                    nowTime = getSystemTimestamp();
                    if (nowTime < revTime + RTT) {
                        continue;
                    } else {
                        break;
                    }
                }

                // struct  msgququeInfo *msg;
                // msg = (struct msgququeInfo *) malloc(sizeof(struct msgququeInfo));
                // msg->msg = 1;
                // sendMsg(tcpThreadMsgId,(void*)msg,sizeof(struct msgququeInfo));
                
                // struct msgququeInfo *rcvMsg;
                // rcvMsg = (struct msgququeInfo *) malloc(sizeof(struct msgququeInfo));
                // udpRevMsg(udpThreadMsgId,rcvMsg);
                // log_info("%d",rcvMsg->msg);

                // struct Packet *point, *list, *ptr;
                // point = head;
                // int count = 0;

                // //遍历链表 复制一份
                // while (point != NULL) {
                //     struct Packet *packet;
                //     packet = (struct Packet *) malloc(sizeof(struct Packet));
                //     packet->sendTime = point->sendTime;
                //     packet->revTime = point->revTime;
                //     if (list == NULL) {
                //         list = packet;
                //         ptr = packet;
                //     } else {
                //         ptr->next = packet;
                //         ptr = ptr->next;
                //     }
                //     point = point->next;
                // }
                // if (ptr != NULL) {
                //     ptr->next = NULL;
                // }



                //point = NULL;


                int revPacketCount = 0;
                // struct Packet *aidePoint;
                // aidePoint = list;

                long long start = getSystemTimestamp();
                struct PacketSendTime *pactketPtr = NULL;
                // printf("------ seq=%d   prevPing=%lld   nowPing=%lld",p->sequence,prevPingTimestamp,nowPingTimestamp);
                while (true) {
                  pactketPtr = (struct PacketSendTime *) malloc(sizeof(struct PacketSendTime));
                    int result = revPacketSendTimeMsg(packetSendTimeMsgId,pactketPtr);
                    if (result == -1)
                    {
                        log_info("revPacketSendTimeMsg rev error");
                        break;
                    }
                    
                    //printf("%d\n",aidePoint->sendTime);
                    if (pactketPtr->sendTime >= prevPingTimestamp && pactketPtr->sendTime < nowPingTimestamp) {
                        ++revPacketCount;
                    }
                    // aidePoint = aidePoint->next;
                }
                //printf("time:%ld\n",(getSystemTimestamp() - start));
                log_info("------ revPacketCount %d------\n", revPacketCount);

//                free(ptr);
//                free(list);
//                list = NULL;
//                ptr = NULL;
//
                struct PacketGroup *curr;
                struct PacketGroup *prev = NULL;
                int result = 0;
                while (true){
                    curr = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
                    result = revPacketGroupMsg(packetGroupMsgId,curr);
                    //log_info("------- curr->groupIndex = %d",curr->groupIndex);
                    if(result == -1){
                        break;
                    }
                    else{  
                        struct PacketGroupDelay *packetGroupDelay;
                        int size = sizeof(struct PacketGroupDelay);
                        packetGroupDelay = (struct PacketGroupDelay *) malloc(size);
                        if (prev != NULL){
                            packetGroupDelay->arrivalTimeMs = curr->firstArrivalPacketTimestamp;
                            packetGroupDelay->sendTimeMs = curr->firstSentPacketTimestamp;
                            packetGroupDelay->sendDelta =  (int) (curr->lastSentPacketTimestamp -
                                                                  prev->lastSentPacketTimestamp);
                            packetGroupDelay->recvDelta = (int) (curr->lastArrivalPacketTimestamp -
                                                                 prev->lastArrivalPacketTimestamp);
                            packetGroupDelay->next = NULL;

                             log_info("  recvDelta  %d - sendDelta  %d = %d",packetGroupDelay->recvDelta,packetGroupDelay->sendDelta,packetGroupDelay->recvDelta - packetGroupDelay->sendDelta);

                            //调试临时赋值
                             //packetGroupDelay->recvDelta = packetGroupDelay->sendDelta - 1;
                            // packetGroupDelay->sendDelta = 0;
                            addPacketGroupDelay( packetGroupDelay);
                        }
                        prev = curr;
                    }

                }
                int level = 0;
                //TODO 斜率计算有问题，一直重复一个值
                char delayChangeLevel = calculate(groupDelayList);
                log_info("delayChangLevel = %d",delayChangeLevel);
                free(groupDelayList);
                groupDelayList = NULL;
    



                processTime = getSystemTimestamp() - revTime;
                //printf("------ rev packet count=%d ------\n", revPacketCount);
                //fflush(stdout);

                //TODO delayChangLevel 需要计算得到  暂时用 0 = 48 代替
                sendPong(p->sequence, p->timestamp, processTime, revPacketCount, delayChangeLevel);

                revPacketCount = 0;
                oldNowPingTimestamp = nowPingTimestamp;
                oldPrevPingTimestamp = prevPingTimestamp;
                // free(list);
                // list = NULL;
                break;
            }
            default:
                printf("error");
        }
        //printf("%s\n", recBuf);
//        break;

    }

}