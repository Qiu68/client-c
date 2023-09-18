#include <stdio.h>
#include "command/CommandType.h"


/* 定义最大缓冲区大小 ,
 * 如果文件小于 64 KB , 则缓冲区就是文件大小 ,
 * 如果文件大于 64 KB , 则缓冲区就是该大小
 */

#define st struct
#define pf printf

//int test();
//struct timeval getTimeStamp();
//long getLeadTime(struct timeVal *start,struct timeVal *end);

char* intToBytes(int num,char p[]);
int BytesToInt(char ch[]);
void longToBytes(long long num,char ch[]);
long long BytesToLong(char ch[]);
char* encode(char command,int sequence);

int main()
{
    encode(CLIENT_INIT_REQ,4);
    return 1;
}





