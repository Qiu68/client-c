//
// Created by 24546 on 2023/9/25.
//


#include "../../command/CommandType.h"
#include "../../command/CommandLenght.h"

int sendTcpMsg(char data[]);
void desReqEcode(char data[], int sequence, long long resId);

//int sendPong(char data[], int sequence, long long resId){
//    sendTcpMsg(data);
//    printf("------ send describe msg success ------\n");
//    return 1;
//}