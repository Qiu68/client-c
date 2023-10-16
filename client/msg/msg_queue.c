//
// Created by root on 10/13/23.
//
#include <sys/msg.h>

#include "../pojo/Frame.h"

//消息结构体
struct my_msg_st {
    long int my_msg_type;                       //消息类型
    struct Frame frameData;                     //消息数据
};

int msgid = -1;
int create_msg(){
    msgid = msgget((key_t) 1000, 0664 | IPC_CREAT);//创建消息队列
    if (msgid == -1) {
        return -1;
    }
    return 1;
}

int get_msgid(){
    return msgid;
}


