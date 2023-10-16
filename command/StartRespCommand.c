//
// Created by 24546 on 2023/9/19.
//
#include "StartRespCommand.h"
#include "CommandType.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);
void shortToBytes(short num, char ch[]);
short bytesToShort(char ch[]);
long long bytesToLong(char ch[]);


void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct StartRespInfo startRespInfo;

struct StartRespInfo* startRespDecode(char data[]){

    startRespInfo.command = DESCRIBE_RESP;

    char seq[4];
    arrCopy(data,1,seq,0,4);
    startRespInfo.sequence = bytesToInt(seq);

    startRespInfo.success = data[5];

    char time[8];
    arrCopy(data,6,time,0,8);
    startRespInfo.time = bytesToLong(time);


    return &startRespInfo;
}