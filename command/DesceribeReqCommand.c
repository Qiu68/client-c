#include <stdio.h>
#include "DesceribeReqCommand.h"
#include "CommandType.h"

//
// Created by 24546 on 2023/9/19.
//

struct DescribeReqInfo describeReqInfo;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

 void desReqEcode(char data[], int sequence, long long resId){
    char seqChar[4];
    char resChar[8];

    describeReqInfo.command = DESCRIBE_REQ;
    describeReqInfo.sequence = sequence;
    describeReqInfo.resourceId = resId;

    data[0] = describeReqInfo.command;
    intToBytes(describeReqInfo.sequence,seqChar);
    arrCopy(seqChar,0,data,1,4);

    longToBytes(describeReqInfo.resourceId,resChar);
    arrCopy(resChar,0,data,5,8);


}

