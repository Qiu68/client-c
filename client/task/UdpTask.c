//
// Created by 24546 on 2023/9/25.
//
#include "stdio.h"
#include "../../log/log.h"
#include <pthread.h>
#include <winsock2.h>
#include <math.h>


#pragma comment(lib, "ws2_32.lib")

#include <ws2tcpip.h>


#include "../../packet/PacketType.h"
#include "../../packet/PacketLength.h"
#include "../../packet/ClientInitRespPacket.h"
#include "../../packet/FramePacket.h"
#include "../pojo/Packet.h"
#include "../pojo/Frame.h"
#include "checkFramTask.h"
#include "../pojo/PacketGroup.h"

#define pf printf


struct clientInitRespPacket *clientInitRespDecode(char data[]);

struct FramePacket *FramePacketDecode(char data[], int length);

struct Frame *getCompleteFrameByFrameIndex(int frameIndex);

struct Frame *getInCompleteFrameByFrameIndex(int frameIndex);

int deleteFrameInComplete(int frameIndex);

int packetSumLength(struct Frame *frame);

struct PacketGroup *getPacketGroup(int groupIndex);

int addPacketGroup(struct PacketGroup *packetGroup);

int addGroupIndex(struct PacketGroup *packetGroup,struct PacketIds *id);

void intToString(char src[],int num);

int addPacketGroup(struct PacketGroup *packetGroup);

extern struct sockaddr_in addr;
extern int udpSockFd;
extern int frameFlagArr[];

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
struct Packet *head, *p, *packetPtr;

long long receiveTimeStamp = 0ll;
int beforeFrameIndex = 0;
int nowFrameIndex = 0;
long long nowPacketTimestamp = 0ll;
long long beforePacketTimestamp = 0ll;
int beforePacketOrder;
int nowPacketOrder;
struct Frame *frame;

struct PacketGroup *nowPacketGroup = NULL;
struct PacketGroup *beforePacketGroup = NULL;



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

int writer(long long offset, char *buf, int length);

int addPacketGroupDelay(struct PacketGroupDelay *packetGroup);


extern pthread_t udpTask;
extern int udpRoutePort;
extern int packageSize;
extern long long fileLength;

void udpListenerInit() {

    printf("------ udp 监听开启 ------ \n");

    nowPacketGroup = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
    nowPacketGroup->lastSentPacketTimestamp = 0;
    nowPacketGroup->lastArrivalPacketTimestamp = 0;
    nowPacketGroup->packageCount = 0;
    nowPacketGroup->firstArrivalPacketTimestamp = 0;
    nowPacketGroup->firstSentPacketTimestamp = 0;
    nowPacketGroup->next = NULL;

    pthread_create(&udpTask, NULL, udpListener, NULL);
}

int addPacket(struct framePacket *package) {
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
        //将新的分包升序插入包组链表
        ptr = frame->packetNode;
        if (data->packageIndex < ptr->packageIndex) {
            data->next = ptr;
            frame->packetNode = data;
            return 1;
        }

        struct packetData *tmp;
        tmp = ptr->next;
        while (NULL != tmp) {
            if (data->packageIndex < tmp->packageIndex) {
                // 插入指定位置
                ptr->next = data;
                data->next = tmp;
                return 1;
            }
            tmp = tmp->next;
            ptr = ptr->next;
        }

        ptr->next = data;
    }
    return 1;
}

int addPacketByFrame(struct Frame *aFrame, struct framePacket *package) {
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
        ptr = aFrame->packetNode;
        //检查是否重复
        while (ptr->next != NULL) {
            if (ptr->packageIndex == framePacket->packageIndex) {
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }


        //将新的分包升序插入包组链表
        ptr = aFrame->packetNode;
        if (data->packageIndex < ptr->packageIndex) {
            data->next = ptr;
            aFrame->packetNode = data;
            return 1;
        }

        struct packetData *tmp;
        tmp = ptr->next;
        while (NULL != tmp) {
            if (data->packageIndex < tmp->packageIndex) {
                // 插入指定位置
                ptr->next = data;
                data->next = tmp;
                return 1;
            }
            tmp = tmp->next;
            ptr = ptr->next;
        }

        ptr->next = data;
    }
    return 1;
}

