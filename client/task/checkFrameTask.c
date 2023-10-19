//
// Created by 24546 on 2023/10/8.
//

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>

#include "checkFramTask.h"
#include "../pojo/Frame.h"
#include "../../log/log.h"


atomic_int writeCount;
int writeCountTmp = 0;
extern pthread_t frameCheckTask;
extern pthread_mutex_t frameMutex;
extern pthread_mutex_t frameInCompleteMutex;
extern pthread_mutex_t frameCompleteMutex;
extern int udpRoutePort;

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);

int writer(long long offset, char *buf, int length);

int writeFileInit();

int addLossPacket(struct Frame *aFrame, int i);

int packetSumLength(struct Frame *frame);

struct Frame *frameCompleteList, *frameCompleteTail = NULL;     // frameCompleteList的节点  不能和 frameList的节点 使用相同的地址
struct Frame *frameInCompleteList = NULL;
struct Frame *frameInCompleteTail = NULL;

struct frameLength *frameLengthList, *frameLengthTail = NULL;
struct framePos *framePosList, *framePostTail = NULL;
struct Frame *frameList = NULL;
struct Frame *frameTail = NULL;

int frameFlagArr[500000];

int addFrame(struct Frame *frame) {
    pthread_mutex_lock(&frameMutex);
//    printf("------ addFrame 获取锁成功 ------\n");
    fflush(stdout);
    if (NULL == frameList) {
        frameList = frame;
        frameTail = frame;
    } else {
        frameTail->next = frame;
        frameTail = frame;
        frameTail->next = NULL;
    }
    pthread_mutex_unlock(&frameMutex);
//    printf("------ addFrame 释放锁成功 ------\n");
//    fflush(stdout);
    return 1;
}
int deleteFrameInComplete(int frameIndex);

struct Frame *deleteFrame(struct Frame *frame) {

    struct Frame *deleteAide;
    pthread_mutex_lock(&frameMutex);
    deleteAide = frameList;
    //移除首节点
    if (frameList->frameIndex == frame->frameIndex) {
        //链表只有一个节点的情况
        if (frameList->next == NULL) {
            frameList = NULL;
        } else {
            frameList = frameList->next;
            frame = NULL;
        }
    }
        //移除尾结点
    else if (deleteAide->next == NULL) {
        //遍历到ptr节点的上一个节点
        while (deleteAide->next->frameIndex != frame->frameIndex) {
            deleteAide = deleteAide->next;
        }
        //断开与ptr的连接
        deleteAide->next = NULL;
        free(frame);
        frame = NULL;
        //free(frame);
    }

        //中间节点
    else {

        //遍历到ptr节点的上一个节点
        while (deleteAide->next->frameIndex != frame->frameIndex) {
            deleteAide = deleteAide->next;
        }
        deleteAide->next = frame->next;
        frame = NULL;
    }
    pthread_mutex_unlock(&frameMutex);
    //packetSumLength(deleteAide);
    return deleteAide;
}

int addFrameComplete(struct Frame *frame) {
    printf("------ addFrameComplete frameIndex = %d ------\n",frame->frameIndex);
    fflush(stdout);
    struct Frame *tmp;
    //pthread_mutex_lock(&frameCompleteMutex);
    tmp = (struct Frame *) malloc(sizeof(struct Frame));
    memcpy(tmp, frame, sizeof(struct Frame));
    tmp->next = NULL;
    if (NULL == frameCompleteList) {
        frameCompleteList = tmp;
        frameCompleteTail = tmp;
    } else {
        frameCompleteTail->next = tmp;
        frameCompleteTail = tmp;
        frameCompleteTail->next = NULL;
    }
    //pthread_mutex_unlock(&frameCompleteMutex);
    return 1;
}


