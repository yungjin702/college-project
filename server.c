//작성자: 박영진
//내용  : 온라인 도서정보관리 시스템의 서버 구현
//수정일: 2025.05.09
//생성일: 2025.05.07

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include "users.h"
#include "books.h"
#include "command.h"

BOOKS books;
USERS users;

HANDLE hBookMutex;
HANDLE hUserMutex;

//파일에 있는 데이터를 원하는 자료형에 맞게 불러오는 함수
int loadDataList(const char*, void*, const size_t);
//저장하고자 하는 데이터를 원하는 자료형에 맞게 파일에 저장하는 함수
int saveDataList(const char*, void*, const size_t);
//평점 순 정렬 시 qsort에서 사용할 compare 함수
int compareByRate(const void* a, const void* b);
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

    //구조체 초기화
    books.size = 0;
    users.size = 0;

    //파일데이터 불러오기
    loadDataList("file/booklist2.txt", (void*)&books, sizeof(BOOKINFO));
    loadDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

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

    fclose(file);

    return 1;
}

int compareByRate(const void* a, const void* b)
{
    if(books.arr[*(int*)a].rating < books.arr[*(int*)b].rating)
        return 1;
    else if(books.arr[*(int*)a].rating == books.arr[*(int*)b].rating)
        return 0;
    else
        return -1;
}

unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clientSock = *((SOCKET*)arg);
    char msg[BUF_SIZE];
    int strLen;
    char** splitMsg;

    while((strLen = recv(clientSock, msg, BUF_SIZE, 0)) != 0)
    {
        printf("msg: %s\n", msg);
        splitMsg = SplitMessage(msg);

        //로그인 요청일 경우
        if(!strcmp(splitMsg[0], LOGIN))
        {
            WaitForSingleObject(hUserMutex, INFINITE);

            //일치하는 사용자 정보가 있는지 확인, 로그인 결과를 클라이언트에게 전송
            sprintf(msg, "%s/%s", LOGIN, (findUser(&users, splitMsg[1], splitMsg[2]) ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hUserMutex);
        }
        //로그아웃 요청일 경우 무한루프 탈출
        else if(!strcmp(splitMsg[0], LOGOUT))
        {
            break;
        }
        //도서 검색 요청일 경우
        else if(!strcmp(splitMsg[0], FIND_BOOK))
        {
            WaitForSingleObject(hBookMutex, INFINITE);

            //일치하는 도서 정보 찾기
            int* result = findBooks(&books, splitMsg[1], splitMsg[2]);
            int resultCount = 0;

            //일치하는 도서 수 확인
            while(result[resultCount] != -1)
                ++resultCount;

            //일치하는 도서 수를 클라이언트에게 전송
            sprintf(msg, "%s/%d", FIND_BOOK, resultCount);
            send(clientSock, msg, BUF_SIZE, 0);

            //일치하는 도서의 도서 정보를 클라이언트에게 전송
            for(int i = 0; i < resultCount; ++i)
            {
                const BOOKINFO book = books.arr[result[i]];
                sprintf(msg, "%s/%s/%s/%.2f", BOOK_INFO, book.bookTitle, book.author, book.rating);
                send(clientSock, msg, BUF_SIZE, 0);
            }
 
            ReleaseMutex(hBookMutex);
        }
        //도서 추가 요청일 경우
        else if(!strcmp(splitMsg[0], CREATE_BOOK))
        {
            BOOKINFO book;

            WaitForSingleObject(hBookMutex, INFINITE);

            //클라이언트로부터 받아온 도서 정보를 book에 저장
            strcpy(book.bookTitle, splitMsg[1]);
            strcpy(book.author, splitMsg[2]);
            book.rating = atof(splitMsg[3]);

            //도서 추가 및 저장 후 결과 저장
            int result = addBook(&books, book);
            result = result && saveDataList("file/booklist2.txt", (void*)&books, sizeof(BOOKINFO));

            //실행 결과를 클라이언트에 전송
            sprintf(msg, "%s/%s", CREATE_BOOK, (result ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hBookMutex);
        }
        //도서 정보 변경 요청일 경우
        else if(!strcmp(splitMsg[0], MODIFY_BOOK))
        {
            BOOKINFO book;

            WaitForSingleObject(hBookMutex, INFINITE);

            //클라이언트로부터 받아온 도서 정보를 book에 저장
            strcpy(book.bookTitle, splitMsg[2]);
            strcpy(book.author, splitMsg[3]);
            book.rating = atof(splitMsg[4]);

            //일치하는 도서의 도서 정보 변경 및 저장 후 결과 저장
            int result = updateBook(&books, splitMsg[1], book);
            result = result && saveDataList("file/booklist2.txt", (void*)&books, sizeof(BOOKINFO));

            //실행 결과를 클라이언트에 전송
            sprintf(msg, "%s/%s", MODIFY_BOOK, (result ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hBookMutex);
        }
        //도서 삭제 요청일 경우
        else if(!strcmp(splitMsg[0], DELETE_BOOK))
        {
            WaitForSingleObject(hBookMutex, INFINITE);

            //일치하는 도서 정보의 도서 삭제 및 저장 후 결과 저장
            int result = deleteBook(&books, splitMsg[1]);
            result = result && saveDataList("file/booklist2.txt", (void*)&books, sizeof(BOOKINFO));

            //실행 결과를 클라이언트에 전송
            sprintf(msg, "%s/%s", DELETE_BOOK, (result ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hBookMutex);
        }
        //평점 순 랭킹 요청일 경우
        else if(!strcmp(splitMsg[0], RANKED_BOOK))
        {
            //현재 저장된 도서 수 만큼 임시 배열 생성
            int rank[books.size];

            WaitForSingleObject(hBookMutex, INFINITE);

            //rank[i]에 books.arr의 index 번호 저장(0 ~ books.size)
            for(int i = 0; i < books.size; ++i)
                rank[i] = i;

            //내장된 qsort 함수를 이용하여 평점을 기준으로 내림차순 정렬
            qsort(rank, sizeof(rank)/sizeof(int), sizeof(int), compareByRate);

            //1등부터 splitMsg[1]등까지의 도서 정보를 클라이언트에게 전송
            for(int i = 0; i < atoi(splitMsg[1]); ++i)
            {
                const BOOKINFO book = books.arr[rank[i]];
                sprintf(msg, "%s/%s/%s/%.2f", BOOK_INFO, book.bookTitle, book.author, book.rating);
                send(clientSock, msg, BUF_SIZE, 0);
            }

            ReleaseMutex(hBookMutex);
        }
        //사용자 생성 요청일 경우
        else if(!strcmp(splitMsg[0], CREATE_USER))
        {
            USERINFO user;

            WaitForSingleObject(hUserMutex, INFINITE);

            //클라이언트로부터 받은 사용자 정보를 user에 저장
            strcpy(user.username, splitMsg[1]);
            strcpy(user.password, splitMsg[2]);

            //사용자 생성 및 저장 후 결과 저장
            int result = addUser(&users, user);
            result = result && saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

            //실행 결과를 클라이언트에게 전송
            sprintf(msg, "%s/%s", CREATE_USER, (result ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hUserMutex);
        }
        //등록된 사용자들의 사용자 이름 요청인 경우
        else if(!strcmp(splitMsg[0], REGISTERED_USER))
        {
            WaitForSingleObject(hUserMutex, INFINITE);

            //클라이언트에게 등록된 사용자 수 전송
            sprintf(msg, "%s/%d", REGISTERED_USER, users.size);
            send(clientSock, msg, BUF_SIZE, 0);

            //클라이언트에게 등록된 사용자들의 사용자 이름 전송
            for(int i = 0; i < users.size; ++i)
            {
                sprintf(msg, "%s/%s", USER_INFO, users.arr[i].username);
                send(clientSock, msg, BUF_SIZE, 0);
            }

            ReleaseMutex(hUserMutex);
        }
        //사용자 삭제 요청일 경우
        else if(!strcmp(splitMsg[0], DELETE_USER))
        {
            WaitForSingleObject(hUserMutex, INFINITE);

            int result = deleteUser(&users, splitMsg[1]);
            result = result && saveDataList("file/users.txt", (void*)&users, sizeof(USERINFO));

            //실행 결과를 클라이언트에게 전송
            sprintf(msg, "%s/%s", DELETE_USER, (result ? "T" : "F"));
            send(clientSock, msg, BUF_SIZE, 0);

            ReleaseMutex(hUserMutex);
        }

        FreeSplitMessage(splitMsg);
    }

    printf("Disconnected Client\n");
    closesocket(clientSock);
    return 0;
}

void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);

    exit(1);
}
