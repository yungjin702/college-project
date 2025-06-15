//작성자: 박영진
//내용  : 클라이언트, 서버 간 메시지 처리 및 규약을 위한 command.c 정의
//수정일: 2025.06.15
//생성일: 2025.05.07

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

//메시지 종류에 대응하는 값 정의
const char* PORT            = "55555";
const char* LOGIN           = "L";
const char* LOGOUT          = "LO";
const char* USER_INFO       = "UI";
const char* CREATE_USER     = "CU";
const char* MODIFY_PW       = "MP";
const char* DELETE_USER     = "DU";
const char* ROOM_INFO       = "RI";
const char* GET_ROOMINFO    = "GL";
const char* CREATE_ROOM     = "CR";
const char* ENTER_ROOM      = "ER";
const char* UPDATED_ROOM    = "UR";
const char* LEAVE_ROOM      = "LR";
const char* READY_GAME      = "RG";
const char* GAME_START      = "GS";
const char* NEXT_DIRE       = "ND";
const char* CREATE_APPLE    = "CA";
const char* GAME_OVER       = "GO";
const char* GAME_RESULT     = "GR"; 

char** SplitMessage(char* message)
{
    char** result = malloc(5 * sizeof(char*));
    int idx = 0;
    char* splitMsg = strtok(message, "/");

    while(splitMsg != NULL)
    {
        result[idx] = strdup(splitMsg);
        splitMsg = strtok(NULL, "/");
        ++idx;
    }

    //사용되지 않은 공간은 NULL로 처리
    for(idx; idx < 5; ++idx)
        result[idx] = NULL;

    return result;
}

void FreeSplitMessage(char** message)
{
    for(int i = 0; i < 5; ++i)
    {
        free(message[i]);
    }
    free(message);
}