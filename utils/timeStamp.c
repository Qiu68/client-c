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

/**
 * 获取两个时间戳的毫米差
 * @param start
 * @param end
 * @return
 */
long getLeadTime(struct timeval *start,struct timeval *end){
    return (1000000 * ( end->tv_sec - start->tv_sec ) + end->tv_usec - start->tv_usec) / 1000;
}
