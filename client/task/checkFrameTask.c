//
// Created by 24546 on 2023/10/8.
//

#include <stdio.h>
#include <pthread.h>
#include <windows.h>
#include <string.h>
#include "checkFramTask.h"
#include "../pojo/Frame.h"

extern pthread_t frameCheckTask;
extern pthread_mutex_t frameMutex;

struct Frame *frameCompleteList, *frameCompleteTail = NULL;     // frameCompleteList的节点  不能和 frameList的节点 使用相同的地址
struct Frame *frameInCompleteList = NULL;
struct Frame *frameInCompleteTail = NULL;

struct frameLength *frameLengthList, *frameLengthTail = NULL;
struct framePos *framePosList, *framePostTail = NULL;
struct Frame *frameList = NULL;
struct Frame *frameTail = NULL;

int addFrame(struct Frame *frame) {
    pthread_mutex_lock(&frameMutex);
    if (NULL == frameList) {
        frameList = frame;
        frameTail = frame;
    } else {
        frameTail->next = frame;
        frameTail = frame;
        frameTail->next = NULL;
    }
    pthread_mutex_unlock(&frameMutex);
    return 1;
}

struct Frame *deleteFrame(struct Frame *frame) {
    pthread_mutex_lock(&frameMutex);
    struct Frame *aide;
    aide = frameList;
    //移除首节点
    if (frameList == frame) {
        //链表只有一个节点的情况
        if (frameList->next == NULL) {
            frameList = NULL;
        } else {
            frameList = frameList->next;
        }
    }

        //移除尾结点
    else if (frame->next == NULL) {
        //遍历到ptr节点的上一个节点
        while (aide->next != frame) {
            aide = aide->next;
        }
        //断开与ptr的连接
        aide->next = NULL;
    }

        //中间节点
    else {

        //遍历到ptr节点的上一个节点
        while (aide->next != frame) {
            aide = aide->next;
        }
        aide->next = frame->next;
    }
    pthread_mutex_unlock(&frameMutex);
    return aide;
}

int addFrameComplete(struct Frame *frame) {
    struct Frame *tmp;
    tmp = (struct Frame *) malloc(sizeof(struct Frame));
    memcpy(tmp, frame, sizeof(struct Frame));
    if (NULL == frameCompleteList) {
        frameCompleteList = tmp;
        frameCompleteTail = tmp;
    } else {
        frameCompleteTail->next = tmp;
        frameCompleteTail = tmp;
        frameCompleteTail->next = NULL;
    }
    return 1;
}


int addFrameInComplete(struct Frame *frame) {
    struct Frame *tmp;
    tmp = (struct Frame *) malloc(sizeof(struct Frame));
    memcpy(tmp, frame, sizeof(struct Frame));
    if (NULL == frameInCompleteList) {
        frameInCompleteList = tmp;
        frameInCompleteTail = tmp;
    } else {
        frameInCompleteTail->next = tmp;
        frameInCompleteTail = tmp;
        frameInCompleteTail->next = NULL;
    }
    return 1;
}

int deleteFrameInComplete(struct Frame *frame) {
    struct Frame *aide;
    aide = frameInCompleteList;
    //移除首节点
    if (frameInCompleteList == frame) {
        //链表只有一个节点的情况
        if (frameInCompleteList->next == NULL) {
            frameInCompleteList = NULL;
        } else {
            frameInCompleteList = frameInCompleteList->next;
        }
    }

        //移除尾结点
    else if (frame->next == NULL) {
        //遍历到ptr节点的上一个节点
        while (aide->next != frame) {
            aide = aide->next;
        }
        //断开与ptr的连接
        aide->next = NULL;
    }

        //中间节点
    else {

        //遍历到ptr节点的上一个节点
        while (aide->next != frame) {
            aide = aide->next;
        }
        aide->next = frame->next;
    }
    aide = NULL;
    return 1;
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
    int packetSum = 0;
    while (frame->packetNode != NULL) {
        packetSum += frame->packetNode->packageLength;
        frame->packetNode = frame->packetNode->next;
    }
    return packetSum;
}

int writeFile(){
    struct Frame *tmp;
    tmp = frameCompleteList;
    while (tmp != NULL){

    }
}

void *task(void *args) {

    while (1) {
        // 统计frameList >=100帧 开始检查 否则休眠等待
        pthread_mutex_lock(&frameMutex);
        int size = frameListSize();
        pthread_mutex_unlock(&frameMutex);
        if (size < 100) {
            Sleep(60);
        } else {
            struct Frame *tmp;
            tmp = frameList;
            while (tmp != NULL) {
                int packetSum = packetSumLength(tmp);
                //收到的包字节数 == 帧字节数
                if (tmp->frameLength == packetSum) {
                    struct Frame *inCompleteTmp;
                    inCompleteTmp = frameInCompleteList;
                    //如果不完整的帧链表存在该帧 则移除
                    while (inCompleteTmp != NULL) {
                        if (inCompleteTmp->frameIndex == tmp->frameIndex) {
                            //TODO 从frameInComplete链表移除
                            //deleteFrameInComplete(inCompleteTmp);
                            continue;
                        }
                        inCompleteTmp = inCompleteTmp->next;
                    }
                    addFrameComplete(tmp);
                    tmp = deleteFrame(tmp);
                } else {
                    // 接收的帧不完整，加入frameInComplete链表
                    addFrameInComplete(tmp);
                }
                tmp = tmp->next;
            }
            writeFile();
            //TODO 释放frameComplete链表的空间
        }
    }
}

void initTask() {
    pthread_create(&frameCheckTask, NULL, task, NULL);
}



