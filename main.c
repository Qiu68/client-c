
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client/pojo/Packet.h"

//统计心跳互斥锁
pthread_mutex_t  packetCountMutex;

pthread_mutex_t  frameMutex;

pthread_mutex_t frameInCompleteMutex;

pthread_mutex_t frameCompleteMutex;

//udp监听线程
pthread_t udpTask;
//tcp监听线程
pthread_t tcpTask;
//帧检查线程
pthread_t frameCheckTask;
//重传线程
pthread_t retryTask;

int clientInit();
void initTask();
int checkRetryInit();
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
    pthread_mutex_init(&frameInCompleteMutex,NULL);
    pthread_mutex_init(&frameCompleteMutex,NULL);

    clientInit();
    getDescribe(106ll);
    setStartFrameIndex(1);
    setResourceId(106ll);
    startPlay(1);
    initTask();
    checkRetryInit();

    pthread_join(tcpTask,NULL);
    pthread_join(udpTask,NULL);
    pthread_join(frameCheckTask,NULL);
    pthread_join(retryTask,NULL);
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
