//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_PONGCOMMAND_H
#define STREAM_CLIENT_PONGCOMMAND_H

struct PongInfo{
    char command;
    int sequence;
    long long timestamp;
    int processTimeMs;
    int receiveCount;
    char delayChangeLevel;
};

#endif //STREAM_CLIENT_PONGCOMMAND_H
