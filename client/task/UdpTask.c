//
// Created by 24546 on 2023/9/25.
//
#include "stdio.h"
#include <pthread.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "../../packet/PacketType.h"
#include "../../packet/PacketLength.h"
#include "../../packet/ClientInitRespPacket.h"
#include "../../packet/FramePacket.h"
#include "../pojo/Packet.h"
#include "../pojo/Frame.h"
#include "checkFramTask.h"

#define pf printf



struct clientInitRespPacket* clientInitRespDecode(char data[]);
struct FramePacket *  FramePacketDecode(char data[],int length);

extern struct sockaddr_in addr;
extern int udpSockFd;

extern pthread_mutex_t packetCountMutex;
extern pthread_mutex_t  frameMutex;

extern struct Frame *frameList;
extern struct Frame *frameCompleteList;
extern struct Frame *frameInCompleteList;
extern struct frameLength *frameLengthList;
extern struct framePos *framePosList;

extern long long oldNowPingTimestamp;
extern long long oldPrevPingTimestamp;

int udpListenerFlag = 1;
int revPackageCount = 0;
//统计一个两个ping之间接收到的包数
struct Packet *head,*p,*q;
long long receiveTimeStamp = 0ll;
int beforeFrameIndex = 0;
int nowFrameIndex = 0;
long long  nowPacketTimestamp = 0ll;
long long beforePacketTimestamp = 0ll;
int beforePacketOrder;
int nowPacketOrder;
struct Frame *frame;


void *udpListener(void* args);
long long getSystemTimestamp();
int addFrame(struct Frame *frame);
long long  getTimeStampByUs();
int addFrameComplete(struct Frame *frame);
int addFrameInComplete(struct Frame *frame);
int addFrameLength(struct frameLength *frameLength);
int addFramePos(struct framePos *framePos);
int frameListSize();


extern pthread_t udpTask;
extern int udpRoutePort;
extern int packageSize;

void udpListenerInit(){

    printf("------ udp 监听开启 ------ \n");

    pthread_create(&udpTask,NULL,udpListener,NULL);
}

