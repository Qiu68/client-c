//
// Created by 24546 on 2023/9/25.
//
#include "stdio.h"
#include "../../log/log.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <stdlib.h>


#include "../../packet/PacketType.h"
#include "../../packet/PacketLength.h"
#include "../../packet/ClientInitRespPacket.h"
#include "../../packet/FramePacket.h"
#include "../pojo/Packet.h"
#include "../pojo/Frame.h"
#include "checkFramTask.h"
#include "../pojo/PacketSendTime.h"
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

int sendPacketGroupMsg(int msgID,void* data,int size);
int sendMsg(int msgID,void* data,int size);
int tcpRevMsg(int msgId,void* data);

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

extern int tcpThreadMsgId;

int udpListenerFlag = 1;
int revPackageCount = 0;
//统计一个两个ping之间接收到的包数
struct Packet *head, *p, packetPtr;

long long receiveTimeStamp = 0ll;
int beforeFrameIndex = 0;
int nowFrameIndex = 0;
long long nowPacketTimestamp = 0ll;
long long beforePacketTimestamp = 0ll;
int beforePacketOrder;
int nowPacketOrder;
struct Frame *frame;

int packetGroupMsgId = -1;
int packetSendTimeMsgId = -1;
int udpThreadMsgId = -1;
long long revTime = 0ll;

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

    revTime = getSystemTimestamp();

    packetGroupMsgId = msgget((key_t) 1000, 0664 | IPC_CREAT);//获取消息队列
    if (packetGroupMsgId == -1) {
        log_error("msgget err");
        exit(-1);
    }

    packetSendTimeMsgId = msgget((key_t) 1001, 0664 | IPC_CREAT);//获取消息队列
    if (packetSendTimeMsgId == -1) {
        log_error("msgget err");
        exit(-1);
    }

    
    struct msqid_ds msqidDs;
    if(msgctl(packetSendTimeMsgId,IPC_STAT,&msqidDs) == -1){
        perror("msgctl error 1");
        exit(-1);
    }
    //所属权限
    msqidDs.msg_perm.mode = 0777;
    //消息最大字节数
    msqidDs.msg_qbytes = 65535 * 10;

    if (-1 == msgctl(packetSendTimeMsgId, IPC_SET, &msqidDs)){
        perror("msgctl error 2");
        exit(1);
    }


     udpThreadMsgId = msgget((key_t) 1002, 0664 | IPC_CREAT);//获取消息队列
    if (udpThreadMsgId == -1) {
        log_error("msgget err");
        exit(-1);
    }

    nowPacketGroup = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
    nowPacketGroup->lastSentPacketTimestamp = 0;
    getTimeStampByUs();
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
        //log_info("udp 111 循环开始");
        while (ptr->next != NULL) {
            if (ptr->packageIndex == framePacket->packageIndex) {
                log_info("udp 111 循环退出");
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        //log_info("udp 111 循环退出");
        //将新的分包升序插入包组链表
        ptr = frame->packetNode;
        if (data->packageIndex < ptr->packageIndex) {
            data->next = ptr;
            frame->packetNode = data;
            return 1;
        }

        struct packetData *tmp;
        tmp = ptr->next;
        //log_info("udp 222 循环开始");
        while (NULL != tmp) {
            if (data->packageIndex < tmp->packageIndex) {
                // 插入指定位置
                ptr->next = data;
                data->next = tmp;
               // log_info("udp 222 循环退出");
                return 1;
            }
            tmp = tmp->next;
            ptr = ptr->next;
        }
        //log_info("udp 222 循环退出");

        ptr->next = data;
    }
    return 1;
}

