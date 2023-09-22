//
// Created by 24546 on 2023/9/19.
//
#include "CommandType.h"
#include "PauseRespCommand.h"

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);



struct PauseRespInfo pauseRespinfo;
/**
 *
 * @param data 解码前的数据
 */
struct PauseRespInfo* pauseDecode(char data[]){

    pauseRespinfo.command = data[0];

    char seq[4];
    arrCopy(data,1,seq,0,4);
    pauseRespinfo.seq = bytesToInt(seq);

    pauseRespinfo.success = data[5];

    return &pauseRespinfo;
}