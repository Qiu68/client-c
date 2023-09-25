//
// Created by 24546 on 2023/9/25.
//

#ifndef STREAM_CLIENT_PACKETLENGTH_H
#define STREAM_CLIENT_PACKETLENGTH_H

#endif //STREAM_CLIENT_PACKETLENGTH_H



extern enum PacketLength{

    FRAME_LENGTH =  1,                    //客户端初始化请求类型
    CLIENT_INIT_REQ_LENGTH = 9,            //客户端初始化响应类型
    CLIENT_INIT_RESP_LENGTH = 9,           //客户端初始化响应类型
    MTU_DISCOVER_LENGTH = 4,               //MTU值探测包类型
    CLIENT_INIT_TCP_REQ_LENGTH = 5,        //客户端初始化Tcp请求类型
    CLIENT_INIT_TCP_RESP_LENGTH = 6        //客户端初始化Tcp响应类型
};
