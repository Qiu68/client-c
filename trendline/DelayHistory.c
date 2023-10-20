//
// Created by 24546 on 2023/10/20.
//
#include <stdio.h>

#include "DelayHistory.h"

struct delayHistory *delayHistoryHead = NULL;
struct delayHistory *delayHistoryTail = NULL;


int addDelayData(struct delayHistory *delayHistory){
    if (NULL == delayHistoryHead) {
        delayHistoryHead = delayHistory;
        delayHistoryTail = delayHistory;
        delayHistoryHead->next = NULL;
    } else {
        delayHistoryTail->next = delayHistory;
        delayHistoryTail = delayHistory;
        delayHistoryTail->next = NULL;
    }
}

int count = 0;
int getDelayHistoryCount(){
    count = 0;
    struct delayHistory *tmp;
    tmp = delayHistoryHead;
    while (tmp != NULL){
        ++count;
        tmp = tmp->next;
    }
    return count;
}

int delFirstNode(){
    delayHistoryHead = delayHistoryHead->next;
}