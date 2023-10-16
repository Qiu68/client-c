#include <stdio.h>
#include <string.h>
#include "CommandType.h"
#include "ClientInitReqCommand.h"
//
// Created by 24546 on 2023/9/18.
//

void intToBytes(int num,char ch[]);
int BytesToInt(char ch[]);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);


/**
 *
 * @param data 编码后的载体 char data[] 引用传递
 * @param command 命令类型
 * @param sequence 命令序列号
 */
void encode(char data[],int sequence){
   struct ClientInitReqInfo info;
    char seqChar[4];
    info.command = CLIENT_INIT_REQ;
    info.sequence = sequence;
    data[0] = CLIENT_INIT_REQ;
    intToBytes(sequence,seqChar);
    arrCopy(seqChar,0,data,1, sizeof(seqChar));
}