struct Frame *getInCompleteFrameByFrameIndex(int frameIndex) {
//    printf("------ getInCompleteFrameByFrameIndex 获取锁 ------\n");
//    fflush(stdout);
    struct Frame *findInCompleteFrame = NULL;
    pthread_mutex_lock(&frameInCompleteMutex);
//    printf("------ getInCompleteFrameByFrameIndex 获取锁成功 ------\n");
//    fflush(stdout);
    findInCompleteFrame = frameInCompleteList;
    while (findInCompleteFrame != NULL) {
        if (findInCompleteFrame->frameIndex == frameIndex) {
            pthread_mutex_unlock(&frameInCompleteMutex);
            return findInCompleteFrame;
        }
        findInCompleteFrame = findInCompleteFrame->next;
    }
    pthread_mutex_unlock(&frameInCompleteMutex);
//    printf("------ getInCompleteFrameByFrameIndex   释放锁------\n");
//    fflush(stdout);
    return NULL;
}

struct Frame *getCompleteFrameByFrameIndex(int frameIndex) {
    struct Frame *findCompleteFrame = NULL;
    pthread_mutex_lock(&frameCompleteMutex);
    findCompleteFrame = frameCompleteList;
    while (findCompleteFrame != NULL) {
        if (findCompleteFrame->frameIndex == frameIndex) {
            //pthread_mutex_unlock(&frameCompleteMutex);
            return findCompleteFrame;
        }
        findCompleteFrame = findCompleteFrame->next;
    }
    pthread_mutex_unlock(&frameCompleteMutex);
    return NULL;
}


int addFrameInComplete(struct Frame *frame) {


//    if(frameFlagArr[frame->frameIndex] = frame->frameIndex){
//        printf("------ frameIndex %d 已写入文件 ------\n");
//        fflush(stdout);
//        return -1;
//    }

//    printf("------ addFrameInComplete 获取锁 ------\n");
//    fflush(stdout);
    struct Frame *tmp;
//    pthread_mutex_lock(&frameInCompleteMutex);
//    printf("------ addFrameInComplete 获取锁成功 ------\n");
    fflush(stdout);
    tmp = (struct Frame *) malloc(sizeof(struct Frame));
    memcpy(tmp, frame, sizeof(struct Frame));
    tmp->next = NULL;
    if (NULL == frameInCompleteList) {
        frameInCompleteList = tmp;
        frameInCompleteTail = tmp;
        frameInCompleteList->next = NULL;
    } else {
        struct Frame *aide;
        aide = frameInCompleteList;
        while (aide->next != NULL) {
            //去除
            if (aide->next->frameIndex == tmp->frameIndex) {
                pthread_mutex_unlock(&frameInCompleteMutex);
//                printf("------ addFrameInComplete 释放锁 ------\n");
//                fflush(stdout);
                return -1;
            }
            aide = aide->next;
        }
        aide->next = tmp;
    }

//    if (frameInCompleteTail->next != NULL) {
//        printf("test frameInCompleteTail->next != null\n");
//        fflush(stdout);
//    }
    pthread_mutex_unlock(&frameInCompleteMutex);
//    printf("------ addFrameInComplete 释放锁 ------\n");
//    fflush(stdout);
    return 1;
}


int deleteFrameInComplete(int frameIndex) {
//    printf("------ deleteFrameInComplete  获取锁 ------ \n");
//    fflush(stdout);
    struct Frame *aide;
    pthread_mutex_lock(&frameInCompleteMutex);
//    printf("------ deleteFrameInComplete  获取锁成功 ------ \n");
//    fflush(stdout);
    if (frameInCompleteList == NULL) {
        return -1;
    }
    aide = frameInCompleteList;
    //移除首节点
    if (frameInCompleteList->frameIndex == frameIndex) {
        //链表只有一个节点的情况
        if (frameInCompleteList->next == NULL) {
            frameInCompleteList = NULL;
        } else {
            frameInCompleteList = frameInCompleteList->next;
        }
    }

        //移除尾结点
    else if (aide->next == NULL) {
        //遍历到ptr节点的上一个节点
        while (aide->next->frameIndex != frameIndex) {
            aide = aide->next;
        }
        //断开与ptr的连接
        aide->next = NULL;
    }

        //中间节点
    else {

        //遍历到ptr节点的上一个节点
        while (aide->next->frameIndex != frameIndex) {
            aide = aide->next;
        }
        struct Frame *tmp;
        tmp = aide;
        aide->next = tmp->next->next;
    }
    aide = NULL;
    pthread_mutex_unlock(&frameInCompleteMutex);
//    printf("------ deleteFrameInComplete  释放锁 ------ \n");
//    fflush(stdout);
    return 1;
}

