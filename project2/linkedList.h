//작성자: 박영진
//내용  : 연결리스트 구현을 위한 linkedList.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node
{
    void* data;
    struct node* next;
}NODE;

typedef int (*CompareFunc)(const void*, const void*);

void append(NODE** list, void* data);
NODE* search(NODE* list, void* data, CompareFunc compare);
int deleteNode(NODE** list, void* data, CompareFunc compare);

#endif