//작성자: 박영진
//내용  : 자료구조 구현을 위한 queue.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdlib.h>
#include "linkedList.h"
#include "queue.h"

int isEmpty(QUEUE* queue)
{
    return queue->front == NULL;
}

void enqueue(QUEUE* queue, void* data)
{
    NODE* node = (NODE*)malloc(sizeof(NODE));
    node->data = data;
    node->next = NULL;

    if(isEmpty(queue))
    {
        queue->front = queue->rear = node;
        return;
    }

    NODE* current = queue->front;

    if(current->next != NULL)
    {  
        current = current->next;
    }

    current->next = node;
    queue->rear = node;
}

void* dequeue(QUEUE* queue)
{
    if(isEmpty(queue)) return NULL;

    NODE* node = queue->front;
    void* data = node->data;
    queue->front = queue->front->next;

    if(queue->front == NULL)
        queue->rear = NULL;

    free(node);
    return data;
}