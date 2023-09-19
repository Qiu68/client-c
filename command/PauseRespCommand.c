//
// Created by 24546 on 2023/9/19.
//
#include "CommandType.h"
#include "PauseRespCommand.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);



struct PauseRespInfo info;
/**
 *
 * @param data 解码前的数据
 */
struct PauseRespInfo* decode(char data[]){

    info.command = data[0];

    char seq[4];
    arrCopy(data,1,seq,0,4);
    info.seq = bytesToInt(seq);

    info.success = data[5];

    return &info;
}