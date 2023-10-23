//
// Created by 24546 on 2023/10/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "PacketGroup.h"
#include "../../log/log.h"

extern pthread_mutex_t packetGroupMutex;

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct PacketGroup *packetGroupList = NULL,*packetGroupTail = NULL;

int addGroupIndex(struct PacketGroup *packetGroup,struct PacketIds *id){

    if(packetGroup == NULL){
        return -1;
    }
    if(packetGroup->packages == NULL){
        packetGroup->packages = id;
        packetGroup->packages->next = NULL;
    }
    else{
        struct PacketIds *tmp = packetGroup->packages;
        while(tmp != NULL){
            tmp = tmp->next;
        }
        tmp = id;
    }

    return 1;
}

struct PacketGroup* getPacketGroup(int groupIndex){

    if(packetGroupList == NULL){
        return NULL;
    }

    struct PacketGroup *tmp = packetGroupList;
    while (NULL != tmp){
        if(tmp->groupIndex == groupIndex){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int addPacketGroup(struct PacketGroup *packetGroup) {
    //log_info("addPacketGroup 获取锁");
   // pthread_mutex_lock(&packetGroupMutex);
    //log_info("addPacketGroup 获取锁成功");
    if (NULL == packetGroupList) {
        packetGroupList = packetGroup;
        packetGroupList->next = NULL;
        packetGroupTail = packetGroup;
    } else {
        packetGroupTail->next = packetGroup;
        packetGroupTail = packetGroup;
        packetGroupTail->next = NULL;
    }
    //pthread_mutex_unlock(&packetGroupMutex);
   //log_info("addPacketGroup 释放锁成功");
    return 1;
}

int delPacketGroup(int groupIndex) {

    log_info("delPacketGroup 获取锁");
    pthread_mutex_lock(&packetGroupMutex);
    log_info("delPacketGroup 获取锁成功");
    if (packetGroupList == NULL) {
        pthread_mutex_unlock(&packetGroupList);
        log_info("delPacketGroup 释放锁成功");
        return -1;
    }

    //移除首节点
    if (packetGroupList->groupIndex == groupIndex) {
        //链表只有一个节点的情况
        if (packetGroupList->next == NULL) {
            packetGroupList = NULL;
        } else {
            packetGroupList = packetGroupList->next;
        }
        pthread_mutex_unlock(&packetGroupList);
        log_info("delPacketGroup 释放锁成功");
        return 1;
    }

    struct PacketGroup *prev = packetGroupList;
    struct PacketGroup *curr = prev->next;
    while(curr != NULL){
        if (curr->groupIndex == groupIndex){
            prev->next = curr->next;
            break;
        }
        prev = prev->next;
        curr = curr->next;
    }

    pthread_mutex_unlock(&packetGroupList);
    log_info("delPacketGroup 释放锁成功");
    return 1;

}