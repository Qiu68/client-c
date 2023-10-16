//#include <pthread.h>
//#include <stdbool.h>
//#include <stdlib.h>
//
//typedef struct Node {
//    int data;
//    struct Node* next;
//} Node;
//
//typedef struct {
//    Node* head;
//    pthread_rwlock_t lock;
//} LinkedList;
//
//void init_linkedlist(LinkedList* list) {
//    list->head = NULL;
//    pthread_rwlock_init(&list->lock, NULL);
//}
//
//void insert_element(LinkedList* list, int element) {
//    Node* new_node = (Node*)malloc(sizeof(Node));
//    new_node->data = element;
//
//    pthread_rwlock_wrlock(&list->lock); // 获取写锁
//
//    new_node->next = list->head;
//    list->head = new_node;
//
//    pthread_rwlock_unlock(&list->lock); // 解锁
//}
//
//bool remove_element(LinkedList* list, int element) {
//    pthread_rwlock_wrlock(&list->lock); // 获取写锁
//
//    Node** curr = &(list->head);
//    while (*curr != NULL) {
//        if ((*curr)->data == element) {
//            Node* temp = *curr;
//            *curr = (*curr)->next;
//            free(temp);
//
//            pthread_rwlock_unlock(&list->lock); // 解锁
//            return true;
//        }
//        curr = &((*curr)->next);
//    }
//
//    pthread_rwlock_unlock(&list->lock); // 解锁
//    return false;
//}
//
//bool contains_element(LinkedList* list, int element) {
//    pthread_rwlock_rdlock(&list->lock); // 获取读锁
//
//    Node* curr = list->head;
//    while (curr != NULL) {
//        if (curr->data == element) {
//            pthread_rwlock_unlock(&list->lock); // 解锁
//            return true;
//        }
//        curr = curr->next;
//    }
//
//    pthread_rwlock_unlock(&list->lock); // 解锁
//    return false;
//}