//작성자: 박영진
//내용  : 온라인 도서정보관리 시스템의 클라이언트 구현
//수정일: 2025.05.17
//생성일: 2025.05.08

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "command.h"

//현재 접속한 클라이언트가 admin인지 확인하는 변수
int IsAdmin = 0;

char msg[BUF_SIZE];
char recvMsg[BUF_SIZE];

//사용자 이름, 비밀번호를 입력받고 서버에 로그인 요청
int Login(SOCKET);
//도서정보관리 시스템에 존재하는 도서 정보 검색
void SearchBook(SOCKET);
//도서정보관리 시스템에 도서 정보 추가
void CreateBook(SOCKET);
//도서정보관리 시스템에 존재하는 도서의 도서정보를 변경
void ModifyBook(SOCKET);
//도서정보관리 시스템에 존재하는 도서의 도서를 삭제
void DeleteBook(SOCKET);
//도서정보관리 시스템에 존재하는 도서를 평점을 기준으로 내림차순으로 정보를 출력
void ShowTopRatedBooks(SOCKET);
//도서정보관리 시스템에 사용자 추가
void CreateUser(SOCKET);
//도서정보관리 시스템에 사용자 삭제
void DeleteUser(SOCKET);
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

    //서버 IP 입력
    printf("Input server IP: ");
    gets(serverIp);

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup error!");

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    serverAddr.sin_port = htons(atoi(PORT));

    //도서정보관리 시스템의 서버에 접속
    if(connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    printf("connect!\n");

    //로그인 시도
    while(!Login(sock))
        printf("login failed!\n");

    //도서정보시스템 메인 메뉴
    while(1)
    {
        printf("----------------------------\n");
        printf("1. 도서 검색\n");
        printf("2. 도서 추가\n");
        printf("3. 도서 정보 변경\n");
        printf("4. 도서 삭제\n");
        printf("5. 평점 순 도서 랭킹\n");

        //접속한 사용자가 admin인 경우에 따라 메뉴를 다르게 출력
        if(IsAdmin)
        {
            printf("6. 사용자 생성\n");
            printf("7. 사용자 삭제\n");
            printf("8. 프로그램 종료\n");
        }
        else
        {
            printf("6. 프로그램 종료\n");
        }
        printf("----------------------------\n");

        //메뉴 선택
        select = getch();

        switch (select)
        {
        case '1':
            SearchBook(sock);
            break;
        case '2':
            CreateBook(sock);
            break;
        case '3':
            ModifyBook(sock);
            break;
        case '4':
            DeleteBook(sock);
            break;
        case '5':
            ShowTopRatedBooks(sock);
            break;
        case '6':
            if(IsAdmin)
                CreateUser(sock);
            else
                ExitProgram(sock);
            break;
         case '7':
            if(!IsAdmin) break;

            DeleteUser(sock);
            break;
        case '8':
            if(!IsAdmin) break;

            ExitProgram(sock);
            break;
        default:
            break;
        }
    }

    //ExitProgram()를 통해 프로그램이 종료되기 때문에 작동되지 않는 코드
    //혹시 모를 상황을 위해 코드 작성
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
    getchar(); //입력 버퍼 지우기

    //서버로 로그인 요청
    sprintf(msg, "%s/%s/%s", LOGIN, username, password);
    send(sock, msg, BUF_SIZE, 0);

    //서버로부터 로그인 결과 받기
    if(recv(sock, recvMsg, BUF_SIZE, 0) != 0)
    {
        splitMsg = SplitMessage(recvMsg);

        if(!strcmp(splitMsg[1], "T"))
        {
            //접속한 사용자가 admin인 경우
            if(!strcmp(username, "admin"))
                IsAdmin = 1;

            result = 1;
        }
    }

    FreeSplitMessage(splitMsg);
    return result;
}

void SearchBook(SOCKET sock)
{
    char type = '0';
    char input[100];
    int findBookCount;
    char** splitMsg;

    while(type != '1' && type != '2')
    {
        printf("----------도서 검색----------\n");
        printf("1. 도서 제목을 통한 검색\n");
        printf("2. 저자명을 통한 검색\n");

        //printf("선택할 번호: ");
        type = getch();
    }

    if(type == '1')
        printf("검색할 도서 제목을 입력하세요: ");
    else
        printf("검색할 저자명을 입력하세요: ");

    gets(input);

    //서버에게 도서 검색 요청
    sprintf(msg, "%s/%c/%s", FIND_BOOK, type, input);
    send(sock, msg, BUF_SIZE, 0);

    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    //서버로부터 일치하는 도서 정보 수 받기
    splitMsg = SplitMessage(recvMsg);
    findBookCount = atoi(splitMsg[1]);
    FreeSplitMessage(splitMsg);

    if(findBookCount > 0)
        printf("-----일치하는 도서-----\n");
    else
        printf("-----일치하는 도서가 없습니다-----\n");

    //서버로부터 도서 정보 받아오기
    for(int i = 0; i < findBookCount; ++i)
    {
        if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
            ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

        splitMsg = SplitMessage(recvMsg);
        printf("도서 제목: %s\t저자: %s\t평점: %s\n", splitMsg[1], splitMsg[2], splitMsg[3]);
        FreeSplitMessage(splitMsg);
    }
}

