

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "client/pojo/Packet.h"


pthread_t udpTask;
pthread_t tcpTask;
int clientInit();
void getDescribe(long long resId);
void startPlay(int frameIndex);
void setResourceId(long long id);
void setStartFrameIndex(int frameIndex);
int hashMapTest();
struct timeval getTimeStamp();
int main()
{
//    clientInit();
//    getDescribe(101ll);
//    setStartFrameIndex(1);
//    setResourceId(101ll);
//    startPlay(1);
//
//
//    pthread_join(tcpTask,NULL);
//    pthread_join(udpTask,NULL);

//     hashMapTest();
    getTimeStamp();

    struct Packet *head,*tail,*p;

    p = (struct Packet *) malloc(sizeof(struct Packet));
    p->revTime = 455;
    p->sendTime = 566;
    head = p;

    p = (struct Packet *) malloc(sizeof (struct Packet));
    p->revTime = 111;
    p->sendTime = 678;
    p->next = NULL;
    head->next = p;

    printf("test");
    return 1;
}





