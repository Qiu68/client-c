//
// Created by root on 10/23/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_TEXT 512
//消息结构体
struct my_msg_st {
    long int my_msg_type;                       //消息类型
    char anytext[MAX_TEXT];                     //消息数据
};

int main() {
    int idx = 1;
    int msgid;
    struct my_msg_st data;
    char buf[BUFSIZ];                        //设置缓存变量
    msgid = msgget((key_t) 1000, 0664 | IPC_CREAT);//创建消息队列
    if (msgid == -1) {
        perror("msgget err");
        exit(-1);
    }
    struct PacketGroup{
        //包组第一个接收到的分包 的到达时间戳
        int firstArrivalPacketTimestamp;

        //包组第一个分包发送时间戳
        int firstSentPacketTimestamp;

        //包组的最后一个包到达时间
        long long lastArrivalPacketTimestamp;
        //包组的最后一个包发送时间
        long long lastSentPacketTimestamp;
        //包组下标
        int groupIndex;

        int packageCount;

        struct PacketIds *packages;

        struct PacketGroup *next;
    };
    struct PacketGroup *packetGroup;
    packetGroup = (struct PacketGroup*) malloc(sizeof (struct PacketGroup));
    packetGroup->firstArrivalPacketTimestamp = 1;
    packetGroup->firstSentPacketTimestamp = 1;
    packetGroup->groupIndex = 1;
    packetGroup->lastArrivalPacketTimestamp = 1;
    packetGroup->next = NULL;
    while (idx < 5) {                            //发送消息

       // data.my_msg_type = rand() % 3 + 1;            //随机获取消息类型
        //发送消息
        if (msgsnd(msgid, (void *) packetGroup, sizeof(struct PacketGroup), 0) == -1) {
            perror("msgsnd err");
            exit(-1);
        }
        idx++;
    }
    free(packetGroup);
    packetGroup = NULL;
    return 0;
}