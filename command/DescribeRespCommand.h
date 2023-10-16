//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_DESCRIBERESPCOMMAND_H
#define STREAM_CLIENT_DESCRIBERESPCOMMAND_H

struct DescribeRespCommand{
    char command;
    int seq;
    char success;
    long long resourceId;
    short height;
    short width;
    short framerate;
    long long length;
    int frameCount;
};

#endif //STREAM_CLIENT_DESCRIBERESPCOMMAND_H
