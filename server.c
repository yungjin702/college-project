//작성자: 박영진
//내용  : 온라인 도서정보관리 시스템의 서버 구현
//수정일: 2025.05.07
//생성일: 2025.05.07

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"
#include "books.h"
#include "command.h"

//파일에 있는 데이터를 원하는 자료형에 맞게 불러오는 함수
int loadDataList(const char*, void*, const size_t);
//저장하고자 하는 데이터를 원하는 자료형에 맞게 파일에 저장
int saveDataList(const char*, void*, const size_t);

int main()
{
    BOOKS books;
    books.size = 0;

    USERS users;
    users.size = 0;

    loadDataList("file/booklist2.txt", &books, sizeof(BOOKINFO));
    loadDataList("file/users.txt", &users, sizeof(USERINFO));

    // USERINFO user = {"admin", "test123"};

    // addUser(&users, user);

    // saveDataList("file/users.txt", &users, sizeof(USERINFO));

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
