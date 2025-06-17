//작성자: 박영진
//내용  : 클라이언트, 서버 간 메시지 처리 및 규약을 위한 command.h 정의
//수정일: 2025.06.15
//생성일: 2025.05.07

#ifndef COMMAND_H
#define COMMAND_H

#define BUF_SIZE 300

//메시지 종류 선언(상수)
const char* PORT;
const char* LOGIN;
const char* LOGOUT;
const char* USER_INFO;
const char* CREATE_USER;
const char* MODIFY_PW;
const char* DELETE_USER;
const char* ROOM_INFO;
const char* GET_ROOMLIST;
const char* CREATE_ROOM;
const char* ENTER_ROOM;
const char* UPDATED_ROOM;
const char* LEAVE_ROOM;
const char* READY_GAME;
const char* GAME_START;
const char* NEXT_DIRE;
const char* CREATE_APPLE;
const char* GAME_OVER;
const char* GAME_RESULT;

//메시지를 종류와 내용으로 분리하는 함수
char** SplitMessage(char*);
//분리한 메시지의 메모리를 정리하는 함수
void FreeSplitMessage(char**);

#endif