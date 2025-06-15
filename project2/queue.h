//작성자: 박영진
//내용  : 자료구조 큐 구현을 위한 queue.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef QUEUE_H
#define QUEUE_H

//NODE 구조체를 사용할 것이라고 명시
typedef struct node NODE;

//큐를 사용하기 위한 구조체 선언
typedef struct queue
{
    NODE* front;
    NODE* rear;
}QUEUE;

//큐가 비어있는지 확인하는 함수
int isEmpty(QUEUE*);
//큐에 데이털르 추가하는 함수
void enqueue(QUEUE*, void*);
//큐에 데이터를 삭제하는 함수
void* dequeue(QUEUE*);

#endif