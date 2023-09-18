//
// Created by 24546 on 2023/9/18.
//
#include "stdio.h"
 void intToBytes(int num,char ch[]){
    ch[0] = (char) ((num >> 24) & 0xFF);
    ch[1] = (char) ((num >> 16) & 0xFF);
    ch[2] = (char) ((num >> 8) & 0xFF);
    ch[3] = (char) (num & 0xFF);
}

int BytesToInt(char ch[]){
    int value;
    value = (int) ((ch[3] & 0xFF)
                   | ((ch[2] & 0xFF) << 8)
                   | ((ch[1] & 0xFF) << 16)
                   | ((ch[0] & 0xFF) << 24));
    return value;
}

void longToBytes(long long num,char ch[]){
    ch[0] = (char) (num >> 56);
    ch[1] = (char) (num >> 48);
    ch[2] = (char) (num >> 40);
    ch[3] = (char) (num >> 32);
    ch[4] = (char) (num >> 24);
    ch[5] = (char) (num >> 16);
    ch[6] = (char) (num >> 8);
    ch[7] = (char) (num >> 0);
}

long long BytesToLong(char ch[]){
    long long value = 0;
    value = (int) ((ch[7] & 0xFF)
                   | ((ch[6] & 0xFF) << 8)
                   | ((ch[5] & 0xFF) << 16)
                   | ((ch[4] & 0xFF) << 24)
                   | ((ch[3] & 0xFF) << 32)
                   | ((ch[2] & 0xFF) << 40)
                   | ((ch[1] & 0xFF) << 48)
                   | ((ch[0] & 0xFF) << 56));
    return value;
}