int addPacket(struct framePacket *package){
    struct FramePacket *framePacket;
    framePacket = package;

    nowPacketOrder = framePacket->packageIndex;

    if (nowPacketOrder == 1){
        frame->framePosition = framePacket->framePosition;
    }
    struct packetData *data;
    data = (struct packetData*) malloc(sizeof (struct packetData));
    data->data = framePacket->data;
    data->packageIndex = framePacket->packageIndex;
    data->packageLength = framePacket->dataLength;
    data->next = NULL;

    if (frame->packetNode == NULL){
        frame->packetNode = data;
    }
    else{
        struct packetData *ptr;
        ptr = frame->packetNode;
        //检查是否重复 不重复移动到链表尾部
        while(ptr->next != NULL){
            if (ptr->packageIndex == framePacket->packageIndex){
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        ptr->next = data;
    }
    return 1;
}

int addLossPacket(int i){
    struct lossPacket *lossPacket;

    lossPacket = (struct lossPacket*) malloc(sizeof (struct lossPacket));
    lossPacket->id = i;
    lossPacket->next = NULL;

    if (frame->lossPacketNode == NULL){
        frame->lossPacketNode = lossPacket;
    }
    else{
        struct lossPacket *ptr;
        ptr = frame->lossPacketNode;
        //检查是否重复 不重复移动到链表尾部
        while(ptr->next != NULL){
            if (ptr->id == i){
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        ptr->id = i;
        ptr->next = NULL;
    }
    return 1;
}

int packetProcess(struct framePacket *package){

    //包顺序到达
    if (nowPacketOrder - beforePacketOrder == 1){
        addPacket(package);
        beforePacketOrder = nowPacketOrder;
    }

    if (nowPacketOrder - beforePacketOrder  >1){
        addPacket(package);
        for (int i = beforePacketOrder; i < nowPacketOrder; ++i) {
            addLossPacket(i);
        }
    }

}

void *udpListener(void* args){
    char buf[1500];
    long long clearTime = 0l;


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
                long long s = getTimeStampByUs();
                //printf("------ rev udp frame msg ------\n");
                struct FramePacket *framePacket;
                q = (struct Packet *) malloc(sizeof (struct Packet));

                framePacket = FramePacketDecode(buf, length);
                q->sendTime = framePacket->sendTime;
                q->revTime = getSystemTimestamp();
                q->next = NULL;
                receiveTimeStamp = getSystemTimestamp();
                if (clearTime == 0){
                    clearTime = framePacket->sendTime;
                }

                //上锁
                pthread_mutex_lock(&packetCountMutex);
                if (head == NULL) {
                    head = q;
                    p = q;
                }
                else{
                    p->next = q;
                    p = p->next;
                }

                struct Packet *point,*aide;
                int count=0;
                point = aide = head;
                //5秒清理掉没用的数据
                //printf("111111111111111\n");
                if (framePacket->sendTime - clearTime >= 5000){
                    while (point != NULL) {
                        if (point->sendTime < oldPrevPingTimestamp) {

                            //移除首节点
                            if( head == point ){
                                //链表只有一个节点的情况
                                if(head->next == NULL){
                                    head = NULL;
                                }
                                else {
                                    head = head->next;
                                }
                            }

                                //移除尾结点
                            else if(point->next == NULL){
                                //遍历到ptr节点的上一个节点
                                while (aide->next != point){
                                    aide = aide->next;
                                }
                                //断开与ptr的连接
                                aide->next = NULL;
                            }

                                //中间节点
                            else{

                                //遍历到ptr节点的上一个节点
                                while (aide->next != point){
                                    aide = aide->next;
                                }
                                aide->next = point->next;
                            }
                            aide = NULL;
                            count++;
                        }
                        point = point->next;
                    }
                    printf("------ 清理%d个数据 ------\n",count);
                    clearTime = 0;
                }

                pthread_mutex_unlock(&packetCountMutex);



                nowFrameIndex = framePacket->packageIndex;

                if (framePacket->packageIndex == 1){
                    // 偏移量 帧长存起来
                    struct framePos *pos;
                    pos = (struct framePos*) malloc(sizeof (struct framePos));
                    pos->framePos = framePacket->framePosition;
                    pos->frameIndex = framePacket->frameIndex;
                    pos->next = NULL;
                    addFramePos(pos);

                    struct frameLength *aLength;
                    aLength = (struct frameLength*) malloc(sizeof(struct frameLength));
                    aLength->frameIndex = framePacket->frameIndex;
                    aLength->frameLength = framePacket->frameLength;
                    addFrameLength(aLength);
                }

                int packetCount = (framePacket->frameLength + 8) % (packageSize - 25)
                                  == 0 ? (framePacket->frameLength + 8) / (packageSize - 25)
                                       : ((framePacket->frameLength + 8) / (packageSize - 25)) + 1;
                //pf("------ frameIndex = %d   packageIndex = %d  frameLength = %d  packetCount = %d  ------\n",framePacket->frameIndex,framePacket->packageIndex,framePacket->frameLength,packetCount);
                //fflush(stdout);

                if (framePacket->frameIndex != beforeFrameIndex){
                    nowPacketTimestamp = getSystemTimestamp();
                }

                if (nowFrameIndex - beforeFrameIndex > 500){
                    //TODO 断网重传
                }

                //同一帧其他分包
                if (framePacket->frameIndex == beforeFrameIndex){
                    packetProcess(framePacket);
                }

                //帧顺序到达
                else if (framePacket->frameIndex - beforeFrameIndex == 1){
                    if (frame != NULL){
                        // 加入检查队列
                        //pthread_mutex_lock(&frameMutex);
                        addFrame(frame);
                        int count = frameListSize();
                        printf("frame size = %d\n",count);
                        //fflush(stdout);
                        //pthread_mutex_unlock(&frameMutex);
                    }
                    frame = (struct Frame *)malloc(sizeof(struct Frame));
                    frame->frameIndex = framePacket->frameIndex;
                    frame->frameLength = framePacket->frameLength;
                    frame->packetSum = packetCount;
                    frame->packetNode = NULL;
                    frame->lossPacketNode = NULL;
                    frame->retryCountNode = NULL;
                    frame->lossTimestampNode = NULL;
                    frame->next = NULL;
                    beforeFrameIndex = framePacket->frameIndex;
                    beforePacketTimestamp = nowPacketTimestamp;

                    beforePacketOrder = 0;
                    nowPacketOrder = framePacket->packageIndex;

                   packetProcess(framePacket);
                }

                //帧非顺序到达
                else if (framePacket->frameIndex - beforeFrameIndex > 1){

                    if (frame != NULL){
                        //TODO 加入检查链表
                        addFrame(frame);
                    }

                    for (int i = beforeFrameIndex + 1; i < framePacket->frameIndex; ++i) {
                        struct Frame *aFrame;
                        aFrame = (struct Frame *) malloc(sizeof(struct Frame));
                        aFrame->frameIndex = i;
                        struct lossPacket *lossPacket;
                        lossPacket = (struct lossPacket *) malloc(sizeof(struct lossPacket));
                        //包号 0 表示整帧丢失
                        lossPacket->id = 0;
                        lossPacket->next = NULL;
                        aFrame->lossPacketNode = lossPacket;
                        //TODO 加入不完整帧链表

                        beforePacketOrder = nowPacketOrder;
                    }

                    frame = (struct Frame *) malloc(sizeof (struct Frame));
                    frame->frameIndex = framePacket->frameIndex;
                    frame->frameLength = framePacket->frameLength;
                    frame->framePosition = framePacket->framePosition;
                    frame->lossPacketNode = NULL;
                    frame->packetNode = NULL;

                    frame->packetSum = packetCount;
                    beforeFrameIndex = framePacket->frameIndex;
                    beforePacketOrder = 0;
                    nowPacketOrder = framePacket->packageIndex;
                    packetProcess(framePacket);
                }

                //乱序到达的帧
                else if (framePacket->frameIndex < beforeFrameIndex){
                    pf("------ 乱序到达的帧 frameIndex=%d  packageIndex=%d",framePacket->frameIndex,framePacket->packageIndex);
                    fflush(stdout);
                    //TODO
                }
              //pf("------ time =%lld -----\n",(getTimeStampByUs() - s));
//                fflush(stdout);
                break;
            }
        }
    }
}

