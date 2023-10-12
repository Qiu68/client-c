//
// Created by 24546 on 2023/10/11.
//

#ifndef CLIENT_TEST_NACKREQCOMMAND_H
#define CLIENT_TEST_NACKREQCOMMAND_H
struct nackReqCommand{
char commandType;
int seq;
int lossPacketCount;
long long resourceId;
int receivePort;
};
#endif //CLIENT_TEST_NACKREQCOMMAND_H
