#include <stdio.h>
#include <string.h>
//
// Created by 24546 on 2023/9/18.
//

void intToBytes(int num,char ch[]);
int BytesToInt(char ch[]);
void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

struct ClientInitReqInfo{
     const int LENGTH;
     char command;
     int sequence;

}info;

char* encode(char command,int sequence){
    char data[5];
    char seqChar[4];
    info.command = command;
    info.sequence = sequence;
    data[0] = info.command;

    intToBytes(sequence,seqChar);
    arrCopy(seqChar,0,data,1, sizeof(seqChar));

    return data;
}

