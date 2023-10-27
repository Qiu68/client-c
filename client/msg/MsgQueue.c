//
// Created by root on 10/23/23.
//
#include <sys/msg.h>
#include <stdio.h>
#include "../../log/log.h"



int sendPacketGroupMsg(int msgID,void* data,int size){
    if (msgsnd(msgID, data, size,  IPC_NOWAIT) == -1) {
        log_error("sendPacketGroupMsg err");
        exit(-1);
    }
    return 1;
}

int  revPacketGroupMsg(int msgId,void* data){
    if (msgrcv(msgId, (void *) data, BUFSIZ, 0, IPC_NOWAIT) == -1) {
        log_error("revPacketGroupMsg err");
        return -1;
    }
    return 1;
}

int sendPacketSendTimeMsg(int msgID,void* data,int size){
    if (msgsnd(msgID, data, size, IPC_NOWAIT) == -1) {
        log_error("revPacketGroupMsg err");
        exit(-1);
    }
    return 1;
}

int  revPacketSendTimeMsg(int msgId,void* data){
    if (msgrcv(msgId, (void *) data, BUFSIZ, 0, IPC_NOWAIT) == -1) {
        log_error("msgrcv err");
        return -1;
    }
    return 1;
}

