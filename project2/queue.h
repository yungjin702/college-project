//작성자: 박영진
//내용  : 자료구조 구현을 위한 queue.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef QUEUE_H
#define QUEUE_H

typedef struct node NODE;

typedef struct queue
{
    NODE* front;
    NODE* rear;
}QUEUE;

int isEmpty(QUEUE*);
void enqueue(QUEUE*, void*);
void* dequeue(QUEUE*);

#endif