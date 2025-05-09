//작성자: 박영진
//내용  : 온라인 도서정보관리 시스템의 클라이언트 구현
//수정일: 2025.05.08
//생성일: 2025.05.08

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "command.h"

char msg[BUF_SIZE];
char recvMsg[BUF_SIZE];

//도서정보관리 시스템에 로그인
int Login(SOCKET);
//프로그램에서 발생한 오류 출력 및 프로그램 종료
void ErrorHandling(char*);

int main()
{
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN serverAddr;
    char serverIp[100];
    char select;

    printf("Input server IP: ");
    gets(serverIp);

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup error!");

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    serverAddr.sin_port = htons(atoi(PORT));

    if(connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    printf("connect!\n");

    while(!Login(sock))
    {
        printf("login failed!\n");
    }

    printf("login!\n");

    closesocket(sock);
    WSACleanup();

    return 0;
}

int Login(SOCKET sock)
{
    int result = 0;
    char username[15];
    char password[20];
    char** splitMsg;

    printf("Input username: ");
    scanf("%s", username);
    printf("Input password: ");
    scanf("%s", password);

    sprintf(msg, "%s/%s/%s", LOGIN, username, password);

    //서버로 로그인 요청
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


void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);

    exit(1);
}