int addLossPacket(struct Frame *aFrame, int i) {
    log_info("------ 丢失 frameIndex = %d   packageIndex = %d \n", aFrame->frameIndex, i);
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
            addLossPacket(frame, i);
        }
        beforePacketOrder = nowPacketOrder;
    }


    // 乱序的包处理
    if (nowPacketOrder < beforePacketOrder) {
        printf("------ 乱序到达的包 frameIndex = %d   packageIndex = %d\n", package->frameIndex, package->packageIndex);
        fflush(stdout);
        delLossPacket(frame, nowPacketOrder);
        addPacket(package);
    }

//    if (packetSumLength(frame) == frame->frameLength) {
//        printf("111111 add frame %d\n",frame->frameIndex);
//        fflush(stdout);
//        addFrame(frame);
//    }

}

long long  delOutdatedPackets(struct FramePacket *framePacket,long long time){
    //上锁
    pthread_mutex_lock(&packetCountMutex);
    if (head == NULL) {
        head = packetPtr;
        p = packetPtr;
    } else {
        p->next = packetPtr;
        p = p->next;
    }

    struct Packet *point, *aide;
    int count = 0;
    point = aide = head;
    //5秒清理掉没用的数据
    //printf("111111111111111\n");
    if (framePacket->sendTime - time >= 5000) {
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
        log_info("------ 清理%d个数据 ------\n", count);
        time = 0;
    }

    pthread_mutex_unlock(&packetCountMutex);
    return time;

}

