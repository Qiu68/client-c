//
// Created by 24546 on 2023/9/26.
//
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#ifndef STREAM_CLIENT_PACKET_H
#define STREAM_CLIENT_PACKET_H

#endif //STREAM_CLIENT_PACKET_H


struct Packet{
    //服务器发送这个包的时间戳
    int sendTime;
    //客户端接收到这个包的时间戳
    int revTime;

    struct Packet *next;
};
