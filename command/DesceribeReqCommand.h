//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_DESCERIBEREQCOMMAND_H
#define STREAM_CLIENT_DESCERIBEREQCOMMAND_H

struct DescribeReqInfo{
    char command;
    int sequence;
    long long resourceId;
};

#endif //STREAM_CLIENT_DESCERIBEREQCOMMAND_H
