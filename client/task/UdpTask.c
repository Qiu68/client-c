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


struct clientInitRespPacket *clientInitRespDecode(char data[]);

struct FramePacket *FramePacketDecode(char data[], int length);

struct Frame* getCompleteFrameByFrameIndex(int frameIndex);
struct Frame* getInCompleteFrameByFrameIndex(int frameIndex);
int deleteFrameInComplete(int frameIndex);
int packetSumLength(struct Frame *frame);

extern struct sockaddr_in addr;
extern int udpSockFd;

extern pthread_mutex_t packetCountMutex;
extern pthread_mutex_t frameMutex;

extern struct Frame *frameList;
extern struct Frame *frameCompleteList;
extern struct Frame *frameInCompleteList;
extern struct frameLength *frameLengthList;
extern struct framePos *framePosList;

extern long long oldNowPingTimestamp;
extern long long oldPrevPingTimestamp;
extern int fileFrameCount;

int udpListenerFlag = 1;
int revPackageCount = 0;
//统计一个两个ping之间接收到的包数
struct Packet *head, *p, *q;

long long receiveTimeStamp = 0ll;
int beforeFrameIndex = 0;
int nowFrameIndex = 0;
long long nowPacketTimestamp = 0ll;
long long beforePacketTimestamp = 0ll;
int beforePacketOrder;
int nowPacketOrder;
struct Frame *frame;


void *udpListener(void *args);

long long getSystemTimestamp();

int addFrame(struct Frame *frame);

long long getTimeStampByUs();

int addFrameComplete(struct Frame *frame);

int addFrameInComplete(struct Frame *frame);

int addFrameLength(struct frameLength *frameLength);

int addFramePos(struct framePos *framePos);

int frameListSize();

int packetSumLength(struct Frame *frame);

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);


extern pthread_t udpTask;
extern int udpRoutePort;
extern int packageSize;
extern long long fileLength;

void udpListenerInit() {

    printf("------ udp 监听开启 ------ \n");

    pthread_create(&udpTask, NULL, udpListener, NULL);
}

