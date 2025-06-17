//작성자: 박영진
//내용  : 온라인 스네이크 게임의 서버 프로그램 구현
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include "linkedList.h"
#include "command.h"
#include "users.h"
#include "rooms.h"

ROOMS rooms;
USERS users;

HANDLE hRoomMutex;
HANDLE hUserMutex;

//파일에 있는 데이터를 원하는 자료형에 맞게 불러오는 함수
int loadDataList(const char*, void*, const size_t);
//저장하고자 하는 데이터를 원하는 자료형에 맞게 파일에 저장하는 함수
int saveDataList(const char*, void*, const size_t);
//방에 있는 모든 클라이언트에게 사용자 정보를 전송
void SendUserInfo(int);
//방에 있는 모든 클라이언트에게 사용자 정보를 전송
void SendGameStart(int);
//서버에 접속한 클라이언트의 요청을 처리하는 함수
unsigned WINAPI HandlingClient(void*);
//서버에 접속한 클라이언트의 요청을 처리하는 함수
unsigned WINAPI HandleClientInRoom(void*);
//게임 결과 처리하는 함수
int GameResult(int);
//프로그램에서 발생한 오류 출력 및 프로그램 종료
void ErrorHandling(char*);

int main()
{
    WSADATA wsaData;
    SOCKET serverSock, *clientSock;
    SOCKADDR_IN serverAddr, clientAddr;
    int clientAddrSize;

    initRooms(&rooms);
    users.list = NULL;
    users.size = 0;

    //뮤텍스 생성
    hRoomMutex = CreateMutex(NULL, 0, NULL);
    hUserMutex = CreateMutex(NULL, 0, NULL);

    if(!loadDataList("file/users.txt", (void*)&users, sizeof(USERINFO)))
    {
        ErrorHandling("file/users.txt 경로에 파일이 존재하지 않습니다.");
    }

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    serverSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(PORT));

    if(bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if(listen(serverSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    printf("server Start\n");

    while(1)
    {
        clientAddrSize = sizeof(clientAddr);
        clientSock = (SOCKET*)malloc(sizeof(SOCKET));
        *clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &clientAddrSize);
        _beginthreadex(NULL, 0, HandlingClient, (void*)clientSock, 0, NULL);
        printf("Connected Client IP: %s\n", inet_ntoa(clientAddr.sin_addr));
    }
}

int loadDataList(const char* filePath, void* data, const size_t dataType)
{
    FILE *file = fopen(filePath, "r");
    char buffer[300]; 

    //파일을 열 수 없는 경우
    if(file == NULL) return 0;
    
    while(fgets(buffer, sizeof(buffer), file))
    {
        //strtok에서 사용할 구분자 지정
        const char *delimiter = "/";
        char *split = strtok(buffer, delimiter);

        USERINFO* user = (USERINFO*)malloc(sizeof(USERINFO));
        strcpy(user->username, split);
        split = strtok(NULL, delimiter);

        strcpy(user->password, split);
        split = strtok(NULL, delimiter);

        user->winCount = atoi(split);
        split = strtok(NULL, delimiter);

        user->meanScore = atoi(split);

        addUser((USERS*)data, user);
    }

    fclose(file);

    return 1;
}

int saveDataList(const char* filePath, void* data, const size_t dataType)
{
    FILE *file = fopen(filePath, "w");

    NODE* current = ((USERS*)data)->list;

    while(current != NULL)
    {
        USERINFO* data = (USERINFO*)current->data;

        if(fprintf(file, "%s/%s/%d/%d\n", 
                data->username, data->password, data->winCount, data->meanScore) == 0)
            return 0;

        current = current->next;
    }

    fclose(file);

    return 1;
}

void SendUserInfo(int roomNum)
{
    ROOMINFO* room = &rooms.arr[roomNum];
    char msg[BUF_SIZE];
    int isUserExist;

    for(int i = 0; i < 4; ++i)
    {
        if(room->clientSocks[i] == INVALID_SOCKET) continue;
        send(room->clientSocks[i], UPDATED_ROOM, BUF_SIZE, 0);

        for(int j = 0; j < 4; ++j)
        {
            isUserExist = (room->clientSocks[j] != INVALID_SOCKET);
            
            if(isUserExist)
                sprintf(msg, "%s/%d/%s/%d/%d", 
                    USER_INFO, j, room->users[j]->username, room->users[j]->winCount, room->users[j]->meanScore);
            else
                sprintf(msg, "%s/%d/%s", USER_INFO, j, "EMPTY");

            send(room->clientSocks[i], msg, BUF_SIZE, 0);
        }
    }
}

void SendGameStart(int roomNum)
{
    for(int i = 0; i < 4; ++i)
    {
        if(rooms.arr[roomNum].clientSocks[i] == INVALID_SOCKET) continue;
        send(rooms.arr[roomNum].clientSocks[i], GAME_START, BUF_SIZE, 0);
    }
}

unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clientSock = *((SOCKET*)arg);
    char msg[BUF_SIZE];
    int strLen;
    char** splitMsg;

    while((strLen = recv(clientSock, msg, BUF_SIZE, 0)) != -1)
    {
        //클라이언트가 보낸 메시지 출력
        printf("Handling msg: %s\n", msg);

        //받은 메시지 분리
        splitMsg = SplitMessage(msg);

        //로그인 요청인 경우
        if(!strcmp(splitMsg[0], LOGIN))
        {
            WaitForSingleObject(hUserMutex, INFINITE);

            USERINFO* user = findUser(&users, splitMsg[1]);
            int result = 0;

            if(user == NULL) result = 0;
            else
                result = !strcmp(user->password, splitMsg[2]) ? 1 : 0;

            ReleaseMutex(hUserMutex);

            //로그인 결과를 클라이언트에게 전송
            sprintf(msg, "%s/%s", LOGIN, result ? "T" : "F");
            send(clientSock, msg, BUF_SIZE, 0);
        }
        //로그아웃 요청인 경우
        else if(!strcmp(splitMsg[0], LOGOUT))
        {
            FreeSplitMessage(splitMsg);
            break;
        }
        //회원가입 요청인 경우
        else if(!strcmp(splitMsg[0], CREATE_USER))
        {
            WaitForSingleObject(hUserMutex, INFINITE);

            USERINFO* user = findUser(&users, splitMsg[1]);
            int result = 0;

            //같은 사용자 이름을 가진 사용자가 없는 경우 사용자 생성
            if(user == NULL)
            {
                user = (USERINFO*)malloc(sizeof(USERINFO));
                strcpy(user->username, splitMsg[1]);
                strcpy(user->password, splitMsg[2]);
                user->winCount = 0;
                user->meanScore = 0;

                result = addUser(&users, user);
                result = result && saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));
            }

            sprintf(msg, "%s/%c", CREATE_USER, (result ? 'T' : 'F'));

            ReleaseMutex(hUserMutex);

            send(clientSock, msg, BUF_SIZE, 0);
        }
        //비밀번호 변경 요청인 경우
        else if(!strcmp(splitMsg[0], MODIFY_PW))
        {
            WaitForSingleObject(hUserMutex, INFINITE);
            
            int result = modifyPassword(&users, splitMsg[1], splitMsg[2]);

            if(result)
                result = result && saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

            sprintf(msg, "%s/%c", MODIFY_PW, (result ? 'T' : 'F'));

            ReleaseMutex(hUserMutex);

            send(clientSock, msg, BUF_SIZE, 0);
        }
        //회원 탈퇴 요청인 경우
        else if(!strcmp(splitMsg[0], DELETE_USER))
        {
            WaitForSingleObject(hUserMutex, INFINITE);
            
            int result = 0;
            result = deleteUser(&users, splitMsg[1]);

            if(result)
                result = result && saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

            sprintf(msg, "%s/%c", DELETE_USER, (result ? 'T' : 'F'));

            ReleaseMutex(hUserMutex);

            send(clientSock, msg, BUF_SIZE, 0);
        }
        //방 목록 요청인 경우
        else if(!strcmp(splitMsg[0], GET_ROOMLIST))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);

            sprintf(msg, "%s/%d", GET_ROOMLIST, rooms.size);
            send(clientSock, msg, BUF_SIZE, 0);

            for(int i = 0; i < 5; ++i)
            {
                if(!strcmp(rooms.arr[i].roomName, "EMPTY")) continue;
                sprintf(msg, "%s/%d/%s/%d", ROOM_INFO, i, rooms.arr[i].roomName, rooms.arr[i].userCount);
                send(clientSock, msg, BUF_SIZE, 0);
            }

            ReleaseMutex(hRoomMutex);
        }
        //방 생성 요청인 경우
        else if(!strcmp(splitMsg[0], CREATE_ROOM))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);
            
            int* roomNum = createRoom(&rooms, splitMsg[1]);

            //roomNum이 NULL인 경우는 방을 생성할 수 없는 경우
            sprintf(msg, "%s/%c/%d", CREATE_ROOM, (roomNum != NULL ? 'T' : 'F'), (roomNum == NULL ? -1 : *roomNum));

            ReleaseMutex(hRoomMutex);
            send(clientSock, msg, BUF_SIZE, 0);
            free(roomNum);
        }
        //방 접속 요청인 경우
        else if(!strcmp(splitMsg[0], ENTER_ROOM))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);
            WaitForSingleObject(hUserMutex, INFINITE);

            int roomNum = atoi(splitMsg[1]);
            int clientNum;

            //들어가려고 하는 방이 만들어지지 않은 방이 아니고 4명 미만인 경우 입장
            if(strcmp(rooms.arr[roomNum].roomName, "EMPTY") && rooms.arr[roomNum].userCount < 4) 
            {
                //클라이언트 번호 부여 받기
                clientNum = connectClientToRoom(&rooms, roomNum, findUser(&users, splitMsg[2]), clientSock);
                
                //메시지 전송
                sprintf(msg, "%s/%c/%d", ENTER_ROOM, 'T', clientNum);
                send(clientSock, msg, BUF_SIZE, 0);

                _beginthreadex(NULL, 0, HandleClientInRoom, arg, 0, NULL);

                //방에 있는 모든 클라이언트에게 사용자 정보 전송
                SendUserInfo(roomNum);

                ReleaseMutex(hRoomMutex);
                ReleaseMutex(hUserMutex);

                FreeSplitMessage(splitMsg);
                break;
            }
            else
            {
                ReleaseMutex(hRoomMutex);
                ReleaseMutex(hUserMutex);
                
                //메시지 전송
                sprintf(msg, "%s/%c/%d", ENTER_ROOM, 'F', -1);
                send(clientSock, msg, BUF_SIZE, 0);
            }
        }

        //동적 할당된 메모리 해제
        FreeSplitMessage(splitMsg);
    }

    if(strLen == -1)
    {   
        printf("Disconnected Client\n");
        closesocket(clientSock);
        free(arg);
    }
    
    return 0;
}

