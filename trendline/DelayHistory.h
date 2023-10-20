//
// Created by 24546 on 2023/10/20.
//

#ifndef CLIENT_TEST_DELAYHISTORY_H
#define CLIENT_TEST_DELAYHISTORY_H

//历史延迟记录   key=x=开始播放相对时间   value=延时毫秒值经过平滑处理之后的值
struct delayHistory{
    int key;
    double value;
    struct delayHistory *next;
};

#endif //CLIENT_TEST_DELAYHISTORY_H
