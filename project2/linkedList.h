//작성자: 박영진
//내용  : 연결리스트 구현을 위한 linkedList.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

//연결리스트 구조체
typedef struct node
{
    void* data;
    struct node* next;
}NODE;

//비교 함수 포인터 이름 정의
typedef int (*CompareFunc)(const void*, const void*);

//연결리스트에 새로운 노드 추가
void append(NODE** list, void* data);
//연결리스트에서 특정 노드 찾기
NODE* search(NODE* list, void* data, CompareFunc compare);
//연결리스트에서 특정 노드 삭제
int deleteNode(NODE** list, void* data, CompareFunc compare);

#endif