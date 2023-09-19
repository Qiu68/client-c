//
// Created by 24546 on 2023/9/19.
//
#include "DescribeRespCommand.h"
#include "CommandType.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);
void shortToBytes(short num, char ch[]);
short bytesToShort(char ch[]);
long long bytesToLong(char ch[]);


void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct DescribeRespCommand describeRespInfo;

struct ClientInitRespInfo* desRespDecode(char data[]){

    describeRespInfo.command = DESCRIBE_RESP;

    char seq[4];
    arrCopy(data,1,seq,0,4);
    describeRespInfo.seq = bytesToInt(seq);

    describeRespInfo.success = data[5];

    char resId[8];
    arrCopy(data,6,resId,0,8);
    describeRespInfo.resourceId = bytesToLong(resId);

    char height[2];
    arrCopy(data,14,height,0,2);
    describeRespInfo.height = bytesToShort(height);

    char width[2];
    arrCopy(data,16,width,0,2);
    describeRespInfo.width = bytesToShort(width);

    char framerate[2];
    arrCopy(data,18,framerate,0,2);
    describeRespInfo.framerate = bytesToShort(framerate);


    char length[8];
    arrCopy(data,20,length,0,8);
    describeRespInfo.length = bytesToLong(length);

    char frameCount[4];
    arrCopy(data,28,frameCount,0,4);
    describeRespInfo.frameCount = bytesToInt(frameCount);

    return &describeRespInfo;
}