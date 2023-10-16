//
// Created by 24546 on 2023/9/22.
//

#ifndef STREAM_CLIENT_FRAMEPACKET_H
#define STREAM_CLIENT_FRAMEPACKET_H

#endif //STREAM_CLIENT_FRAMEPACKET_H


struct FramePacket{
    char type;
    int sendTime;
    long long resourceId;
    int frameIndex;
    int packageIndex;
    int frameLength;
    long long framePosition;
    char data[1400];

    int dataLength;
};