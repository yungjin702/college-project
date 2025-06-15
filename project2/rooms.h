//작성자: 박영진
//내용  : 방에 접속한 유저들을 관리하기 위한 rooms.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef ROOMS_H
#define ROOMS_H

#include "queue.h"

typedef unsigned int SOCKET;
typedef struct userinfo USERINFO;

typedef struct roominfo
{
    char roomName[16];
    int userCount;
    int readyUser[4];
    int overCount;
    SOCKET clientSocks[4];
    USERINFO* users[4];
    int scores[4];
    QUEUE emptyClientNum;
}ROOMINFO;

typedef struct rooms
{
    ROOMINFO arr[5];
    QUEUE emptyRooms;
    int size;
}ROOMS;

void initRooms(ROOMS*);
int* createRoom(ROOMS*, const char*);
void removeRoom(ROOMS*, const int);
int readyClient(ROOMS*, int, int);
int connectClientToRoom(ROOMS*, int, USERINFO*, SOCKET);
void disconnectClientFromRoom(ROOMS*, int, int);

#endif