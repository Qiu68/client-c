//
// Created by 24546 on 2023/9/22.
//
#include <stdlib.h>
#include "FramePacket.h"

void intToBytes(int num, char ch[]);

int bytesToInt(char ch[]);

void longToBytes(long long num, char ch[]);

void intToBytes(int num, char ch[]);

long long bytesToLong(char ch[]);

void arrCopy(char src[], int srcPos, char dest[], int destPos, int length);


struct FramePacket framePacketInfo;

struct FramePacket *FramePacketDecode(char data[],int length) {


    framePacketInfo.type = data[0];

    char stCh[4];
    arrCopy(data, 1, stCh, 0, 4);
    framePacketInfo.sendTime = bytesToInt(stCh);

    char resId[8];
    arrCopy(data, 5, resId, 0, 8);
    framePacketInfo.resourceId = bytesToLong(resId);

    char frameIndex[4];
    arrCopy(data, 13, frameIndex, 0, 4);
    framePacketInfo.frameIndex = bytesToInt(frameIndex);

    char packageIndexBytes[4];
    arrCopy(data, 17, packageIndexBytes, 0, 4);
    framePacketInfo.packageIndex = bytesToInt(packageIndexBytes);

    char frameLength[4];
    arrCopy(data, 21, frameLength, 0, 4);
    framePacketInfo.frameLength = bytesToInt(frameLength);

    //第一个分包的情况 报头 33 byte
    if (framePacketInfo.packageIndex == 1) {
        char posBytes[8];
        arrCopy(data, 25, posBytes, 0, 8);
        framePacketInfo.framePosition = bytesToLong(posBytes);


        framePacketInfo.dataLength = length - 33;

        //指向实际数据
        framePacketInfo.data = &data[33];
    }

        //其他分包 报头 25 byte
    else {
        framePacketInfo.framePosition = -1;

        framePacketInfo.dataLength = length - 25;

        framePacketInfo.data = &data[25];
    }
    return &framePacketInfo;
}