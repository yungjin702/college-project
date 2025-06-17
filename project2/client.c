//작성자: 박영진
//내용  : 온라인 스네이크 게임의 클라이언트 프로그램 구현
//수정일: 2025.06.17
//생성일: 2025.06.15

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "command.h"
#include "snakeGame.h"
#include "graphic.h"

char username[15];
char msg[BUF_SIZE];
char recvMsg[BUF_SIZE];
volatile int isInput;

//사용자 이름, 비밀번호를 입력받고 서버에 로그인 요청
int Login(SOCKET);
//회원가입
void RegisterUser(SOCKET);
//방 생성
void CreateRoom(SOCKET);
//방 목록 확인
void GetRoomList(SOCKET);
//방 생성
void EnterRoom(SOCKET, int);
//방 나가기
void LeaveRoom(PLAYERINFO*);
//방 나가기
void ReadyGame(PLAYERINFO*);
//비밀번호 변경
void ChangePassword(SOCKET);
//회원 탈퇴
int WithdrawUser(SOCKET);
//방에 접속했을 때 사용자의 입력을 처리
unsigned WINAPI InputRoom(void* arg);
//방에 접속했을 때 서버로부터 받는 메시지를 처리
unsigned WINAPI HandleRoomMessage(void* arg);
//프로그램 종료
void ExitProgram(SOCKET);
//프로그램에서 발생한 오류 출력 및 프로그램 종료
void ErrorHandling(char*);

