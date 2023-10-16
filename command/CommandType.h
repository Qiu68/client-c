//
// Created by 24546 on 2023/9/18.
//

#ifndef STREM_CLIENT_C_COMMANDTYPE_H
#define STREM_CLIENT_C_COMMANDTYPE_H

#endif //STREM_CLIENT_C_COMMANDTYPE_H

extern enum commandType{
    DESCRIBE_REQ = 1,       //查询描述请求指令
    START_REQ = 2,          //开始推流请求指令
    PAUSE_REQ = 3,          //暂停推流请求指令
    NACK_REQ = 4,           //丢包重发请求指令
    CLIENT_INIT_REQ = 5,    //客户端初始化请求指令
    SET_UDP_MTU_REQ = 6,    //设置udp mtu请求指令
    PING = 100,             //心跳 ping指令

    DESCRIBE_RESP =  (int)(char) (DESCRIBE_REQ & 0b10000000==0 ? 0:(DESCRIBE_REQ | 0b10000000)) ,//查询描述响应指令
    START_RESP = (int)(char) (START_REQ & 0b10000000==0 ? 0:(START_REQ | 0b10000000)),//开始推流响应指令
    PAUSE_RESP = (int)(char) (PAUSE_REQ & 0b10000000==0 ? 0:(PAUSE_REQ | 0b10000000)),//暂停推流响应指令
    NACK_RESP =  (int)(char) (NACK_REQ & 0b10000000==0 ? 0:(NACK_REQ | 0b10000000)),//丢包重发响应指令
    CLIENT_INIT_RESP = (int)(char) (CLIENT_INIT_REQ & 0b10000000==0 ? 0:(CLIENT_INIT_REQ | 0b10000000)) ,//客户端初始化响应指令
    SET_UDP_MTU_RESP = (int)(char) (SET_UDP_MTU_REQ & 0b10000000==0 ? 0:(SET_UDP_MTU_REQ | 0b10000000)),//设置udp mtu响应指令
    PONG = (int)(char) (PING & 0b10000000==0 ? 0:(PING | 0b10000000))//心跳 pong指令
};