int frameInCompleteListSize() {
    // printf("------ frameInCompleteListSize 获取锁 ------\n");
    fflush(stdout);
    struct Frame *tmp;
    // pthread_mutex_lock(&frameInCompleteMutex);
    tmp = frameInCompleteList;
    int count = 0;
    while (tmp != NULL) {
        printf("frameInComplete frameIndex = %d\n", tmp->frameIndex);
        ++count;
        tmp = tmp->next;
    }
    //printf("------ 2222 ------\n");
    // pthread_mutex_unlock(&frameInCompleteMutex);
    //printf("------ frameInCompleteListSize 释放锁 ------\n");
    fflush(stdout);
    return count;
}

int addFrameLength(struct frameLength *frameLength) {
    if (NULL == frameLengthList) {
        frameLengthList = frameLength;
        frameLengthTail = frameLength;
    } else {
        frameLengthTail->next = frameLength;
        frameLengthTail = frameLength;
        frameLengthTail->next = NULL;
    }
    return 1;
}

int addFramePos(struct framePos *framePos) {
    if (NULL == framePosList) {
        framePosList = framePos;
        framePosList = framePos;
    } else {
        framePostTail = framePos;
        framePostTail = framePos;
        framePostTail->next = NULL;
    }
    return 1;
}

int frameListSize() {
    // printf("------ 11111 ------\n");
    struct Frame *tmp;
    tmp = frameList;
    int count = 0;
    while (tmp != NULL) {
        ++count;
        tmp = tmp->next;
    }
    //printf("------ 2222 ------\n");
    return count;
}

int packetSumLength(struct Frame *frame) {
    /**
     * 注意这里直接用的头指针遍历，遍历完后，这个指针已经等于NULL了，需要使用一个辅助指针进行遍历
     */

    int packetSum = 0;
    if (frame == NULL) {
        return packetSum;
    }
    if (NULL == frame->packetNode) {
        return packetSum;
    }
    struct packetData *aide;
    aide = frame->packetNode;
    while (aide != NULL) {
        packetSum += aide->packageLength;
        aide = aide->next;
    }
    return packetSum;
}

int maxIndex = 0;

int getMaxIndex(struct Frame *frame) {
    struct packetData *tmp;
    tmp = frame->packetNode;
    maxIndex = 0;
    while (tmp != NULL) {
        if (tmp->packageIndex > maxIndex) {
            maxIndex = tmp->packageIndex;
        }
        tmp = tmp->next;
    }
    return maxIndex;
}

int writeFile() {
    struct Frame *tmp;
    tmp = frameCompleteList;
    while (tmp != NULL) {
        int pos = 0;
        char data[tmp->frameLength];
        data[0] = '\0';
        while (tmp->packetNode != NULL) {
            arrCopy(tmp->packetNode->data, 0, data, pos, tmp->packetNode->packageLength);
            pos = pos + tmp->packetNode->packageLength;
            tmp->packetNode = tmp->packetNode->next;
        }
        //printf("%c",data[0]);
        frameFlagArr[tmp->frameIndex] = tmp->frameIndex;

        log_info("------ 写入文件 frameIndex = %d  frameLength = %d  framePosition = %lld ------\n", tmp->frameIndex,
               tmp->frameLength, tmp->framePosition);
        writer(tmp->framePosition, data, tmp->frameLength);
        tmp = tmp->next;


    }
    return 1;
}

