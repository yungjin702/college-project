//작성자: 박영진
//내용  : 도서 정보 및 관리를 위한 books.h 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#ifndef BOOKS_H
#define BOOKS_H

//도서 정보를 담을 구조체 선언
typedef struct bookinfo BOOKINFO;
//도서 정보를 관리할 구조체 선언
typedef struct books BOOKS;

//도서 정보를 추가하는 함수수
int addBook(BOOKS*, const BOOKINFO);
//매개변수로 받은 정보와 일치하는 도서들의 idx를 반환하는 함수
int* findBooks(BOOKS*, const char*, const char*);
//도서 제목과 일치하는 도서의 정보를 변경하는 함수
int updateBook(BOOKS*, const char*, const BOOKINFO);
//도서 제목과 일치하는 도서의 정보를 삭제하는 함수
int deleteBook(BOOKS*, const char*);

#endif