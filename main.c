

#include <pthread.h>

pthread_t udpTask;
pthread_t tcpTask;
int clientInit();
void getDescribe(long long resId);
int main()
{
    clientInit();
    getDescribe(106ll);

    pthread_join(tcpTask,NULL);
    pthread_join(udpTask,NULL);
    return 1;
}





