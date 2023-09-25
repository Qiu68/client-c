//
// Created by 24546 on 2023/9/25.
//

#ifndef STREAM_CLIENT_COMMANDLENGHT_H
#define STREAM_CLIENT_COMMANDLENGHT_H

#endif //STREAM_CLIENT_COMMANDLENGHT_H

//存放各命令类型的长度
extern enum commandLength{
    DESCRIBE_REQ_LENGTH = 13,       //查询描述请求指令
    START_REQ_LENGTH = 22,          //开始推流请求指令
    PAUSE_REQ_LENGTH = 5,          //暂停推流请求指令
    NACK_REQ_LENGTH = 5+8+4+4,           //丢包重发请求指令
    CLIENT_INIT_REQ_LENGTH = 5,    //客户端初始化请求指令
    SET_UDP_MTU_REQ_LENGTH = 9,    //设置udp mtu请求指令
    PING_LENGTH = 17,             //心跳 ping指令

    DESCRIBE_RESP_LENGTH = 29  ,//查询描述响应指令
    START_RESP_LENGTH = 13 ,//开始推流响应指令
    PAUSE_RESP_LENGTH = 6 ,//暂停推流响应指令
    NACK_RESP_LENGTH = 6,//丢包重发响应指令
    CLIENT_INIT_RESP_LENGTH = 17 ,//客户端初始化响应指令
    SET_UDP_MTU_RESP_LENGTH = 6 ,//设置udp mtu响应指令
    PONG_LENGTH = 22 //心跳 pong指令

};