int addPacketByFrame(struct Frame *aFrame, struct framePacket *package) {
   
    //struct packetData *data;
    struct FramePacket *framePacket;
    framePacket = package;
     log_info("addPacketByFrame frameIndex = %d packageDatLength = %d",aFrame->frameIndex,framePacket->dataLength);

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
        log_info("udp 333 循环开始");
        while (ptr->next != NULL) {
            if (ptr->packageIndex == framePacket->packageIndex) {
                log_info("udp 333 循环退出");
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        log_info("udp 333 循环退出");


        //将新的分包升序插入包组链表
        ptr = aFrame->packetNode;
        if (data->packageIndex < ptr->packageIndex) {
            data->next = ptr;
            aFrame->packetNode = data;
            return 1;
        }

        struct packetData *tmp;
        tmp = ptr->next;
        log_info("udp 444 循环开始");
        while (NULL != tmp) {
            if (data->packageIndex < tmp->packageIndex) {
                // 插入指定位置
                ptr->next = data;
                data->next = tmp;
                log_info("udp 444 循环退出");
                return 1;
            }
            tmp = tmp->next;
            ptr = ptr->next;
        }
        log_info("udp 444 循环退出");

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
        //log_info("udp 555 循环开始");
        while (ptr->next != NULL) {
            if (ptr->id == i) {
               // log_info("udp 555 循环退出");
                //已经存在
                return -1;
            }
            ptr = ptr->next;
        }
        //log_info("udp 555 循环退出");

        ptr->next = lossPacket;
        ptr = lossPacket;
        ptr->id = i;
        ptr->next = NULL;
    }
    return 1;
}

int delLossPacket(struct Frame *framePacket, int packageIndex) {
    if (framePacket->lossPacketNode == NULL){
        return -1;
    }
    struct Frame *aide;
    aide = framePacket;
    log_info("delLossPacket frameIndex = %d packageIndex = %d ",framePacket->frameIndex,packageIndex);
    //移除首节点
    if (aide->lossPacketNode->id == 0 || aide->lossPacketNode->id == packageIndex) {
        //链表只有一个节点的情况
        if (aide->lossPacketNode->next == NULL) {
            aide->lossPacketNode = NULL;
        } else {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }

        return 1;
    }

        //移除尾结点
    else if (aide->lossPacketNode->next == NULL) {
        //遍历到ptr节点的上一个节点
        log_info("udp 666 循环开始");
        while (aide->lossPacketNode->id != packageIndex) {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }
        log_info("udp 666 循环退出");
        //断开与ptr的连接
        aide->lossPacketNode->next = NULL;
    }

        //中间节点
    else {
        log_info("udp 777 循环开始");
        //遍历到ptr节点的上一个节点
        while (aide->lossPacketNode->next->id != packageIndex) {
            aide->lossPacketNode = aide->lossPacketNode->next;
        }
        log_info("udp 777 循环退出");
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

void *udpListener(void *args) {
    char buf[1500];
    long long clearTime = 0l;
    long lastFrameLengthCount = 0l;
    long long prevPrintTime = getSystemTimestamp();
    long long printTime = 0ll;
    struct PacketSendTime packetSendTimeArr[15];
    int bufCount = 0;

    while (udpListenerFlag) {
        //long long s = getTimeStampByUs();
        //log_info("------ udp 111 ------");
        socklen_t len = sizeof(addr);
        int length = recvfrom(udpSockFd, &buf, sizeof(buf), 0, (struct sockaddr *) &addr, &len);
       // log_info("------ udp 222 ------");
       //log_info("loop time = %lld",(getTimeStampByUs() - s));

        int type = buf[0];
        int rcvPacketCount = 0;
        
        
        switch (type) {
            //接收打洞端口
            case CLIENT_UDP_INIT_RESP: {
                printf("------ rev udp init msg ------\n");
                struct clientInitRespPacket *respPacket;
                respPacket = clientInitRespDecode(buf);
                udpRoutePort = respPacket->clientPort;
                log_info("------ udp route port %d =  ------",udpRoutePort);
                break;
            }

                //接收帧数据
            case FRAME: {
               
                struct FramePacket *framePacket;
                //packetPtr = (struct Packet *) malloc(sizeof(struct Packet));

                framePacket = FramePacketDecode(buf, length);
                packetPtr.sendTime = framePacket->sendTime;
                packetPtr.revTime = getSystemTimestamp() - revTime;
                //log_info("sendtime = %d rev time = %lld",packetPtr->sendTime,packetPtr->revTime);
                //packetPtrnext = NULL;
    
                struct PacketSendTime packetSendTime;
                 //packetSendTime = (struct PacketSendTime *) malloc(sizeof(struct PacketSendTime));
                 packetSendTime.sendTime = packetPtr.sendTime;
 
                 sendPacketSendTimeMsg(packetSendTimeMsgId,(void *)&packetSendTime,sizeof(struct PacketSendTime ));
                
                receiveTimeStamp = getSystemTimestamp();
                if (clearTime == 0) {
                    clearTime = framePacket->sendTime;
                }

                //char packetId[] = framePacket->frameIndex + framePacket->packageIndex +framePacket->sendTime;

                // if(framePacket->packageIndex == 1){
                    log_info("------ frameIndex = %d   packageIndex = %d  frameLength = %-6d  ------\n",
                         framePacket->frameIndex, framePacket->packageIndex, framePacket->frameLength);     
                // }

          
                int groupIndex = framePacket->sendTime / 5;

                struct PacketGroup *packetGroup = getPacketGroup(groupIndex);
      

                if (NULL != packetGroup) {
                    packetGroup->lastArrivalPacketTimestamp = packetPtr.revTime > packetGroup->lastArrivalPacketTimestamp
                                                              ? packetPtr.revTime:packetGroup->lastArrivalPacketTimestamp;
                    packetGroup->lastSentPacketTimestamp = packetPtr.sendTime > packetGroup->lastSentPacketTimestamp
                                                              ?packetPtr.sendTime : packetGroup->lastSentPacketTimestamp;
                    packetGroup->packageCount = (packetGroup->packageCount) + 1;
                }

                if (nowPacketGroup->firstArrivalPacketTimestamp == 0) {
                    nowPacketGroup->firstSentPacketTimestamp = framePacket->sendTime;
                    nowPacketGroup->firstArrivalPacketTimestamp = packetPtr.revTime;
                    nowPacketGroup->lastArrivalPacketTimestamp = packetPtr.revTime;
                    //得到包组下标
                    nowPacketGroup->groupIndex = framePacket->sendTime / 5;
                    nowPacketGroup->lastSentPacketTimestamp = framePacket->sendTime;
                    nowPacketGroup->packageCount = (nowPacketGroup->packageCount) + 1;
                   // nowPacketGroup.packages.add(packetID);
                }


                //5ms内接收的包为一个包组
                if ((framePacket->sendTime / 5) == nowPacketGroup->groupIndex) {
//
                    nowPacketGroup->lastArrivalPacketTimestamp = nowPacketGroup->lastArrivalPacketTimestamp > packetPtr.revTime
                                                                    ? nowPacketGroup->lastArrivalPacketTimestamp : packetPtr.revTime;

                    nowPacketGroup->lastSentPacketTimestamp = nowPacketGroup->lastSentPacketTimestamp > packetPtr.sendTime
                                                                    ? nowPacketGroup->lastSentPacketTimestamp : packetPtr.sendTime;
                    nowPacketGroup->packageCount = (nowPacketGroup->packageCount) + 1;
                    nowPacketGroup->next = NULL;
                    //nowPacketGroup.packages.add(packetID);
                } else {
                    if (beforePacketGroup != NULL) {
                        int result = sendPacketGroupMsg(packetGroupMsgId,(void *)nowPacketGroup,sizeof (struct PacketGroup));
                        if (result == -1){
                            exit(-1);
                        }
                    }

                    beforePacketGroup = nowPacketGroup;

                    //nowPacketGroup = new PacketGroup();
                    nowPacketGroup = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
                    nowPacketGroup->firstArrivalPacketTimestamp = packetPtr.revTime;
                    nowPacketGroup->firstSentPacketTimestamp = packetPtr.sendTime;

                    nowPacketGroup->lastArrivalPacketTimestamp = packetPtr.revTime;
                    nowPacketGroup->lastSentPacketTimestamp = packetPtr.sendTime;
                    nowPacketGroup->groupIndex = framePacket->sendTime / 5;
                    nowPacketGroup->packageCount++;
                    //nowPacketGroup->packages.add(packetID);

//                        System.out.println("----- nowPacketGroup.receiveTimestamp " +nowPacketGroup.receiveTimestamp);

                }
//


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
            

                // log_info("------ frameIndex = %d   packageIndex = %d  frameLength = %-6d  packetCount = %d  ------\n",
                //          framePacket->frameIndex, framePacket->packageIndex, framePacket->frameLength, packetCount);

               fflush(stdout);

                if (framePacket->frameIndex != beforeFrameIndex) {
                    nowPacketTimestamp = getSystemTimestamp();
                }

                if (nowFrameIndex - beforeFrameIndex > 500) {
                    //TODO 断网重传
                }

        
                //同一帧其他分包
                if (framePacket->frameIndex == beforeFrameIndex) {
                    packetProcess(framePacket);
                }

                    //帧顺序到达
                else if (framePacket->frameIndex - beforeFrameIndex == 1) {
                    if (frame != NULL) {
                        addFrame(frame);
                    
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
                        addFrame(frame);
                    }
                    //log_info("udp 2-222 循环开始");
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
                        beforePacketOrder = nowPacketOrder;
                    
                        addFrameInComplete(aFrame);
                    }
                    //log_info("udp 2-222 循环退出");
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

                    if (frameFlagArr[framePacket->frameIndex] == framePacket->frameIndex) {
               
                        struct Frame *s =  getInCompleteFrameByFrameIndex(framePacket->frameIndex);
                        if (NULL != s) {
                            deleteFrameInComplete(framePacket->frameIndex);
                        }
                                    
                        continue;
                    }


                  
                    struct Frame *lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
                


                    if (NULL == lossFrame) {
                    
                        lossFrame = getCompleteFrameByFrameIndex(framePacket->frameIndex);

                    }

                    if (NULL == lossFrame) {
//                      
                        lossFrame = getInCompleteFrameByFrameIndex(framePacket->frameIndex);
//                
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
                    //log_info("------ frameIndex = %d  frameLength = %d  packetLengthSum = %d  packetSum = %d ------ \n",
                    //    lossFrame->frameIndex, lossFrame->frameLength,
                    //    packetSumLength(lossFrame), lossFrame->packetSum);


                    if (NULL != lossFrame->lossPacketNode) {

                        delLossPacket(lossFrame, framePacket->packageIndex);
                    }
                 
                    addFrame(lossFrame);
            

                //跳出 case Frame
                break;
            }

               //log_info("udp 444");
                //最后一帧
                if (framePacket->frameIndex == fileFrameCount) {
                    lastFrameLengthCount = lastFrameLengthCount + framePacket->dataLength;
                    if (frame->framePosition + lastFrameLengthCount == fileLength) {
                        addFrame(frame);
                    }

                }
          
        }
          default:
                //log_info("rev udp other msg %d",type);
        //log_info("------ udp 222 ------");
    }
    
}

}
