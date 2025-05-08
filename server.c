//작성자: 박영진
//내용  : 온라인 도서정보관리 시스템의 서버 구현
//수정일: 2025.05.07
//생성일: 2025.05.07

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include "users.h"
#include "books.h"
#include "command.h"

//파일에 있는 데이터를 원하는 자료형에 맞게 불러오는 함수
int loadDataList(const char*, void*, const size_t);
//저장하고자 하는 데이터를 원하는 자료형에 맞게 파일에 저장하는 함수
int saveDataList(const char*, void*, const size_t);
//서버에 접속한 클라이언트의 요청을 처리하는 함수
unsigned WINAPI HandlingClient(void*);
//프로그램에서 발생한 오류 출력 및 프로그램 종료
void ErrorHandling(char*);

int main()
{
    WSADATA wsaData;
    SOCKET serverSock, clientSock;
    SOCKADDR_IN serverAddr;
    SOCKADDR_IN clientAddr;
    int clientAddrSize;

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
        clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &clientAddrSize);
        _beginthreadex(NULL, 0, HandlingClient, (void*)&clientSock, 0, NULL);
        printf("Connected Client IP: %s\n", inet_ntoa(clientAddr.sin_addr));
    }

    closesocket(serverSock);
    WSACleanup();

    return 0;
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
        const char *delimiter = (dataType == sizeof(BOOKINFO)) ? "\t" : "//";
        char *split = strtok(buffer, delimiter);

        if(dataType == sizeof(BOOKINFO))
        {
            BOOKINFO book;
            book.num = atoi(split);
            split = strtok(NULL, delimiter);
            strcpy(book.bookTitle, split);
            split = strtok(NULL, delimiter);
            strcpy(book.author, split);
            split = strtok(NULL, delimiter);
            book.rating = atof(split);

            //테스트용
            //printf("%d\t%s\t%s\t%.2f\n", book.num, book.bookTitle, book.author, book.rating);
    
            addBook((BOOKS*)data, book);
        }
        else
        {
            USERINFO user;
            strcpy(user.username, split);
            split = strtok(NULL, delimiter);
            strcpy(user.password, split);
            user.password[strlen(user.password) - 1] = '\0';

            //테스트용
            //printf("%s\t%s\n", user.username, user.password);

            addUser((USERS*)data, user);
        }
    }

    fclose(file);

    return 1;
}

int saveDataList(const char* filePath, void* data, const size_t dataType)
{
    FILE *file = fopen(filePath, "w");
    
    if(dataType == sizeof(BOOKINFO))
    {
        for(int i = 0; i < ((BOOKS*)data)->size; ++i)
        {
            const BOOKINFO *book = &((BOOKS*)data)->arr[i];

            //만약 제대로 저장되지 않은 경우 0을 반환하도록 함
            if(fprintf(file, "%d\t%s\t%s\t%.2f\n",
                i + 1, book->bookTitle, book->author, book->rating) == 0)
            return 0;
        }
    }
    else
    {
        for(int i = 0; i < ((USERS*)data)->size; ++i)
        {
            const USERINFO *user = &((USERS*)data)->arr[i];

            //만약 제대로 저장되지 않은 경우 0을 반환하도록 함
            if(fprintf(file, "%s//%s\n",
                user->username, user->password) == 0)
            return 0;
        }
    }

    return 1;
}

unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clientSock = *((SOCKET*)arg);
    char msg[BUF_SIZE];
    int strLen;
    char** splitMsg;

    while((strLen = recv(clientSock, msg, sizeof(msg), 0)) != 0)
    {
        splitMsg = SplitMessage(msg);

        printf("msg: %s\n", splitMsg[0]);
    }
}

void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);

    exit(1);
}
