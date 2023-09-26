#include <stdio.h>
#include "PacketType.h"
#include "ClientInitReqPacket.h"


//
// Created by 24546 on 2023/9/19.
//

struct clientInitReqPacket clientInitReqinfo;

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length);

void clientInitEcode(char data[], char clientHost[4],int clientPort){

    clientInitReqinfo.type = CLIENT_UDP_INIT_REQ;

    data[0] = clientInitReqinfo.type;

    arrCopy(clientHost,0,data,1,4);

    char portBytes[4];
    intToBytes(clientPort,portBytes);
    arrCopy(data,5,portBytes,0,4);


}