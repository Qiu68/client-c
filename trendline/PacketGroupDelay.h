//
// Created by 24546 on 2023/10/17.
//

#ifndef CLIENT_TEST_PACKETGROUPDELAY_H
#define CLIENT_TEST_PACKETGROUPDELAY_H
struct PacketGroupDelay{
    //包组发送时间间隔
     int sendDelta;

    //包组接收时间间隔
     int recvDelta;

    //包组最早发送时间
     int sendTimeMs;

    //包组最早到达时间
     int arrivalTimeMs;

     struct PacketGroupDelay *next;
};
#endif //CLIENT_TEST_PACKETGROUPDELAY_H
