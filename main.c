
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client/pojo/Packet.h"

//统计心跳互斥锁
pthread_mutex_t  packetCountMutex;
pthread_mutex_t  frameMutex;
pthread_t udpTask;
pthread_t tcpTask;
pthread_t frameCheckTask;

int clientInit();
void initTask();
void getDescribe(long long resId);
void startPlay(int frameIndex);
void setResourceId(long long id);
void setStartFrameIndex(int frameIndex);
struct Packet *removeNode(struct Packet *head,struct Packet *ptr);
struct timeval getTimeStamp();
long long getSystemTimestamp();

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);

int main() {

    pthread_mutex_init(&packetCountMutex,NULL);
    pthread_mutex_init(&frameMutex,NULL);

    clientInit();
    getDescribe(106ll);
    setStartFrameIndex(1);
    setResourceId(106ll);
    startPlay(1);
    initTask();

    pthread_join(tcpTask,NULL);
    pthread_join(udpTask,NULL);
    pthread_join(frameCheckTask,NULL);
//struct testNode{
//    int id;
//    struct testNode *next;
//};
//    int arr[500000];
//    long long s = getSystemTimestamp();
//    for (int i = 0; i < 500000; ++i) {
//        arr[i] = i;
//    }
//    printf("time=%lld",(getSystemTimestamp() - s));
//    s = getSystemTimestamp();
//    for (int i = 0; i < 500000; ++i) {
//        arr[i];
//    }
//    printf("time=%lld",(getSystemTimestamp() - s));
    return 0;
}
