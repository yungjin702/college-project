//작성자: 박영진
//내용  : 사용자 정보 및 관리를 위한 user.h 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#ifndef USERS_H
#define USERS_H

//사용자 정보를 담을 구조체 선언
typedef struct userinfo{
    char username[15];  //사용자 이름
    char password[20];  //비밀번호
}USERINFO;

//사용자 정보를 관리할 구조체 선언
typedef struct users{
    USERINFO arr[10];
    int size;           //등록된 사용자 수
}USERS;

//사용자 정보를 추가하는 함수 선언
int addUser(USERS*, const USERINFO);
//매개변수로 받은 정보와 일치하는 사용자 정보 찾는 함수 선언
int findUser(USERS*, const char*, const char*);
//사용자 이름과 일치하는 정보를 삭제하는 함수 선언
int deleteUser(USERS*, const char*);

#endif