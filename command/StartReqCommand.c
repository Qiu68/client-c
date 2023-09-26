#include <stdio.h>
#include "StartReqCommand.h"
#include "CommandType.h"

//
// Created by 24546 on 2023/9/19.
//

struct StartReqInfo startReqInfo;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

void startReqEcode(char data[], int sequence, long long resId,
                   char protocol,int receivePort,int startFrameIndex){
    char seqChar[4];
    char resChar[8];
    char revPort[4];
    char startIndex[4];

    startReqInfo.command = START_REQ;
    startReqInfo.sequence = sequence;
    startReqInfo.resourceId = resId;
    startReqInfo.protocol = protocol;

    data[0] = startReqInfo.command;
    intToBytes(startReqInfo.sequence,seqChar);
    arrCopy(seqChar,0,data,1,4);

    longToBytes(startReqInfo.resourceId,resChar);
    arrCopy(resChar,0,data,5,8);

      data[13] = startReqInfo.protocol;

    startReqInfo.receivePort = receivePort;
    intToBytes(startReqInfo.receivePort,revPort);
    arrCopy(revPort,0,data,14,4);

    startReqInfo.startFrameIndex = startFrameIndex;
    intToBytes(startReqInfo.startFrameIndex,startIndex);
    arrCopy(startIndex,0,data,18,4);


}