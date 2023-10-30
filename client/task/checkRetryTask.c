//
// Created by 24546 on 2023/10/10.
//

#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>

#include "../../client/pojo/Frame.h"
#include "../../log/log.h"
#include "checkFramTask.h"



extern pthread_t retryTask;
extern int maxInCompleteFrameNum;
extern int minInCompleteFrameNum;
extern atomic_int writeCount;
extern struct Frame *frameInCompleteList;
extern int RTT;
extern int udpRoutePort;
extern long long resourceId;
extern int frameFlagArr[500000];

void *retry(void *args);

void startPlay(int frameIndex);

int sendPause(int sequence);

struct Frame *getCompleteFrameByFrameIndex(int frameIndex);

int deleteFrameInComplete(int frameIndex);

long long getSystemTimestamp();

int addFrame(struct Frame *frame);

void intToBytes(int num, char ch[]);

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);

int frameInCompleteListSize();

void nackReqEncode(char data[], int sequence, int receivePort, long long resId, int lossCount);

int sendNackReq(char *buf,int length);

int addLossPacket(struct Frame *aFrame,int i);

int pauseFlag = 1;

struct lossPacketList *lossPacketHead, *lossPacketTail = NULL;
struct lossPacketList *castTmp;
int addLossPacketAndCast(int frameIndex, int packageIndex) {

    char frameIndexArr[4];
    char packageIndexArr[4];
    if (NULL == lossPacketHead) {
        lossPacketHead = (struct lossPacket *) malloc(sizeof(struct lossPacket));
        lossPacketTail = lossPacketHead;

        intToBytes(frameIndex, frameIndexArr);
        arrCopy(frameIndexArr, 0, lossPacketHead->data, 0, 4);
        intToBytes(packageIndex, packageIndexArr);
        arrCopy(packageIndexArr, 0, lossPacketHead->data, 4, 4);
        lossPacketHead->next = NULL;

    } else {
        castTmp = (struct lossPacketList *) malloc(sizeof(struct lossPacketList));

        intToBytes(frameIndex, frameIndexArr);
        arrCopy(frameIndexArr, 0, lossPacketHead->data, 0, 4);
        intToBytes(packageIndex, packageIndexArr);
        arrCopy(packageIndexArr, 0, lossPacketHead->data, 4, 4);

        lossPacketTail->next = castTmp;
        lossPacketTail = castTmp;
        lossPacketTail->next = NULL;
    }
    return 1;
}


int checkRetryInit() {
    pthread_create(&retryTask, NULL, retry, NULL);
}

void stop() {
    printf("------ 不完整帧超过 %d 发送暂停 ------\n", maxInCompleteFrameNum);
    fflush(stdout);
    //TODO 发送暂停指令
    sendPause(1);
}

void play() {
    printf("------ 不完整帧小于 %d 重新推流 ------\n", minInCompleteFrameNum);
    startPlay(atomic_load(&writeCount));
}

void *retry(void *args) {

    /**
     * 先遍历 frameInComplete链表，然后链表中的元素编码，最后发送重传请求
     */

    while (1) {

        int size = frameInCompleteListSize();

//        printf("frameInComplete size = %d\n",size);
//        fflush(stdout);
        if (size == 0) {
            sleep(0.5);
            continue;
        }
        if (pauseFlag) {
            if (size >= maxInCompleteFrameNum) {
                stop();
                pauseFlag = 0;
            }
        }

        if (!pauseFlag) {
            if (size <= minInCompleteFrameNum) {
                play();
                pauseFlag = 1;
            }
        }

        log_info("frameInCompleteSize = %d",size);
        struct Frame *frameTmp;
        frameTmp = frameInCompleteList;

        while (NULL != frameTmp) {


             //如果完整帧链表存在该帧，就从不完整链表删除该帧
//            if (NULL != getCompleteFrameByFrameIndex(frameTmp->frameIndex)) {
//                deleteFrameInComplete(frameTmp->frameIndex);
//            }

            struct lossPacket *lossPacket;
            lossPacket = frameTmp->lossPacketNode;

            if (NULL == lossPacket) {
                    // 将该帧加入检查链表
                    printf("777 add frame %d\n", frameTmp->frameIndex);
                    fflush(stdout);
                    addFrame(frameTmp);
                    frameTmp = frameTmp->next;
                    continue;
            }

            while (NULL != lossPacket) {
                long long nowTimestamp = getSystemTimestamp();

                if (lossPacket->retryCount == 0){
                    addLossPacketAndCast(frameTmp->frameIndex, lossPacket->id);
                    lossPacket->retryCount = (lossPacket->retryCount) + 1;
                    lossPacket->lossTimestamp = getSystemTimestamp();
                    log_info("------ 111 重传 frameIndex = %d   packageIndex = %d ------\n",frameTmp->frameIndex,lossPacket->id);
                }
                //符合重传规则 加入重传链表
               else if (nowTimestamp - lossPacket->lossTimestamp > ((lossPacket->retryCount * 100))) {

                    addLossPacketAndCast(frameTmp->frameIndex, lossPacket->id);
                    log_info("------ 222 重传 frameIndex = %d   packageIndex = %d ------\n",frameTmp->frameIndex,lossPacket->id);
                    //重传次数不超过6次 就加1
                    if (lossPacket->retryCount < 6) {
                        lossPacket->retryCount = (lossPacket->retryCount) + 1;
                        //更新时间戳
                    }
                    lossPacket->lossTimestamp = getSystemTimestamp();

                }
//               else{
//                    printf("------  不符合重传要求 frameIndex = %d   packaegeIndex = %d ------ \n",frameTmp->frameIndex,lossPacket->id);
//               }

                lossPacket = lossPacket->next;

            }
            frameTmp = frameTmp->next;
        }


        struct lossPacketList *tmp;
        tmp = lossPacketHead;
        int count = 0;
        while(NULL != tmp){
            ++count;
            tmp = tmp->next;
        }
        printf("lossPacket count = %d \n",count);
        fflush(stdout);
        if (count != 0) {
            //TODO 编码
            char data[21 + count * 8];
            nackReqEncode(data, 1, udpRoutePort, resourceId, count);
            //TODO 发送nack指令
            sendNackReq(data, sizeof(data));
            printf("------ send nack command ------\n");
            printf("----- lossPacketCount = %d ------ \n", count);
            fflush(stdout);
            //TODO 释放lossPacketHead内存  lossPacketHead = NULL
            free(lossPacketHead);
            lossPacketHead = NULL;
            lossPacketTail = NULL;
        }
        fflush(stdout);
        //TODO 释放lossPacketHead内存  lossPacketHead = NULL
        free(lossPacketHead);
        lossPacketHead = NULL;
        lossPacketTail = NULL;

        printf("------ sleep 1 m ------ \n");
        fflush(stdout);
        sleep(0.1);
    }

    printf("------- 异常退出 ------ \n");
    fflush(stdout);
}