void CreateBook(SOCKET sock)
{
    char bookTitle[100];
    char author[50];
    char rating[5];
    char** splitMsg;

    printf("----------도서 추가----------\n");
    printf("추가할 도서 제목을 입력하세요: ");
    gets(bookTitle);
    printf("추가할 도서의 저자명을 입력하세요: ");
    gets(author);
    printf("추가할 도서의 평점을 입력하세요: ");
    scanf("%s", rating);
    getchar(); //입력 버퍼 지우기

    //서버에게 도서 추가 요청
    sprintf(msg, "%s/%s/%s/%s", CREATE_BOOK, bookTitle, author, rating);
    send(sock, msg, BUF_SIZE, 0);
    
    //요청 결과 확인
    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
        printf("정보 추가 성공\n");
    else
        printf("정보 추가 실패\n");

    FreeSplitMessage(splitMsg);
}

void ModifyBook(SOCKET sock)
{
    char input[100];
    char rBookTitle[100];
    char rAuthor[50];
    float rRating;
    char** splitMsg;

    printf("----------도서 정보 변경----------\n");
    printf("원래 도서 제목을 입력하세요: ");
    gets(input);
    printf("변경할 도서의 도서 제목을 입력하세요: ");
    gets(rBookTitle);
    printf("변경할 도서의 저자명을 입력하세요: ");
    gets(rAuthor);
    printf("변경할 도서의 평점을 입력하세요: ");
    scanf("%f", &rRating);
    getchar(); //입력 버퍼 지우기

    //서버에게 도서 정보 변경 요청
    sprintf(msg, "%s/%s/%s/%s/%.2f", MODIFY_BOOK, input, rBookTitle, rAuthor, rRating);
    send(sock, msg, BUF_SIZE, 0);

    //요청 결과 확인
    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
        printf("정보 변경 성공\n");
    else
        printf("정보 변경 실패\n");

    FreeSplitMessage(splitMsg);
}

void DeleteBook(SOCKET sock)
{
    char bookTitle[100];
    char** splitMsg;

    printf("----------도서 삭제----------\n");
    printf("삭제할 도서의 도서 제목을 입력하세요: ");
    gets(bookTitle);

    //서버에게 도서 삭제 요청
    sprintf(msg, "%s/%s", DELETE_BOOK, bookTitle);
    send(sock, msg, BUF_SIZE, 0);

    //요청 결과 확인
    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
        printf("도서 삭제 성공\n");
    else
        printf("도서 삭제 실패\n");

    FreeSplitMessage(splitMsg);
}

void ShowTopRatedBooks(SOCKET sock)
{
    int maxRating;
    char** splitMsg;

    printf("----------평점 순 도서 랭킹----------\n");
    printf("1등부터 몇 등까지 보길 원하십니까?: ");
    scanf("%d", &maxRating);
    getchar(); //입력 버퍼 지우기

    sprintf(msg, "%s/%d", RANKED_BOOK, maxRating);
    send(sock, msg, BUF_SIZE, 0);

    //서버로부터 1등 ~ maxRaing등까지의 도서 정보 받기 및 출력
    for(int i = 0; i < maxRating; ++i)
    {
        if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
            ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

        splitMsg = SplitMessage(recvMsg);
        printf("순위: %-3d\t도서 제목: %s\t저자: %s\t평점: %s\n", i + 1, splitMsg[1], splitMsg[2], splitMsg[3]);
        FreeSplitMessage(splitMsg);
    }
}

void CreateUser(SOCKET sock)
{
    char username[15];
    char password[20];
    char** splitMsg;

    printf("----------사용자 추가----------\n");
    printf("생성할 사용자의 이름을 입력하세요: ");
    scanf("%s", username);
    printf("생성할 사용자의 비밀번호를 입력하세요: ");
    scanf("%s", password);
    getchar(); //입력 버퍼 지우기

    //서버에게 사용자 생성 요청
    sprintf(msg, "%s/%s/%s", CREATE_USER, username, password);
    send(sock, msg, BUF_SIZE, 0);

    //요청 결과 확인
    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
        printf("사용자 생성 성공\n");
    else
        printf("사용자 생성 실패\n");

    FreeSplitMessage(splitMsg);
}

void DeleteUser(SOCKET sock)
{
    char username[15];
    int userCount;
    char** splitMsg;

    printf("----------등록된 사용자----------\n");

    //등록된 사용자들의 이름 요청
    send(sock, REGISTERED_USER, BUF_SIZE, 0);

    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    //서버로부터 등록된 사용자 수 저장
    splitMsg = SplitMessage(recvMsg);
    userCount = atoi(splitMsg[1]);
    FreeSplitMessage(splitMsg);

    //서버로부터 받은 등록된 사용자들의 이름 출력
    for(int i = 0; i < userCount; ++i)
    {
        if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
            ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

        splitMsg = SplitMessage(recvMsg);
        printf("%d: %s\n", i + 1, splitMsg[1]);
        FreeSplitMessage(splitMsg);
    }

    printf("삭제할 사용자의 이름을 입력하세요: ");
    scanf("%s", username);
    getchar(); //입력 버퍼 지우기

    //서버에게 사용자 삭제 요청
    sprintf(msg, "%s/%s", DELETE_USER, username);
    send(sock, msg, BUF_SIZE, 0);

    //요청 결과 확인
    if(recv(sock, recvMsg, BUF_SIZE, 0) == -1)
        ErrorHandling("서버 통신 오류. 강제 종료 합니다.\n");

    splitMsg = SplitMessage(recvMsg);

    if(!strcmp(splitMsg[1], "T"))
        printf("사용자 삭제 성공\n");
    else
        printf("사용자 삭제 실패\n");
    
    FreeSplitMessage(splitMsg);
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