//
// Created by 24546 on 2023/9/18.
//

#include <stdio.h>
#include <pthread.h>

#define pf printf

void *task1(void* args);
void *task2(void* args);


void test111(){
    pthread_t t1;
    pthread_t t2;

    pthread_create(&t1,NULL,task1,NULL);
    pthread_create(&t2,NULL,task2,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
}

void *task1(void* args){
    for (int i = 0; i < 100000; ++i) {
        pf("1");
    }
}

void *task2(void* args){
    for (int i = 0; i < 100000; ++i) {
        pf("2");
    }
}