//
// Created by 24546 on 2023/9/22.
//
#include <stdlib.h>
#include "FramePacket.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);
void intToBytes(int num,char ch[]);
long long bytesToLong(char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

void FramePacketDecode(struct FramePacket info,char data[]){


    info.type = data[0];

    char stCh[4];
    arrCopy(data,1,stCh,0,4);
    info.sendTime = bytesToInt(stCh);

    char resId[8];
    arrCopy(data,5,resId,0,8);
    info.resourceId = bytesToLong(resId);

    char frameIndex[4];
    arrCopy(data,13,frameIndex,0,4);
    info.frameIndex = bytesToInt(frameIndex);

    char packageIndexBytes[4];
    arrCopy(packageIndexBytes,17,packageIndexBytes,0,4);
    info.packageIndex = bytesToInt(packageIndexBytes);

    char frameLength[4];
    arrCopy(data,21,frameLength,0,4);
    info.frameLength = bytesToInt(frameLength);

        //第一个分包的情况 报头 33 byte
    if(info.packageIndex == 32){
      char posBytes[8];
        arrCopy(data,25,posBytes,0,8);
        info.framePosition = bytesToLong(posBytes);
        char a = (char)malloc(sizeof(char)* sizeof(data) -33);
        info.data = (char *) a;
    }

    //其他分包 报头 25 byte
    else{
        info.framePosition = -1;
        char a = (char)malloc(sizeof(char)* sizeof(data) -33);
        info.data = (char *) a;
    }

}