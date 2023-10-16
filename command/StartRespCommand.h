//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_STARTRESPCOMMAND_H
#define STREAM_CLIENT_STARTRESPCOMMAND_H
struct StartRespInfo{
    char command;
    int sequence;
    char success;
    long long time;
};
#endif //STREAM_CLIENT_STARTRESPCOMMAND_H