void *udpListener(void *args) {
    char buf[1500];
    long long clearTime = 0l;
    long lastFrameLengthCount = 0l;
    long long prevPrintTime = getSystemTimestamp();




    while (udpListenerFlag) {

//
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
                packetPtr = (struct Packet *) malloc(sizeof(struct Packet));

                framePacket = FramePacketDecode(buf, length);
                packetPtr->sendTime = framePacket->sendTime;
                packetPtr->revTime = getSystemTimestamp();
                packetPtr->next = NULL;
                receiveTimeStamp = getSystemTimestamp();
                if (clearTime == 0) {
                    clearTime = framePacket->sendTime;
                }

                //char packetId[] = framePacket->frameIndex + framePacket->packageIndex +framePacket->sendTime;


//                long long start = getSystemTimestamp();
                int groupIndex = framePacket->sendTime / 5;
                struct PacketGroup *packetGroup = getPacketGroup(groupIndex);

                if (NULL != packetGroup) {
                    packetGroup->lastArrivalPacketTimestamp = max(packetPtr->revTime,packetGroup->lastArrivalPacketTimestamp);
                    packetGroup->lastSentPacketTimestamp = max(packetPtr->sendTime,packetGroup->lastSentPacketTimestamp);
                    packetGroup->packageCount = (packetGroup->packageCount) + 1;
                }

                if (nowPacketGroup->firstArrivalPacketTimestamp == 0) {
                    nowPacketGroup->firstSentPacketTimestamp = framePacket->sendTime;
                    nowPacketGroup->firstArrivalPacketTimestamp = packetPtr->revTime;
                    nowPacketGroup->lastArrivalPacketTimestamp = packetPtr->revTime;
                    //得到包组下标
                    nowPacketGroup->groupIndex = framePacket->sendTime / 5;
                    nowPacketGroup->lastSentPacketTimestamp = framePacket->sendTime;
                    nowPacketGroup->packageCount = (nowPacketGroup->packageCount) + 1;
                   // nowPacketGroup.packages.add(packetID);
                }


                //5ms内接收的包为一个包组
                if ((framePacket->sendTime / 5) == nowPacketGroup->groupIndex) {
//
                    nowPacketGroup->lastArrivalPacketTimestamp = max(nowPacketGroup->lastArrivalPacketTimestamp, packetPtr->revTime);

                    nowPacketGroup->lastSentPacketTimestamp = max(nowPacketGroup->lastSentPacketTimestamp, packetPtr->sendTime);
                    nowPacketGroup->packageCount = (nowPacketGroup->packageCount) + 1;
                    nowPacketGroup->next = NULL;
                    //nowPacketGroup.packages.add(packetID);
                } else {
                    if (beforePacketGroup != NULL) {
                        //packetGroupMap.put(nowPacketGroup.groupIndex, nowPacketGroup);
                        addPacketGroup(nowPacketGroup);
                    }

                    beforePacketGroup = nowPacketGroup;

                    //nowPacketGroup = new PacketGroup();
                    nowPacketGroup = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
                    nowPacketGroup->firstArrivalPacketTimestamp = packetPtr->revTime;
                    nowPacketGroup->firstSentPacketTimestamp = packetPtr->sendTime;

                    nowPacketGroup->lastArrivalPacketTimestamp = packetPtr->revTime;
                    nowPacketGroup->lastSentPacketTimestamp = packetPtr->sendTime;
                    nowPacketGroup->groupIndex = framePacket->sendTime / 5;
                    nowPacketGroup->packageCount++;
                    //nowPacketGroup->packages.add(packetID);

//                        System.out.println("----- nowPacketGroup.receiveTimestamp " +nowPacketGroup.receiveTimestamp);

                }
//
//                log_info("包组延迟计算耗时 = %lld",(getSystemTimestamp() - start));


//                long long start = getSystemTimestamp();
               clearTime =  delOutdatedPackets(framePacket,clearTime);
//                log_info("调用delOutdatedPackets耗时 %ld",(getSystemTimestamp() - start));

                nowFrameIndex = framePacket->frameIndex;
                if (framePacket->frameIndex == 1) {
                    nowPacketTimestamp = getSystemTimestamp();
                    beforePacketTimestamp = getSystemTimestamp();
                }

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
                if (getSystemTimestamp() - prevPrintTime > 1000){
                    log_info("------ frameIndex = %d   packageIndex = %d  frameLength = %-6d  packetCount = %d  ------\n",
                             framePacket->frameIndex, framePacket->packageIndex, framePacket->frameLength, packetCount);
                    prevPrintTime = getSystemTimestamp();
                }

               // fflush(stdout);

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
//                        printf("2222 add frame\n",frame->frameIndex);
//                        fflush(stdout);
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
//                        printf("3333 add frame %d\n",frame->frameIndex);
//                        fflush(stdout);
                        addFrame(frame);
                    }

                    for (int i = beforeFrameIndex + 1; i < framePacket->frameIndex; ++i) {
                        struct Frame *aFrame;
                        aFrame = (struct Frame *) malloc(sizeof(struct Frame));

                        aFrame->frameIndex = i;
                        aFrame->lossPacketNode = NULL;
                        aFrame->packetNode = NULL;
                        aFrame->next = NULL;
                        aFrame->retryCountNode = NULL;
                        aFrame->lossTimestampNode = NULL;
                        aFrame->framePosition = 0ll;
                        aFrame->frameLength = 0;
                        aFrame->packetSum = 0;
                        addLossPacket(aFrame, 0);
                        // 加入帧列表
                        //addFrameInComplete(aFrame);
                        addFrame(aFrame);
//                        printf("4444 add frame %d",aFrame->frameIndex);
//                        fflush(stdout);
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
                    log_info("------ 乱序到达的帧 frameIndex=%d  packageIndex=%d\n", framePacket->frameIndex,
                       framePacket->packageIndex);
                    fflush(stdout);
                    //TODO
                    if (frameFlagArr[framePacket->frameIndex] == framePacket->frameIndex) {
//                        printf("------ udp getInCompleteFrameByFrameIndex 111 ------\n");
//                        fflush(stdout);
                        struct Frame *s =  getInCompleteFrameByFrameIndex(framePacket->frameIndex);
//                        printf("------ udp getInCompleteFrameByFrameIndex 222 ------\n");
//                        fflush(stdout);
                        if (NULL != s) {
//                            printf("------ udp deleteFrameInComplete 333 ------\n");
//                            fflush(stdout);
                            deleteFrameInComplete(framePacket->frameIndex);
//                            printf("------ udp deleteFrameInComplete 444 ------\n");
//                            fflush(stdout);
                        }
                        //                            log.info(framePacket.getFrameIndex() + "已经写入文件");
                        continue;
                    }


                    //Map<Integer, Frame> frameInCompleteMap = checkFrameQueue.getFrameInCompleteMap();

//                    printf("------ udp getInCompleteFrameByFrameIndex 555 ------\n");
//                    fflush(stdout);
                    struct Frame *lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
//                    printf("------ udp getInCompleteFrameByFrameIndex 666 ------\n");
//                    fflush(stdout);
                    //Frame lossFrame = frameInCompleteMap.get(framePacket.getFrameIndex());


                    if (NULL == lossFrame) {
//                        printf("------ udp getInCompleteFrameByFrameIndex 777 ------\n");
//                        fflush(stdout);
                        lossFrame = getCompleteFrameByFrameIndex(framePacket->frameIndex);
//                        printf("------ udp getInCompleteFrameByFrameIndex 888 ------\n");
//                        fflush(stdout);
                        // lossFrame = checkFrameQueue.frameMap.get(framePacket.getFrameIndex());
                    }

                    if (NULL == lossFrame) {
//                        printf("------ udp getInCompleteFrameByFrameIndex  999------\n");
//                        fflush(stdout);
                        lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
//                        printf("------ udp getInCompleteFrameByFrameIndex   101010------\n");
//                        fflush(stdout);
                        //                        lossFrame = frameInCompleteMap.get(framePacket.getFrameIndex());
                    }

                    if (NULL == lossFrame) {
                        log_info("------ 乱序到达的帧为空 %d ------\n", framePacket->frameIndex);
                        //                            continue;
                        lossFrame = (struct Frame *) malloc(sizeof(struct Frame));
                        lossFrame->frameIndex = framePacket->frameIndex;
                        lossFrame->framePosition = framePacket->framePosition;
                        lossFrame->packetNode = NULL;
                        lossFrame->lossPacketNode = NULL;
                        lossFrame->lossTimestampNode = NULL;
                        lossFrame->retryCountNode = NULL;
                        lossFrame->next = NULL;
                    }

                    lossFrame->frameLength = framePacket->frameLength;


                    lossFrame->packetSum = packetCount;
                    //
                    addPacketByFrame(lossFrame, framePacket);
                    log_info("------ frameIndex = %d  frameLength = %d  packetLengthSum = %d  packetSum = %d ------ \n",
                       lossFrame->frameIndex, lossFrame->frameLength,
                       packetSumLength(lossFrame), lossFrame->packetSum);


                    if (NULL != lossFrame->lossPacketNode) {

                        delLossPacket(lossFrame, framePacket->packageIndex);
                        //                            log.info("帧" + framePacket.getFrameIndex() + " 分包" + framePacket.getPackageIndex() + "从frame.lossPacketList移除");
                    }
                    //每一个乱序的帧到达后，都放入检查列表
//                    printf("555 add frame %d",lossFrame->frameIndex);
//                    fflush(stdout);
                    addFrame(lossFrame);

//                    if (lossFrame->frameLength == packetSumLength(lossFrame)) {
//                        if (NULL != getInCompleteFrameByFrameIndex(lossFrame->frameIndex)) {
//                            deleteFrameInComplete(lossFrame->frameIndex);
//                        }
//                        lossFrame->lossPacketNode = NULL;
//
//                        //TODO 临时办法 后续需要优化
//                        int pos = 0;
//                        char data[lossFrame->frameLength];
//                        data[0] = '\0';
//                        while (lossFrame->packetNode != NULL) {
//                            arrCopy(lossFrame->packetNode->data, 0, data, pos, lossFrame->packetNode->packageLength);
//                            pos = pos + lossFrame->packetNode->packageLength;
//                            lossFrame->packetNode = lossFrame->packetNode->next;
//                        }
//                        //printf("%c",data[0]);
//                        frameFlagArr[lossFrame->frameIndex] = lossFrame->frameIndex;
//                        printf("------ udp 写入文件 frameIndex = %d  frameLength = %d  framePosition = %lld ------\n",
//                               lossFrame->frameIndex, lossFrame->frameLength, lossFrame->framePosition);
//                        fflush(stdout);
//                        writer(lossFrame->framePosition, data, lossFrame->frameLength);
//                        //lossFrame = NULL;
//                    }



                    //pf("------ time =%lld -----\n",(getTimeStampByUs() - s));
                    //                fflush(stdout);
                    //break;
                }

                //最后一帧
                if (framePacket->frameIndex == fileFrameCount) {
                    lastFrameLengthCount = lastFrameLengthCount + framePacket->dataLength;
                    if (frame->framePosition + lastFrameLengthCount == fileLength) {
                        addFrame(frame);
                    }

                }
            }

        }
//        printf("udp 222\n");
//        fflush(stdout);
    }
}

