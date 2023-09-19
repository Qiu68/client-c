//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_STARTREQCOMMAND_H
#define STREAM_CLIENT_STARTREQCOMMAND_H

struct StartReqInfo{
    char command;
    int sequence;
    long long resourceId;
    char protocol;
    int receivePort;
    int startFrameIndex;
};

#endif //STREAM_CLIENT_STARTREQCOMMAND_H