void *task(void *args) {

    while (1) {


//        int size = frameListSize();
//        printf("frameListSize = %d\n",size);

        if (frameList == NULL) {
            //pthread_mutex_unlock(&frameMutex);
            sleep(0.02);

        } else {

            struct Frame *tmp;
            //pthread_mutex_lock(&frameMutex);
            tmp = frameList;
            while (tmp != NULL) {

                int packetSum = packetSumLength(tmp);
                if (tmp->packetNode == NULL) {
                    printf("test\n");
                }

                //收到的包字节数 == 帧字节数
                if (tmp->frameLength == packetSum && tmp->packetNode!= NULL) {
                    struct Frame *inCompleteTmp;
                    inCompleteTmp = frameInCompleteList;
                    //如果不完整的帧链表存在该帧 则移除
                    log_info("------ checkTask ------\n");
                   // fflush(stdout);
                    struct Frame *tmp1 = getInCompleteFrameByFrameIndex(tmp->frameIndex);
                    if (tmp1 != NULL && tmp1->frameIndex == tmp->frameIndex) {
                        log_info("------ check deleteFrameInComplete 111 ------\n");
                        //fflush(stdout);
                        deleteFrameInComplete(tmp->frameIndex);
                        log_info("------ check deleteFrameInComplete 222 ------\n");
                    }

//                    printf("%d 检查完成\n", tmp->frameIndex);
//                    fflush(stdout);
                    log_info("------ check addFrameComplete %d-----\n",tmp->frameIndex);


                    addFrameComplete(tmp);

                    //printf("帧完整  删除前 %d\n", frameListSize());
                    tmp = deleteFrame(tmp);
                    //pthread_mutex_unlock(&frameMutex);
                    //printf("帧完整  删除后 %d\n", frameListSize());

                    if (frameListSize() == 0) {
                        tmp = NULL;
                    }
                    //tmp = frameList;


                } else {

                    // 判断尾包是否到达
                    int maxIndex = getMaxIndex(tmp);
                    if (tmp->packetSum != 0 && tmp->packetSum > maxIndex) {
                        for (int i = maxIndex + 1; i <= tmp->packetSum; ++i) {
                            addLossPacket(tmp, i);
                        }
                    }

                        //接收的帧不完整，加入frameInComplete链表
                        //printf("------ check addFrameComplete 111 ------\n");
                    log_info("帧不完整  添加前 frameInComplete size %d\n", frameInCompleteListSize());
//                        fflush(stdout);
                        int i = 0;
                        i = addFrameInComplete(tmp);
                    log_info("帧不完整  添加后 frameInComplete size %d result= %d\n", frameInCompleteListSize(), i);
//                    fflush(stdout);
//                    printf("------ check addFrameComplete 222 ------\n");
//                    fflush(stdout);
                    log_info("frameIndex %d join frameInComplete list\n", tmp->frameIndex);
                        //printf("帧不完整  删除前 size %d\n",frameListSize());
                        tmp = deleteFrame(tmp);
                        //printf("帧不完整  删除后 size %d\n",frameListSize());
                        //pthread_mutex_unlock(&frameMutex);
                        //fflush(stdout);
                }

//                if (frameList < -1){
//                    printf("Test");
//                }
                if (tmp == NULL) {
                    break;
                }
                tmp = tmp->next;

            }
            //pthread_mutex_unlock(&frameMutex);

            //printf("111111111111
            // 111111111111\n");
            writeFile();
            // 释放frameComplete链表的空间
            //pthread_mutex_unlock(&frameCompleteMutex);
            free(frameCompleteList);
            //free(frameCompleteTail);
            frameCompleteList = NULL;
            frameCompleteTail = NULL;
            sleep(0.005);
        }
    }
}


void initTask() {
    writeCount = ATOMIC_VAR_INIT(0);
    writeFileInit();
    pthread_create(&frameCheckTask, NULL, task, NULL);
}