unsigned WINAPI HandleClientInRoom(void* arg)
{
    SOCKET clientSock = *((SOCKET*)arg);
    char msg[BUF_SIZE];
    int strLen;
    char** splitMsg;

    while((strLen = recv(clientSock, msg, BUF_SIZE, 0)) != -1)
    {
        //클라이언트가 보낸 메시지 출력
        printf("Room msg: %s\n", msg);

        //받은 메시지 분리
        splitMsg = SplitMessage(msg);

        //방 나가기 요청인 경우
        if(!strcmp(splitMsg[0], LEAVE_ROOM))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);

            //방 나가기 처리 후 방에 남아있는 클라이언트에게 유저 정보 전송
            send(clientSock, LEAVE_ROOM, BUF_SIZE, 0);
            disconnectClientFromRoom(&rooms, atoi(splitMsg[1]), atoi(splitMsg[2]));
            SendUserInfo(atoi(splitMsg[1]));

            ReleaseMutex(hRoomMutex);

            FreeSplitMessage(splitMsg);

            _beginthreadex(NULL, 0, HandlingClient, arg, 0, NULL);

            break;
        }
        //게임 준비 요청인 경우
        else if(!strcmp(splitMsg[0], READY_GAME))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);

            //준비 상태 확인 후 모든 플레이어가 레디 상태면 게임 시작 메시지 전송
            if(readyClient(&rooms, atoi(splitMsg[1]), atoi(splitMsg[2])))
                SendGameStart(atoi(splitMsg[1]));

            ReleaseMutex(hRoomMutex);
        }
        //다음 위치 이동 및 사과 위치 생성인 경우
        else if(!strcmp(splitMsg[0], NEXT_DIRE) || !strcmp(splitMsg[0], CREATE_APPLE))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);
            ROOMINFO* room = &rooms.arr[atoi(splitMsg[1])];

            //전송 받은 메시지 재가공
            if(!strcmp(splitMsg[0], NEXT_DIRE))
                sprintf(msg, "%s/%s/%s", NEXT_DIRE, splitMsg[2], splitMsg[3]);
            else
                sprintf(msg, "%s/%s/%s/%s", CREATE_APPLE, splitMsg[2], splitMsg[3], splitMsg[4]);   

            for(int i = 0; i < 4; ++i)
            {
                if(room->clientSocks[i] == INVALID_SOCKET) continue;
                //자기 자신이면 보내지 않음
                if(i == atoi(splitMsg[2])) continue;

                send(room->clientSocks[i], msg, BUF_SIZE, 0);
            }

            ReleaseMutex(hRoomMutex);
        }
        //클라이언트가 게임 오버된 경우
        else if(!strcmp(splitMsg[0], GAME_OVER))
        {
            WaitForSingleObject(hRoomMutex, INFINITE);
            ROOMINFO* room = &rooms.arr[atoi(splitMsg[1])];
            int result;

            //게임 오버된 클라이언트 수 증가 및 점수 저장
            ++room->overCount;
            room->scores[atoi(splitMsg[2])] = atoi(splitMsg[3]);

            //만약 모든 클라이언트가 게임 오버라면 게임 결과 처리 및 메시지 전송
            if(room->overCount == room->userCount)
            {
                result = GameResult(atoi(splitMsg[1]));

                if(result == -1)
                        sprintf(msg, "%s/%s", GAME_RESULT, "DRAW");
                    else
                        sprintf(msg, "%s/%s/%s", GAME_RESULT, "WIN", room->users[result]->username);

                for(int i = 0; i < 4; ++i)
                {
                    if(room->clientSocks[i] == INVALID_SOCKET) continue;
                    send(room->clientSocks[i], msg, BUF_SIZE, 0);
                }

                removeRoom(&rooms, atoi(splitMsg[1]));
            }

            FreeSplitMessage(splitMsg);
            ReleaseMutex(hRoomMutex);

            _beginthreadex(NULL, 0, HandlingClient, arg, 0, NULL);

            break;
        }

        FreeSplitMessage(splitMsg);
    }

    if(strLen == -1)
    {   
        printf("Disconnected Client\n");
        closesocket(clientSock);
        free(arg);
    }

    return 0;
}

int GameResult(int roomNum)
{
    ROOMINFO* room = &rooms.arr[roomNum];
    int isDraw = 0;
    int max = 0;

    //승리, 무승부 여부 확인 및 가장 높은 점수를 얻은 클라이언트 탐색
    for(int i = 1; i < 4; ++i)
    {
        if(room->scores[max] == room->scores[i])
            isDraw = 1;
        else if(room->scores[max] < room->scores[i])
        {
            isDraw = 0;
            max = i;
        }
    }

    //사용자 정보 업데이트
    for(int i = 0; i < 4; ++i)
    {
        if(room->clientSocks[i] == INVALID_SOCKET) continue;

        room->users[i]->meanScore = room->users[i]->meanScore == 0 ? room->users[i]->meanScore 
                                        : (room->users[i]->meanScore + room->scores[i]) / 2;
        if(i == max && !isDraw)
            room->users[i]->winCount++;
    }

    saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

    if(isDraw) return -1;
    else return max;
}

void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);

    exit(1);
}

