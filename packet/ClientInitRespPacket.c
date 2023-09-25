#include <stdio.h>
#include "PacketType.h"
#include "ClientInitRespPacket.h"


//
// Created by 24546 on 2023/9/19.
//

struct clientInitRespPacket clientInitRespinfo;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct clientInitRespPacket* clientInitRespDecode(char data[]){

    clientInitRespinfo.type = data[0];

    char seqChar[4];
    arrCopy(data,1,seqChar,0,4);
    clientInitRespinfo.seq = bytesToInt(seqChar);

    clientInitRespinfo.success = data[5];

    arrCopy(data,6,clientInitRespinfo.clientHost,0,4);

    char portBytes[4];
    arrCopy(data,10,portBytes,0,4);
    clientInitRespinfo.clientPort = bytesToInt(portBytes);

    char sizeBytes[4];
    arrCopy(data,14,sizeBytes,0,4);
    clientInitRespinfo.packageSize = bytesToInt(sizeBytes);
    return &clientInitRespinfo;
}