//
// Created by 24546 on 2023/9/19.
//
#include "PingCommand.h"
#include "CommandType.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);
void shortToBytes(short num, char ch[]);
short bytesToShort(char ch[]);
long long bytesToLong(char ch[]);


void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct PingInfo pingInfo;

struct PingInfo* pingDecode(char data[]){

    pingInfo.command = PING;

    char seq[4];
    arrCopy(data,1,seq,0,4);
    pingInfo.sequence = bytesToInt(seq);

    char timestamp[8];
    arrCopy(data,5,timestamp,0,8);
    pingInfo.timestamp = bytesToLong(timestamp);

    char rtt[4];
    arrCopy(data,13,timestamp,0,4);
    pingInfo.rtt = bytesToInt(rtt);
    return &pingInfo;
}