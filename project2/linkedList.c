//작성자: 박영진
//내용  : 연결리스트 구현을 위한 linkedList.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdlib.h>
#include "linkedList.h"

void append(NODE** list, void* data)
{
    NODE* temp = (NODE*)malloc(sizeof(NODE));
    temp->data = data;
    temp->next = NULL;

    NODE* current = *list;

    //만약 연결리스트가 비어있는 경우
    if(*list == NULL)
    {
        *list = temp;
        return;
    }

    //생성 위치 탐색
    while(current->next != NULL)
    {
        current = current->next;
    }

    current->next = temp;
}

NODE* search(NODE* list, void* data, CompareFunc compare)
{
    NODE* current = list;

    //compare 함수가 true인 current 찾기
    while(current != NULL)
    {
        if(compare(current->data, data))
            return current;
        current = current->next;
    }

    return NULL;
}

int deleteNode(NODE** list, void* data, CompareFunc compare)
{
    NODE* current = *list, *prev = NULL;

    //compare 함수가 true인 current을 찾고 해당 node 삭제
    while(current != NULL)
    {
        if(compare(current->data, data))
        {
            if(prev == NULL)
                *list = current->next;
            else
                prev->next = current->next;

            free(current->data);
            free(current);
            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}