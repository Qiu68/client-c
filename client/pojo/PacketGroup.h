//
// Created by 24546 on 2023/10/16.
//

#ifndef CLIENT_TEST_PACKETGROUP_H
#define CLIENT_TEST_PACKETGROUP_H

struct PacketIds{
    char ids[30];
    int idLength;
    struct PacketIds *next;
};

struct PacketGroup{
    //包组第一个接收到的分包 的到达时间戳
     int firstArrivalPacketTimestamp;

    //包组第一个分包发送时间戳
     int firstSentPacketTimestamp;

    //包组的最后一个包到达时间
     long long lastArrivalPacketTimestamp;
    //包组的最后一个包发送时间
     long long lastSentPacketTimestamp;
    //包组下标
     int groupIndex;

     int packageCount;

     struct PacketIds *packages;

     struct PacketGroup *next;
};
#endif //CLIENT_TEST_PACKETGROUP_H
