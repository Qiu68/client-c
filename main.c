#include <stdio.h>
#include <pthread.h>
#include "command/DescribeRespCommand.h"
#define pf printf


void tcpListenerInit();
int tcpInit();
int main()
{
    int stat = tcpInit();
    if(stat < 0){
        printf("连接失败！");
        return -1;
    }
    tcpListenerInit();
    getchar();

//    pthread_join(,NULL);
    return 1;
}





