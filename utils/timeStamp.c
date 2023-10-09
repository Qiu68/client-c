//
// Created by 24546 on 2023/9/18.
//
#include "stdio.h"
#include <sys/time.h>

/**
 * 获取系统当前时间戳
 * @return
 */
struct timeval getTimeStamp(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return time;
}

 long long  getTimeStampByUs(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec;
}

/**
 * 获取两个时间戳的毫米差
 * @param start
 * @param end
 * @return
 */
long getLeadTime(struct timeval *start,struct timeval *end){
    return (1000000 * ( end->tv_sec - start->tv_sec ) + end->tv_usec - start->tv_usec) / 1000;
}


long long getSystemTimestamp(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec) * 1000ll + (time.tv_usec) / 1000;
}