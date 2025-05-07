//작성자: 박영진
//내용  : 사용자 정보 및 관리를 위한 user.c 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#include <stdio.h>
#include <string.h>
#include "users.h"

typedef struct userinfo{
    char username[15];  //사용자 이름
    char password[20];  //비밀번호
}USERINFO;

typedef struct users{
    USERINFO arr[10];
    int size;           //등록된 사용자 수
}USERS;

int addUser(USERS* users, const USERINFO user)
{
    if(users->size >= 10) return 0;
    else
    {
        users->arr[users->size++] = user;
        return 1;
    }
}

int findUser(USERS* users, const char* username, const char* password)
{
    int idx = 0;

    //등록된 사용자 정보와 비교
    for(idx = 0; idx < users->size; ++idx)
    {
        if(!strcmp(users->arr[idx].username, username) && !strcmp(users->arr[idx].password, password))
        {
            break;
        }
    }

    if(idx == users->size) return 0;
    else return 1;
}

int deleteUser(USERS* users, const char* username)
{
    int idx = 0;

    //등록된 사용자 이름과 일치하는 정보 찾기
    for(idx = 0; idx < users->size; ++idx)
    {
        if(!strcmp(users->arr[idx].username, username))
        {
            //일치하는 정보의 idx를 기준으로 뒤에 있는 정보를 옮기기
            while(idx < users->size - 1)
            {
                users->arr[idx] = users->arr[idx + 1];
                ++idx;
            }
            --users->size;
            return 1;
        }
    }

    return 0;
}

// int main()
// {
//     USERS users;
//     users.size = 0;
//     USERINFO user = {"test", "test"};

//     printf("%d\n", addUser(&users, user));

//     printf("%s\n%s\n", users.arr[0].username, users.arr[0].password);

//     printf("%d", users.size);

//     printf("%d\n", findUser(&users, "test", "test"));
//     printf("%d\n", findUser(&users, "1412", "tsdfs"));

//     printf("%d\n", deleteUser(&users, "test"));

//     printf("%d", users.size);

//     return 0;
// }

