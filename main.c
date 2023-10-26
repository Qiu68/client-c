
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client/pojo/Packet.h"
#include "client/pojo/Frame.h"
#include "log/log.h"

//统计心跳链表互斥锁
pthread_mutex_t  packetCountMutex;
//帧链表互斥锁
pthread_mutex_t  frameMutex;
//不完整帧互斥锁
pthread_mutex_t frameInCompleteMutex;
//完整帧互斥锁
pthread_mutex_t frameCompleteMutex;
//packetGroup链表互斥锁 用于计算斜率
pthread_mutex_t packetGroupMutex;
//trendCalculater 互斥锁 用于计算斜率
pthread_mutex_t trendCalculaterMutex;

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
void intToString(char src[],int num);

void intToBytes(int num,char ch[]);
int bytesToInt(char ch[]);


FILE *logFd;


int main() {

    logFd = fopen("/home/test/client.log", "ab");
    if(logFd == NULL) {
        printf("logfd error!");
        return -1;
    }
    log_add_fp(logFd,LOG_INFO);


    pthread_mutex_init(&packetCountMutex,NULL);
    pthread_mutex_init(&frameMutex,NULL);
    pthread_mutex_init(&frameInCompleteMutex,NULL);
    pthread_mutex_init(&frameCompleteMutex,NULL);
    pthread_mutex_init(&packetGroupMutex,NULL);
    pthread_mutex_init(&trendCalculaterMutex,NULL);

    clientInit();
    getDescribe(101ll);
    setStartFrameIndex(1);
    setResourceId(101ll);
    startPlay(1);
    initTask();
    checkRetryInit();

    pthread_join(tcpTask,NULL);
    pthread_join(udpTask,NULL);
    pthread_join(frameCheckTask,NULL);
    pthread_join(retryTask,NULL);
   
    return 0;

}
