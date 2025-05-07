//작성자: 박영진
//내용  : 사용자 정보 및 관리를 위한 user.h 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#ifndef USERS_H
#define USERS_H

typedef struct userinfo USERINFO;
typedef struct users USERS;

int addUser(USERS*, const USERINFO);
int findUser(USERS*, const char*, const char*);
int deleteUser(USERS*, const char*);

#endif