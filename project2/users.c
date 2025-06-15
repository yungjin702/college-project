//작성자: 박영진
//내용  : 사용자 정보 및 관리를 위한 user.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdio.h>
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

// int main()
// {
//     USERS users;
//     users.list = 0;
//     users.size = 0;

//     for(int i = 0; i < 10; ++i)
//     {
//         USERINFO* temp = (USERINFO*)malloc(sizeof(USERINFO));
//         char tempS[20]; sprintf(tempS, "user%d", i);

//         strcpy(temp->username, tempS);
//         strcpy(temp->password, tempS);

//         printf("%d\n", addUser(&users, temp));
//     }

//     printf("%d", users.size);

//     USERINFO* user = findUser(&users, "user0");
//     printf("%s/%s\n", user->username, user->password);

//     modifyPassword(&users, "user0", "user");
//     printf("%s/%s\n", user->username, user->password);

//     printf("%d\n", deleteUser(&users, "user0"));
//     user = findUser(&users, "user0");

//     if(user == NULL) printf("NULL\n");

//     printf("%d\n", users.size);

//     return 0;
// }

