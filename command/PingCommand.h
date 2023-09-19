//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_PINGCOMMAND_H
#define STREAM_CLIENT_PINGCOMMAND_H
struct PingInfo{
    char command;
    int sequence;
    long long timestamp;
    int rtt;
};
#endif //STREAM_CLIENT_PINGCOMMAND_H
