
//
// Created by 24546 on 2023/9/18.
//


#include <stdio.h>
#include <string.h>

void arrCopy(char src[],int srcPos,char dest[],int destPos,int length){
    for (int i = 0; i < length; ++i) {
        dest[destPos +i] = src[srcPos + i];
    }
}

void chartoInt(char *ch,int size){
    int num;
    for (int i = 0; i < size; ++i) {
        num = (int ) *ch;
        printf("%d.",num);
        ch++;
    }
}