
//
// Created by 24546 on 2023/9/18.
//


#include <stdio.h>
#include <math.h>

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length){
    for (int i = 0; i < length; ++i) {
        dest[destPos +i] = src[srcPos + i];
    }
}

//void intToString(char src[],int num){
//    int tmp = num;
//    src[0] = tmp % 10 + '0';
//    for (int i = 1; tmp >=1; ++i) {
//        tmp = num / (int ) (pow(10,i));
//        src[i] = tmp % 10 + '0';
//    }
//}