int addPacket(struct framePacket *package) {
    //TODO
    //struct packetData *data;
    struct FramePacket *framePacket;
    framePacket = package;

    nowPacketOrder = framePacket->packageIndex;

    if (nowPacketOrder == 1) {
        frame->framePosition = framePacket->framePosition;
    }
    struct packetData *data;
    data = (struct packetData *) malloc(sizeof(struct packetData));
    //data->data = framePacket->data;
    arrCopy(framePacket->data, 0, data->data, 0, framePacket->dataLength);
    data->packageIndex = framePacket->packageIndex;
    data->packageLength = framePacket->dataLength;
    data->next = NULL;

    if (frame->packetNode == NULL) {
        frame->packetNode = data;
    } else {
        struct packetData *ptr;
        ptr = frame->packetNode;
        //检查是否重复 不重复移动到链表尾部
        while (ptr->next != NULL) {
            if (ptr->packageIndex == framePacket->packageIndex) {
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        ptr->next = data;
        frameList;
    }
    return 1;
}

int addPacketByFrame(struct Frame * aFrame,struct framePacket *package) {
    //struct packetData *data;
    struct FramePacket *framePacket;
    framePacket = package;

    nowPacketOrder = framePacket->packageIndex;

    if (nowPacketOrder == 1) {
        aFrame->framePosition = framePacket->framePosition;
    }
    struct packetData *data;
    data = (struct packetData *) malloc(sizeof(struct packetData));
    //data->data = framePacket->data;
    arrCopy(framePacket->data, 0, data->data, 0, framePacket->dataLength);
    data->packageIndex = framePacket->packageIndex;
    data->packageLength = framePacket->dataLength;
    data->next = NULL;

    if (aFrame->packetNode == NULL) {
        aFrame->packetNode = data;
    } else {
        struct packetData *ptr;
        ptr =aFrame->packetNode;
        //检查是否重复 不重复移动到链表尾部
        while (ptr->next != NULL) {
            if (ptr->packageIndex == framePacket->packageIndex) {
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        ptr->next = data;
        ptr = data;
        ptr->next = NULL;
    }
    return 1;
}

int addLossPacket(struct Frame *aFrame,int i) {
    printf("------ 丢失 frameIndex = %d   packageIndex = %d \n",aFrame->frameIndex,i);
    fflush(stdout);
    struct lossPacket *lossPacket;

    lossPacket = (struct lossPacket *) malloc(sizeof(struct lossPacket));
    lossPacket->id = i;
    lossPacket->retryCount = 0;
    lossPacket->lossTimestamp = getSystemTimestamp();
    lossPacket->next = NULL;

    if (aFrame->lossPacketNode == NULL) {
        aFrame->lossPacketNode = lossPacket;
    } else {
        struct lossPacket *ptr;
        ptr = aFrame->lossPacketNode;
        //检查是否重复 不重复移动到链表尾部
        while (ptr->next != NULL) {
            if (ptr->id == i) {
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }

        ptr->next = lossPacket;
        ptr = lossPacket;
        ptr->id = i;
        ptr->next = NULL;
    }
    return 1;
}

int delLossPacket(struct Frame *framePacket, int packageIndex) {
    struct Frame *aide;
    aide = framePacket;
    //移除首节点
    if (aide->lossPacketNode->id == 0 || aide->lossPacketNode->id == packageIndex) {
    //链表只有一个节点的情况
        if (aide->lossPacketNode->next == NULL) {
            aide->lossPacketNode = NULL;
        } else {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }
    }

    //移除尾结点
    else if (aide->lossPacketNode->next == NULL) {
    //遍历到ptr节点的上一个节点
        while (aide->lossPacketNode->id != packageIndex) {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }
    //断开与ptr的连接
        aide->lossPacketNode->next = NULL;
    }

    //中间节点
    else {

    //遍历到ptr节点的上一个节点
        while (aide->lossPacketNode->next->id != packageIndex) {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }
        struct Frame *tmp;
        tmp = aide;
        aide->lossPacketNode->next = tmp->lossPacketNode->next->next;
    }
    aide = NULL;
    return 1;
}

int packetProcess(struct FramePacket *package) {

    nowPacketOrder = package->packageIndex;

    if (nowPacketOrder == 1) {
        frame->framePosition = package->framePosition;
    }
    //包顺序到达
    if (nowPacketOrder - beforePacketOrder == 1) {
        addPacket(package);
        beforePacketOrder = nowPacketOrder;
    }

    if (nowPacketOrder - beforePacketOrder > 1) {
        addPacket(package);
        for (int i = beforePacketOrder + 1; i < nowPacketOrder; ++i) {
            addLossPacket(frame,i);
        }
        beforePacketOrder = nowPacketOrder;
    }


    // 乱序的包处理
    if (nowPacketOrder < beforePacketOrder) {
        printf("------ 乱序到达的包 frameIndex = %d   packageIndex = %d\n",package->frameIndex,package->packageIndex);
        fflush(stdout);
        delLossPacket(frame,nowPacketOrder);
        addPacket(package);
    }

    if (packetSumLength(frame) == frame->frameLength){
        addFrame(frame);
    }

}

void *udpListener(void *args) {
    char buf[1500];
    long long clearTime = 0l;
    long lastFrameLengthCount = 0l;

    while (udpListenerFlag) {

        //printf("udp 111");
        socklen_t len = sizeof(addr);
        int length = recvfrom(udpSockFd, &buf, sizeof(buf), 0, (struct sockaddr *) &addr, &len);
        //printf("udp 222");

        int type = buf[0];

        switch (type) {
            //接收打洞端口
            case CLIENT_UDP_INIT_RESP: {
                printf("------ rev udp init msg ------\n");
                struct clientInitRespPacket *respPacket;
                respPacket = clientInitRespDecode(buf);
                udpRoutePort = respPacket->clientPort;
                break;
            }

                //接收帧数据
            case FRAME: {
                long long s = getTimeStampByUs();
                //printf("------ rev udp frame msg ------\n");
                struct FramePacket *framePacket;
                q = (struct Packet *) malloc(sizeof(struct Packet));

                framePacket = FramePacketDecode(buf, length);
                q->sendTime = framePacket->sendTime;
                q->revTime = getSystemTimestamp();
                q->next = NULL;
                receiveTimeStamp = getSystemTimestamp();
                if (clearTime == 0) {
                    clearTime = framePacket->sendTime;
                }

                //上锁
                pthread_mutex_lock(&packetCountMutex);
                if (head == NULL) {
                    head = q;
                    p = q;
                } else {
                    p->next = q;
                    p = p->next;
                }

                struct Packet *point, *aide;
                int count = 0;
                point = aide = head;
                //5秒清理掉没用的数据
                //printf("111111111111111\n");
                if (framePacket->sendTime - clearTime >= 5000) {
                    while (point != NULL) {
                        if (point->sendTime < oldPrevPingTimestamp) {

                            //移除首节点
                            if (head == point) {
                                //链表只有一个节点的情况
                                if (head->next == NULL) {
                                    head = NULL;
                                } else {
                                    head = head->next;
                                }
                            }

                                //移除尾结点
                            else if (point->next == NULL) {
                                //遍历到ptr节点的上一个节点
                                while (aide->next != point) {
                                    aide = aide->next;
                                }
                                //断开与ptr的连接
                                aide->next = NULL;
                            }

                                //中间节点
                            else {

                                //遍历到ptr节点的上一个节点
                                while (aide->next != point) {
                                    aide = aide->next;
                                }
                                aide->next = point->next;
                            }
                            aide = NULL;
                            count++;
                        }
                        point = point->next;
                    }
                    printf("------ 清理%d个数据 ------\n", count);
                    clearTime = 0;
                }

                pthread_mutex_unlock(&packetCountMutex);


                nowFrameIndex = framePacket->packageIndex;

                if (framePacket->packageIndex == 1) {
                    // 偏移量 帧长存起来
                    struct framePos *pos;
                    pos = (struct framePos *) malloc(sizeof(struct framePos));
                    pos->framePos = framePacket->framePosition;
                    pos->frameIndex = framePacket->frameIndex;
                    pos->next = NULL;
                    addFramePos(pos);

                    struct frameLength *aLength;
                    aLength = (struct frameLength *) malloc(sizeof(struct frameLength));
                    aLength->frameIndex = framePacket->frameIndex;
                    aLength->frameLength = framePacket->frameLength;
                    addFrameLength(aLength);
                }

                int packetCount = (framePacket->frameLength + 8) % (packageSize - 25)
                                  == 0 ? (framePacket->frameLength + 8) / (packageSize - 25)
                                       : ((framePacket->frameLength + 8) / (packageSize - 25)) + 1;
                pf("------ frameIndex = %d   packageIndex = %d  frameLength = %-6d  packetCount = %d  ------\n",
                   framePacket->frameIndex, framePacket->packageIndex, framePacket->frameLength, packetCount);
                fflush(stdout);

                if (framePacket->frameIndex != beforeFrameIndex) {
                    nowPacketTimestamp = getSystemTimestamp();
                }

                if (nowFrameIndex - beforeFrameIndex > 500) {
                    //TODO 断网重传
                }

//                if (framePacket->frameIndex == 4){
//                    printf("test\n");
//                }
                //同一帧其他分包
                if (framePacket->frameIndex == beforeFrameIndex) {
                    packetProcess(framePacket);
                }

                    //帧顺序到达
                else if (framePacket->frameIndex - beforeFrameIndex == 1) {
                    if (frame != NULL) {
                        // 加入检查队列
                        //pthread_mutex_lock(&frameMutex);
                        addFrame(frame);
                        //int count = frameListSize();
                        //printf("frame size = %d\n",count);
                        //fflush(stdout);
                        //pthread_mutex_unlock(&frameMutex);
                    }
                    frame = (struct Frame *) malloc(sizeof(struct Frame));
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
                else if (framePacket->frameIndex - beforeFrameIndex > 1) {

                    if (frame != NULL) {
                        // 加入检查链表
                        addFrame(frame);
                    }

                    for (int i = beforeFrameIndex + 1; i < framePacket->frameIndex; ++i) {
                        struct Frame *aFrame;
                        aFrame = (struct Frame *) malloc(sizeof(struct Frame));

                        aFrame->frameIndex = i;
                        aFrame->lossPacketNode  = NULL;
                        aFrame->packetNode = NULL;
                        aFrame->next = NULL;
                        aFrame->framePosition = 0ll;
                        aFrame->frameLength = 0;
                        addLossPacket(aFrame,0);
                        // 加入不完整帧链表
                        addFrameInComplete(aFrame);
                        beforePacketOrder = nowPacketOrder;
                    }

                    frame = (struct Frame *) malloc(sizeof(struct Frame));
                    frame->frameIndex = framePacket->frameIndex;
                    frame->frameLength = framePacket->frameLength;
                    frame->framePosition = framePacket->framePosition;
                    frame->lossPacketNode = NULL;
                    frame->packetNode = NULL;
                    frame->lossPacketNode = NULL;
                    frame->retryCountNode = NULL;
                    frame->lossTimestampNode = NULL;
                    frame->next = NULL;
                    frame->packetSum = packetCount;
                    beforeFrameIndex = framePacket->frameIndex;
                    beforePacketOrder = 0;
                    nowPacketOrder = framePacket->packageIndex;
                    packetProcess(framePacket);

                }

                    //乱序到达的帧
                else if (framePacket->frameIndex < beforeFrameIndex) {
                    pf("------ 乱序到达的帧 frameIndex=%d  packageIndex=%d\n", framePacket->frameIndex,framePacket->packageIndex);
                    fflush(stdout);
                    //TODO
                    if (NULL != getCompleteFrameByFrameIndex(framePacket->frameIndex)) {
                        if (NULL != getInCompleteFrameByFrameIndex(framePacket->frameIndex)) {
                            deleteFrameInComplete(framePacket->frameIndex);
                        }
//                            log.info(framePacket.getFrameIndex() + "已经写入文件");
                        continue;
                    }


                    //Map<Integer, Frame> frameInCompleteMap = checkFrameQueue.getFrameInCompleteMap();

                    struct Frame *lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
                    //Frame lossFrame = frameInCompleteMap.get(framePacket.getFrameIndex());


                    if (NULL == lossFrame) {
                        lossFrame = getCompleteFrameByFrameIndex(framePacket->frameIndex);
                       // lossFrame = checkFrameQueue.frameMap.get(framePacket.getFrameIndex());
                    }

                    if (NULL == lossFrame) {
                        lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
//                        lossFrame = frameInCompleteMap.get(framePacket.getFrameIndex());
                    }

                    if (NULL == lossFrame) {
                            pf("------ 乱序到达的帧为空 %d ------\n",framePacket->frameIndex);
//                            continue;
                        lossFrame  = (struct Frame*) malloc(sizeof (struct Frame));
                        lossFrame->frameIndex = framePacket->frameIndex;
                        lossFrame->framePosition = framePacket->framePosition;
                        lossFrame->packetNode = NULL;
                        lossFrame->lossPacketNode = NULL;
                        lossFrame->lossTimestampNode = NULL;
                        lossFrame->retryCountNode = NULL;
                    }

                    lossFrame->frameLength = framePacket->frameLength;


                    lossFrame->packetSum = packetCount;
//
                    addPacketByFrame(lossFrame,framePacket);
                    pf("------ frameIndex = %d  frameLength = %d  packetLengthSum = %d  packetSum ------ \n",lossFrame->frameIndex,lossFrame->frameLength,
                       packetSumLength(lossFrame),lossFrame->packetSum);


                    if (lossFrame->frameLength == packetSumLength(lossFrame)){
                        deleteFrameInComplete(lossFrame->frameIndex);
                        lossFrame->lossPacketNode = NULL;
                    }

                    if (NULL != lossFrame->lossPacketNode) {

                        delLossPacket(lossFrame,framePacket->packageIndex);
//                            log.info("帧" + framePacket.getFrameIndex() + " 分包" + framePacket.getPackageIndex() + "从frame.lossPacketList移除");
                    }
                    //每一个乱序的帧到达后，都放入检查列表
                    //checkFrameQueue.addFrame(lossFrame.getFrameIndex(), lossFrame);
                    addFrame(lossFrame);

//                        if (lossFrame.getFrameLength() == lossFrame.getPacketsLength()) {
////                        log.info(lossFrame.getFrameIndex() + "帧从frameIncompleteMap移除" + " frameLength" + lossFrame.getFrameLength() + " packetLengthSum" + lossFrame.getPacketsLength());
//                            checkFrameQueue.addFrame(lossFrame.getFrameIndex(), lossFrame);
//                            frameInCompleteMap.remove(lossFrame.getFrameIndex());
//                        }
                }

                //最后一帧
                if (framePacket->frameIndex == fileFrameCount) {
//                        System.out.println(checkFrameQueue.completeFrame);
                    lastFrameLengthCount = lastFrameLengthCount + framePacket->dataLength;
                    if (frame->framePosition + lastFrameLengthCount == fileLength) {
                        addFrame(frame);
                        frameInCompleteList;
                    }

                }

                //pf("------ time =%lld -----\n",(getTimeStampByUs() - s));
//                fflush(stdout);
                break;
            }
        }
    }
}