int main()
{
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN serverAddr;

    char serverIp[100];
    char select;

    //콘솔에서 한글이 깨지는 현상을 해결하기 위한 코드
    system("chcp 65001 > nul");

    //커서 제거를 위한 코드
    CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    printf("Input server IP : ");
    gets(serverIp);

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup error!");

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    serverAddr.sin_port = htons(atoi(PORT));

    //온라인 스네이크 게임 서버에 접속
    if(connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    //로그인, 회원가입
    while(1)
    {
        system("cls");

        int result = 0;

        printf("온라인 스네이크 게임\n");
        printf("1. 로그인\n");
        printf("2. 회원가입\n");

        //메뉴 선택
        select = getch();

        switch (select)
        {
        case '1':
            result = Login(sock);
            break;
        case '2':
            RegisterUser(sock);
            break;
        default:
            break;
        }

        if(result) break;
    }

    //메인 메뉴
    while(1)
    {
        system("cls");

        int result = 0;

        printf("온라인 스네이크 게임\n");
        printf("1. 방 생성\n");
        printf("2. 방 입장\n");
        printf("3. 비밀번호 변경\n");
        printf("4. 계정 삭제\n");
        printf("5. 프로그램 종료\n");

        //메뉴 선택
        select = getch();

        switch (select)
        {
        case '1':
            system("cls");
            CreateRoom(sock);
            break;
        case '2':
            system("cls");
            GetRoomList(sock);
            printf("들어갈 방 번호를 입력해주세요: ");
            int num; scanf("%d", &num);
            getchar(); // 입력 버퍼

            if(num >= 0 && num <= 4)
                EnterRoom(sock, num);
            break;
        case '3':
            system("cls");
            ChangePassword(sock);
            break;
        case '4':
            system("cls");
            result = WithdrawUser(sock);
            break;
        case '5':
            ExitProgram(sock);
            break;
        default:
            break;
        }

        if(result) ExitProgram(sock);
    }
}

int Login(SOCKET sock)
{
    int result = 0;
    char password[20];
    char** splitMsg;

    printf("사용자 이름을 입력해주세요: ");
    scanf("%s", username);
    printf("비밀번호를 입력해주세요: ");
    scanf("%s", password);
    getchar(); //입력 버퍼 지우기

    //서버로 로그인 요청
    sprintf(msg, "%s/%s/%s", LOGIN, username, password);
    send(sock, msg, BUF_SIZE, 0);

    //서버로부터 로그인 결과 받기
    if(recv(sock, recvMsg, BUF_SIZE, 0) != 0)
    {
        splitMsg = SplitMessage(recvMsg);

        if(!strcmp(splitMsg[1], "T"))
            result = 1;
    }

    FreeSplitMessage(splitMsg);
    return result;
}

void RegisterUser(SOCKET sock)
{
    char password[20];
    char rePassword[20];
    char** splitMsg;

    printf("생성할 사용자의 이름을 입력하세요: ");
    scanf("%s", username);
    printf("생성할 사용자의 비밀번호를 입력하세요: ");
    scanf("%s", password);
    printf("입력한 비밀번호를 다시 입력하세요: ");
    scanf("%s", rePassword);
    getchar(); //입력 버퍼 지우기

    if(!strcmp(password, rePassword))
    {
        sprintf(msg, "%s/%s/%s", CREATE_USER, username, password);
        send(sock, msg, BUF_SIZE, 0);
        recv(sock, recvMsg, BUF_SIZE, 0);

        splitMsg = SplitMessage(recvMsg);

        if(!strcmp(splitMsg[1], "T"))
            printf("회원가입 성공\n");
        else
            printf("회원가입 실패\n");

        FreeSplitMessage(splitMsg);
    }
    else
    {
         printf("비밀번호가 일치하지 않습니다\n");
    }

    printf("아무 키나 입력해주세요");
    getch();

    return;
}

void CreateRoom(SOCKET sock)
{
    char roomName[16];
    int roomNum;
    char** splitMsg;

    printf("생성할 방의 이름을 입력하세요: ");
    scanf("%s", roomName);
    getchar(); //입력 버퍼 지우기

    sprintf(msg, "%s/%s", CREATE_ROOM, roomName);
    send(sock, msg, BUF_SIZE, 0);
    recv(sock, recvMsg, BUF_SIZE, 0);

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
    {
        //생성된 방 번호 확인
        roomNum = atoi(splitMsg[2]);
        //방 입장
        EnterRoom(sock, roomNum);
        FreeSplitMessage(splitMsg);
    }
    else
        FreeSplitMessage(splitMsg);
}

void GetRoomList(SOCKET sock)
{
    int roomCount;
    char** splitMsg;

    //방 목록 요청
    send(sock, GET_ROOMLIST, BUF_SIZE, 0);
    recv(sock, recvMsg, BUF_SIZE, 0);

    splitMsg = SplitMessage(recvMsg);

    //생성된 방 갯수 확인
    roomCount = atoi(splitMsg[1]);
    FreeSplitMessage(splitMsg);

    if(roomCount == 0)
        printf("생성된 방이 없습니다\n");

    for(int i = 0; i < roomCount; ++i)
    {
        recv(sock, recvMsg, BUF_SIZE, 0);
        splitMsg = SplitMessage(recvMsg);
        printf("[%d] %s(%d/4)\n", atoi(splitMsg[1]), splitMsg[2], atoi(splitMsg[3]));
        FreeSplitMessage(splitMsg);
    }
}

void EnterRoom(SOCKET sock, int roomNum)
{
    HANDLE roomThread;
    PLAYERINFO playerInfo;
    char** splitMsg;

    playerInfo.sock = sock;
    playerInfo.roomNum = roomNum;

    sprintf(msg, "%s/%d/%s", ENTER_ROOM, playerInfo.roomNum, username);
    send(sock, msg, BUF_SIZE, 0);
    recv(sock, recvMsg, BUF_SIZE, 0);

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
    {
        playerInfo.playerNum = atoi(splitMsg[2]);
        FreeSplitMessage(splitMsg);
        roomThread = (HANDLE)_beginthreadex(NULL, 0, HandleRoomMessage, (void*)&playerInfo, 0, NULL);
        WaitForSingleObject(roomThread, INFINITE);
    }
    else
        FreeSplitMessage(splitMsg);
}

void LeaveRoom(PLAYERINFO* playerInfo)
{
    sprintf(msg, "%s/%d/%d", LEAVE_ROOM, playerInfo->roomNum, playerInfo->playerNum);
    send(playerInfo->sock, msg, BUF_SIZE, 0);
}

void ReadyGame(PLAYERINFO* playerInfo)
{
    sprintf(msg, "%s/%d/%d", READY_GAME, playerInfo->roomNum, playerInfo->playerNum);
    send(playerInfo->sock, msg, BUF_SIZE, 0);
}

void ChangePassword(SOCKET sock)
{
    char password[20];
    char rePassword[20];
    char** splitMsg;

    printf("변경할 비밀번호를 입력하세요: ");
    scanf("%s", password);
    printf("입력한 비밀번호를 다시 입력하세요: ");
    scanf("%s", rePassword);
    getchar(); //입력 버퍼 지우기

    if(!strcmp(password, rePassword))
    {
        sprintf(msg, "%s/%s/%s", MODIFY_PW, username, password);
        send(sock, msg, BUF_SIZE, 0);
        recv(sock, recvMsg, BUF_SIZE, 0);

        splitMsg = SplitMessage(recvMsg);

        if(!strcmp(splitMsg[1], "T"))
            printf("변경 성공\n");
        else
            printf("변경 실패\n");
    }
    else
    {
         printf("비밀번호가 일치하지 않습니다\n");
    }

    FreeSplitMessage(splitMsg);
    printf("아무 키나 입력해주세요");
    getch();
}

int WithdrawUser(SOCKET sock)
{
    char input;
    int result;
    char** splitMsg;

    printf("탈퇴를 원하시면 '1'을 입력해주세요: ");
    scanf("%c", &input);
    getchar(); //입력 버퍼 지우기

    if(input == '1')
    {
        sprintf(msg, "%s/%s", DELETE_USER, username);
        send(sock, msg, BUF_SIZE, 0);
        recv(sock, recvMsg, BUF_SIZE, 0);

        splitMsg = SplitMessage(recvMsg);

        if(!strcmp(splitMsg[1], "T"))
        {
            printf("탈퇴 성공\n");
            result = 1;
        }
        else
        {
            printf("탈퇴 실패\n");
            result = 0;
        }
    }
    else
        return 0;

    FreeSplitMessage(splitMsg);
    printf("아무 키나 입력해주세요");
    getch();

    return result;
}

unsigned WINAPI InputRoom(void* arg)
{
    PLAYERINFO* playerInfo = ((PLAYERINFO*)arg);
    int ready = 0;

    while(isInput)
    {
        if(kbhit())
        {   
            char ch = getch();
            
            switch (ch)
            {
            case 27:        //ESC키
                LeaveRoom(playerInfo);
                isInput = 0;
                break;
            case 32:        //SPACE키
                ReadyGame(playerInfo);
                ready = !ready;
                gotoxy(0, 5);

                if(ready)
                    printf("%s", "다른 플레이어를 기다리는 중");
                else
                    removeLine(0, 5, 40);
                break;
            default:
                break;
            }
        }
    }

    return 0;
}

unsigned WINAPI HandleRoomMessage(void* arg)
{
    system("cls");

    PLAYERINFO playerInfo = *((PLAYERINFO*)arg);
    char userNames[4][15];
    int strLen;
    char** splitMsg;

    isInput = 1;
    //InputRoom 함수를 스레드로 실행
    _beginthreadex(NULL, 0, InputRoom, arg, 0, NULL);

    while((strLen = recv(playerInfo.sock, recvMsg, BUF_SIZE, 0)) != -1)
    {
        //받은 메시지 분리
        splitMsg = SplitMessage(recvMsg);

        //방에 있는 클라이언트의 정보가 변경되었을 때
        if(!strcmp(splitMsg[0], UPDATED_ROOM))
        {
            FreeSplitMessage(splitMsg);

            for(int i = 0; i < 4; ++i)
            {
                removeLine(0, i, 50);
                gotoxy(0, i);

                recv(playerInfo.sock, recvMsg, BUF_SIZE, 0);
                splitMsg = SplitMessage(recvMsg);
                strcpy(userNames[atoi(splitMsg[1])], splitMsg[2]);

                if(strcmp(splitMsg[2], "EMPTY"))
                    printf("%d: %s/승리: %s회/평균: %s점", i + 1, splitMsg[2], splitMsg[3], splitMsg[4]);
                else
                    printf("%d: %s", i, splitMsg[2]);

                FreeSplitMessage(splitMsg);
            }
        }
        else if(!strcmp(splitMsg[0], LEAVE_ROOM))
        {
            FreeSplitMessage(splitMsg);
            break;
        }
        else if(!strcmp(splitMsg[0], GAME_START))
        {
            extern int printX[];

            //inputThread 종료
            isInput = 0;

            //사용자 이름 출력
            system("cls");
            for(int i = 0; i < 4; ++i)
            {
                gotoxy(printX[i], 2);
                printf("%s", userNames[i]);
            }

            //스네이크 게임 실행
            _beginthreadex(NULL, 0, snakeGame, arg, 0, NULL);

            FreeSplitMessage(splitMsg);
        }
        else if(!strcmp(splitMsg[0], NEXT_DIRE))
        {
            move(atoi(splitMsg[1]), atoi(splitMsg[2]));
            FreeSplitMessage(splitMsg);
        }
        else if(!strcmp(splitMsg[0], CREATE_APPLE))
        {
            updateApplePos(atoi(splitMsg[1]), atoi(splitMsg[2]), atoi(splitMsg[3]));
            FreeSplitMessage(splitMsg);
        }
        else if(!strcmp(splitMsg[0], GAME_RESULT))
        {
            gotoxy(0, 22);
            if(!strcmp(splitMsg[1], "DRAW"))
                printf("%s!\n", splitMsg[1]);
            else
                printf("%s %s!\n", splitMsg[2], splitMsg[1]);

            FreeSplitMessage(splitMsg);

            printf("아무 키를 누르고 메인 화면으로 돌아가기");
            getch();
            break;
        } 
    }
    
    return 0;
}

void ExitProgram(SOCKET sock)
{
    send(sock, LOGOUT, BUF_SIZE, 0);
    closesocket(sock);
    WSACleanup();
    exit(0);
}

void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);

    exit(1);
}