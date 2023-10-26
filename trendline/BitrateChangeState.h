//
// Created by 24546 on 2023/10/16.
//

#ifndef CLIENT_TEST_BITRATECHANGESTATE_H
#define CLIENT_TEST_BITRATECHANGESTATE_H
enum BitrateChangeState{
    //码率上调
    INCR =  '1',
    //码率不变
    HOLD = '0',
    //码率下调
    DECR = '2'
};
#endif //CLIENT_TEST_BITRATECHANGESTATE_H
