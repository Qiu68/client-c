

//
// Created by 24546 on 2023/10/11.
//

#include <stdio.h>
#include "NackReqCommand.h"
#include "CommandType.h"
#include "../client/task/checkFramTask.h"

extern struct lossPacketList *lossPacketHead;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

void nackReqEncode(char data[], int sequence, int receivePort,long long resId,int lossCount){

    struct nackReqCommand command;
    char seqChar[4];
    char portChar[4];
    char resChar[8];
    char lossCountChar[4];

    command.commandType = NACK_REQ;
    command.seq = sequence;
    command.resourceId = resId;

    data[0] = command.commandType;
    intToBytes(command.seq,seqChar);
    arrCopy(seqChar,0,data,1,4);

    intToBytes(lossCount,lossCountChar);
    arrCopy(lossCountChar,0,data,5,4);

    longToBytes(resId,resChar);
    arrCopy(resChar,0,data,9,8);

    intToBytes(receivePort,portChar);
    arrCopy(portChar,0,data,17,4);


    //将丢包信息写入字符数组
    struct lossPacketList *tmp;
    tmp = lossPacketHead;
    int count=0;
    while(NULL != tmp){
        arrCopy(tmp->data,0,data,21 + (count * 8),8);
        tmp = tmp->next;
        count++;
    }

}
