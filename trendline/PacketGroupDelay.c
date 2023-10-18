//
// Created by 24546 on 2023/10/17.
//

#include <stdio.h>

#include "PacketGroupDelay.h"

struct PacketGroupDelay *groupDelayList = NULL, *tail;

struct PacketGroupDelay * getPacketGroupDelay(int groupIndex){

    if(NULL == groupDelayList){
        return NULL;
    }
    return NULL;
}
int addPacketGroupDelay(struct PacketGroupDelay *packetGroup) {
    if (NULL == groupDelayList) {
        groupDelayList = packetGroup;
        tail = packetGroup;
    } else {
        struct PacketGroupDelay *tmp = groupDelayList;
        //将新的分包升序插入包组链表
        if (packetGroup->sendTimeMs < tmp->sendTimeMs) {
            packetGroup->next = tmp;
            groupDelayList = packetGroup;
            return 1;
        }

        struct PacketGroupDelay *aide;
        aide = groupDelayList;
        tmp = groupDelayList->next;
        while (NULL != tmp) {
            if (packetGroup->sendTimeMs < tmp->sendTimeMs) {
                // 插入指定位置
                aide->next = packetGroup;
                packetGroup->next = tmp;
                return 1;
            }
            tmp = tmp->next;
            aide = aide->next;
        }

        aide->next = packetGroup;
    }
    return 1;
}