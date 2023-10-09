//
// Created by 24546 on 2023/10/8.
//

#ifndef CLIENT_TEST_CHECKFRAMTASK_H
#define CLIENT_TEST_CHECKFRAMTASK_H
#include "../pojo/Frame.h"
//struct frameList{
//    int frameIndex;
//    struct Frame *node;
//};
//struct frameInCompleteList{
//    int frameIndex;
//    struct Frame *next;
//};
struct frameCompleteList{
    int frameIndex;
    struct Frame *next;
};
struct frameLength{
    int frameIndex;
    int frameLength;
    struct frameLength *next;
};
struct framePos{
    int frameIndex;
    long long  framePos;
    struct framePos *next;
};
#endif //CLIENT_TEST_CHECKFRAMTASK_H
