//
// Created by 24546 on 2023/9/19.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CommandType.h"
#include "ClientInitRespCommand.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);



struct ClientInitRespInfo info;
/**
 *
 * @param data 解码前的数据
 */
struct ClientInitRespInfo* clientInitTcpDecode(char data[]){

    info.command = data[0];

    char seq[4];
    arrCopy(data,1,seq,0,4);
    info.seq = bytesToInt(seq);

    info.success = data[5];

    arrCopy(data,6,info.clientHost,0,4);

    char port[4];
    arrCopy(data,10,port,0,4);
    info.clientPort = bytesToInt(port);

    char size[4];
    arrCopy(data,14,size,0,4);
    info.packetSize = bytesToInt(size);
    return &info;
}