//작성자: 박영진
//내용  : 클라이언트, 서버 간 메시지 처리 및 규약을 위한 command.h 정의
//수정일: 2025.05.07
//생성일: 2025.05.07

#ifndef COMMAND_H
#define COMMAND_H

#define BUF_SIZE 300

//메시지 종류 선언(상수)
extern const char* PORT;
extern const char* LOGIN;
extern const char* LOGOUT;
extern const char* USER_INFO;
extern const char* CREATE_USER;
extern const char* REGISTERED_USER;
extern const char* DELETE_USER;
extern const char* FIND_BOOK;
extern const char* BOOK_INFO;
extern const char* CREATE_BOOK;
extern const char* MODIFY_BOOK;
extern const char* DELETE_BOOK;
extern const char* RANKED_BOOK;

//메시지를 종류와 내용으로 분리하는 함수
char** SplitMessage(char*);
//분리한 메시지의 메모리를 정리하는 함수
void FreeSplitMessage(char**);

#endif