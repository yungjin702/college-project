//작성자: 박영진
//내용  : 방에 접속한 유저들을 관리하기 위한 rooms.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "queue.h"
#include "linkedList.h"
#include "users.h"
#include "rooms.h"

void initRooms(ROOMS* rooms)
{
    rooms->emptyRooms.front = NULL;
    rooms->emptyRooms.rear = NULL;
    rooms->size = 0;

    for(int i = 0; i < 5; ++i)
    {
        rooms->arr[i].emptyClientNum.front = NULL;
        rooms->arr[i].emptyClientNum.rear = NULL;
        strcpy(rooms->arr[i].roomName, "EMPTY");

        int* temp = (int*)malloc(sizeof(int));
        *temp = i;
        enqueue(&rooms->emptyRooms, (void*)temp);
    }
}

int* createRoom(ROOMS* rooms, const char* roomName)
{
    //빈 방 확인
    int* emptyRoomNum = (int*)dequeue(&rooms->emptyRooms);

    if(emptyRoomNum == NULL) return NULL;
    
    ROOMINFO* room = &rooms->arr[*emptyRoomNum];

    //방 정보 등록 및 초기화
    strcpy(room->roomName, roomName);
    room->userCount = 0;
    memset(room->readyUser, 0, sizeof(room->readyUser));
    room->overCount = 0;
    memset(room->clientSocks, INVALID_SOCKET, sizeof(room->clientSocks));
    memset(room->scores, 0, sizeof(room->scores));

    for(int i = 0; i < 4; ++i)
    {
        int* temp = (int*)malloc(sizeof(int));
        *temp = i;
        enqueue(&room->emptyClientNum, (void*)temp);
    }

    ++rooms->size;

    return emptyRoomNum;
}

void removeRoom(ROOMS* rooms, const int roomNum)
{
    ROOMINFO* room = &rooms->arr[roomNum];
    int* emptyRoomNum = (int*)malloc(sizeof(int));

    //이름 변경
    strcpy(room->roomName, "EMPTY");
    *emptyRoomNum = roomNum;

    //해당 방의 emptyClientNum 비우기
    while(!isEmpty(&room->emptyClientNum))
    {   
        int* temp = (int*)dequeue(&room->emptyClientNum);
        free(temp);
    }

    //해당 방 번호를 emptyRooms에 넣기
    enqueue(&rooms->emptyRooms, (void*)emptyRoomNum);
    --rooms->size;
}

int readyClient(ROOMS* rooms, int roomNum, int clientNum)
{
    ROOMINFO* room = &rooms->arr[roomNum];
    int result = 0;

    //ready 상태를 반전(0이면 1, 1이면 0)
    room->readyUser[clientNum] = !room->readyUser[clientNum];

    //ready한 유저 확인
    for(int i = 0; i < 4; ++i)
    {
        result += room->readyUser[i];
    }

    if(room->userCount >= 2 && room->userCount == result) return 1;
    else return 0;
}

int connectClientToRoom(ROOMS* rooms, int roomNum, USERINFO* user, SOCKET clientSock)
{
    ROOMINFO* room = &rooms->arr[roomNum];

    //방에서 사용할 클라이언트 번호 부여
    int* clientNum = (int*)dequeue(&room->emptyClientNum);
    int result = *clientNum;

    //클라이언트 정보 저장
    room->clientSocks[*clientNum] = clientSock;
    room->users[*clientNum] = user;
    ++room->userCount;

    free(clientNum);
    return result;
}

void disconnectClientFromRoom(ROOMS* rooms, int roomNum, int clientNum)
{
    ROOMINFO* room = &rooms->arr[roomNum];
    
    room->clientSocks[clientNum] = INVALID_SOCKET;
    room->readyUser[clientNum] = 0;
    --room->userCount;

    if(room->userCount <= 0)
        removeRoom(rooms, roomNum);
}

// int main()
// {
//     ROOMS rooms;
//     USERINFO user = { "user", "user", 0, 0 };
//     USERINFO user1 = { "user1", "user1", 0, 0 };
//     SOCKET sock;

//     initRooms(&rooms);
//     int* roomNum = createRoom(&rooms, "TEST");

//     if(roomNum != NULL)
//     {
//         printf("%s\n", rooms.arr[*roomNum].roomName);
//         printf("%d\n", rooms.size);
//     }

//     int clientNum = connectClientToRoom(&rooms, 0, &user, sock);

//     printf("connect: %d, username: %s\n", clientNum, rooms.arr[0].users[clientNum]->username);

//     clientNum = connectClientToRoom(&rooms, 0, &user1, sock);
//     printf("connect: %d, username: %s\n", clientNum, rooms.arr[0].users[clientNum]->username);

//     printf("ready: %d\n", readyClient(&rooms, 0, 0));
//     printf("ready: %d\n", readyClient(&rooms, 0, 1));
//     printf("ready: %d\n", readyClient(&rooms, 0, 1));

//     disconnectClientFromRoom(&rooms, 0, 0);
//     printf("count: %d\n", rooms.arr[0].userCount);
//     disconnectClientFromRoom(&rooms, 0, 1);
//     printf("count: %d\n", rooms.arr[0].userCount);

//     removeRoom(&rooms, 0);
//     printf("%s\n", rooms.arr[0].roomName);
//     printf("%d\n", rooms.size);

//     return 0;
// }

