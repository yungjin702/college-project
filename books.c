//작성자: 박영진
//내용  : 도서 정보 및 관리를 위한 books.c 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"

typedef struct bookinfo{
    int num;                //순번
    char bookTitle[100];    //도서 제목
    char author[50];        //저자명
    float rating;           //평점
}BOOKINFO;

typedef struct books{
    BOOKINFO arr[700];
    int size;               //등록된 도서 수
} BOOKS;

int addBook(BOOKS* books, const BOOKINFO book)
{
    if(books->size >= 700) return 0;
    else
    {
        books->arr[books->size++] = book;
        return 1;
    }
}

int* findBooks(BOOKS* books, const char* type, const char* compare)
{
    int* result = NULL;
    int idx = 0;
    int size = 0;

    for(idx = 0; idx < books->size; ++idx)
    {
        if(size >= 10) break;

        //type이 1인 경우 도서 제목과 일치하는 도서 찾기
        if(!strcmp(type, "1") && !strcmp(books->arr[idx].bookTitle, compare))
        {
            ++size;
            result = realloc(result, sizeof(int) * size);
            result[size - 1] = idx;
        }
        //type이 2인 경우 도서 저자명과 일치하는 도서 찾기
        else if(!strcmp(type, "2") && !strcmp(books->arr[idx].author, compare))
        {
            ++size;
            result = realloc(result, sizeof(int) * size);
            result[size - 1] = idx;
        }
    }

    //배열의 끝을 알리기 위한 작업
    result = realloc(result, sizeof(int) * (size + 1));
    result[size] = -1;

    return result;
}

int updateBook(BOOKS* books, const char* bookTitle, const BOOKINFO book)
{
    int idx = 0;

    for(idx = 0; idx < books->size; ++idx)
    {
        if(!strcmp(books->arr[idx].bookTitle, bookTitle))
        {
            books->arr[idx] = book;
            break;
        }
    }

    if(idx == books->size) return 0;
    else return 1;
}

int deleteBook(BOOKS* books, const char* bookTitle)
{
    int idx = 0;

    for(idx = 0; idx < books->size; ++idx)
    {
        if(!strcmp(books->arr[idx].bookTitle, bookTitle))
        {
            //일치하는 정보의 idx를 기준으로 뒤에 있는 정보를 옮기기
            while(idx < books->size - 1)
            {
                books->arr[idx] = books->arr[idx + 1];
                ++idx;
            }
            --books->size;
            return 1;
        }
    }

    return 0;
}

// int main()
// {
//     BOOKS books;
//     books.size = 0;

//     BOOKINFO book = {1, "Test", "TESTS", 4.5f};
//     BOOKINFO book1 = {1, "test", "TESTS", 4.5f};

//     printf("%d\n", addBook(&books, book));
//     printf("%d\n", addBook(&books, book1));
//     printf("%s\n%s\n%f\n", books.arr[0].bookTitle, books.arr[0].author, books.arr[0].rating);

//     int* result = findBooks(&books, "1", "Test");

//     while(*result != -1) 
//     {
//         printf("T: %d\n", *result);
//         printf("%s\n%s\n", books.arr[*result].bookTitle, books.arr[*result].author);
//         ++result;
//     }

//     printf("%d\n", updateBook(&books, "Test", book));

//     printf("%d\n", deleteBook(&books, "Test"));
//     printf("%d\n", books.size);
//     printf("%s\n", books.arr[0].bookTitle);


//     free(result);

//     return 0;
// }

