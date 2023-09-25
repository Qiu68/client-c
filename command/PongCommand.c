#include <stdio.h>
#include "PongCommand.h"
#include "CommandType.h"

//
// Created by 24546 on 2023/9/19.
//

struct PongInfo pongInfo;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

void pongEcode(char data[], int sequence,  long long timestamp,
                   int processTimeMs,int receiveCount,
                   char delayChangeLevel){

    pongInfo.command = PONG;
    pongInfo.sequence = sequence;
    pongInfo.timestamp = timestamp;
    pongInfo.processTimeMs = processTimeMs;
    pongInfo.receiveCount = receiveCount;
    pongInfo.delayChangeLevel = delayChangeLevel;

    data[0] = pongInfo.command;
    char seqData[4];
    intToBytes(pongInfo.sequence,seqData);
    arrCopy(seqData,0,data,1,4);

    char timestampData[8];
    longToBytes(pongInfo.timestamp,timestampData);
    arrCopy(timestampData,0,data,5,8);

    char processTimeData[4];
    intToBytes(pongInfo.processTimeMs,processTimeData);
    arrCopy(processTimeData,0,data,13,4);

    char revCount[4];
    intToBytes(pongInfo.receiveCount,revCount);
    arrCopy(revCount,0,data,17,4);

    data[21] = pongInfo.delayChangeLevel;

}