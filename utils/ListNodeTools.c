//
// Created by 24546 on 2023/9/27.
//

#include <stdlib.h>
#include "../client/pojo/Packet.h"


struct Packet * removeNode(struct Packet *head,struct Packet *ptr){

    struct Packet *aide;
    aide = head;

    //移除首节点
    if( head == ptr ){
        //链表只有一个节点的情况
        if(head->next == NULL){
            free(head);
            head = NULL;
        }
        else {
            head = head->next;
            free(aide);
        }
    }

    //移除尾结点
    else if(ptr->next == NULL){
        //遍历到ptr节点的上一个节点
        while (aide->next != ptr){
            aide = aide->next;
        }
        //断开与ptr的连接
        aide->next = NULL;
        free(ptr);
        ptr = NULL;
    }

    //中间节点
    else{

        //遍历到ptr节点的上一个节点
        while (aide->next != ptr){
            aide = aide->next;
        }
        aide->next = ptr->next;
        free(ptr);
    }
    aide = NULL;
    return head;
}