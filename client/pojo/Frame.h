//
// Created by 24546 on 2023/10/7.
//

#ifndef CLIENT_TEST_FRAME_H
#define CLIENT_TEST_FRAME_H
struct packetData{
    int packageIndex;
    int packageLength;
    char data[1400];
    struct packetData *next;
};
struct lossPacket{
    int id;
    int retryCount;
    long long lossTimestamp;
    struct lossPacket *next;
};
//struct retryCount{
//    int packetIndex;
//    int count;
//    struct retryCount *next;
//};
//struct lossTimestamp{
//    int packetIndex;
//    long long timestamp;
//    struct lossTimestamp *next;
//};
struct Frame{
    int frameIndex;
    int frameLength;
    long long framePosition;
    int packetSum;
    int prevPacketOrder;
    struct packetData  *packetNode;
    struct lossPacket *lossPacketNode;
    struct retryCount *retryCountNode;
    struct lossTimestamp *lossTimestampNode;
    struct Frame *next;
};

#endif //CLIENT_TEST_FRAME_H
