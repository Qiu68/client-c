//
// Created by 24546 on 2023/10/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PacketGroup.h"

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
    if (NULL == packetGroupList) {
        packetGroupList = packetGroup;
        packetGroupTail = packetGroup;
    } else {
        packetGroupTail = packetGroup;
        packetGroupTail = packetGroup;
        packetGroupTail->next = NULL;
    }
    return 1;
}