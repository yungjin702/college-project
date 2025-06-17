//작성자: 박영진
//내용  : 사용자 정보 및 관리를 위한 user.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdlib.h>
#include <string.h>
#include "users.h"
#include "linkedList.h"

int addUser(USERS* users, USERINFO* user)
{
    //search 함수를 이용하여 같은 정보의 유저가 있는 지 확인
    if(search(users->list, (void*)user, compareUser) == NULL)
    {
        //연결리스트에 추가
        append(&users->list, (void*)user);
        ++users->size;
        return 1;
    }
    
    //만약 같은 정보의 유저가 있으면 메모리 해제
    free(user);
    return 0;
}

USERINFO* findUser(USERS* users, const char* username)
{
    //username과 일치하는 유저 정보가 있는 지 확인
    NODE* userNode = search(users->list, (void*)username, compareUsername);

    if(userNode != NULL) return (USERINFO*)userNode->data;
    else return NULL;
}

int modifyPassword(USERS* users, const char* username, const char* newPassword)
{
    USERINFO* user = findUser(users, username);

    if(user != NULL)
    {
        strcpy(user->password, newPassword);
        return 1;
    }

    return 0;
}

int deleteUser(USERS* users, const char* username)
{
    int result = deleteNode(&users->list, (void*)username, compareUsername);

    if(result) --users->size;

    return result;
}

int compareUser(const void* userA, const void* userB)
{
    USERINFO* a = (USERINFO*)userA;
    USERINFO* b = (USERINFO*)userB;

    if(!strcmp(a->username, b->username) && !strcmp(a->password, b->password))
        return 1;
    else
        return 0;
}

int compareUsername(const void* user, const void* username)
{
    USERINFO* a = (USERINFO*)user;

    if(!strcmp(a->username, username))
        return 1;
    else
        return 0;
}

