//
// Created by 24546 on 2023/9/22.
//

#ifndef STREAM_CLIENT_CLIENTINITRESPPACKET_H
#define STREAM_CLIENT_CLIENTINITRESPPACKET_H

#endif //STREAM_CLIENT_CLIENTINITRESPPACKET_H

struct clientInitRespPacket{
    char type;
    int seq;
    char success;
    char clientHost[4];
    int clientPort;
    int packageSize;
};