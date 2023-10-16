//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_PACKETTYPE_H
#define STREAM_CLIENT_PACKETTYPE_H

#endif //STREAM_CLIENT_PACKETTYPE_H

extern enum PacketType{

    FRAME =  1,                    //客户端初始化请求类型
    CLIENT_UDP_INIT_REQ = 2,            //客户端初始化响应类型
    CLIENT_UDP_INIT_RESP = 3,           //客户端初始化响应类型
    MTU_DISCOVER = 4,               //MTU值探测包类型
    CLIENT_INIT_TCP_REQ = 5,        //客户端初始化Tcp请求类型
    CLIENT_INIT_TCP_RESP = 6        //客户端初始化Tcp响应类型
};