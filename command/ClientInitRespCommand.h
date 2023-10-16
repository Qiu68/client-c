//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_CLIENTINITRESPCOMMAND_H
#define STREAM_CLIENT_CLIENTINITRESPCOMMAND_H

struct ClientInitRespInfo{
    char command;
    int seq;
    char success;
    char clientHost[4];
    int clientPort;
    int packetSize;
};

#endif //STREAM_CLIENT_CLIENTINITRESPCOMMAND_H
