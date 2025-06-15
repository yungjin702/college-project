//작성자: 박영진
//내용  : 스네이크 게임 구현을 위한 snakeGame.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef SNAKEGAME_H
#define SNAKEGAME_H

//상수 정의
#define EMPTY 0
#define PLAYER 1
#define APPLE 2
#define BOARD_X 17
#define BOARD_Y 15

typedef unsigned int SOCKET;

//위치 정보를 가지는 구조체 선언
typedef struct pos
{
    int x;
    int y;
}POS;

//클라이언트의 정보를 가지는 구조체 선언
typedef struct playerinfo
{
    SOCKET sock;
    int roomNum;
    int playerNum;
}PLAYERINFO;

//키보드 입력을 받아 키보드 값에 맞는 정수 값을 저장하는 함수
unsigned __stdcall inputThread(void*);
//게임 준비를 위한 변수 초기화하는 함수
void init();
//플레이어 이동 처리 함수
int move(int, int);
//사과를 생성하고 그 위치를 반환하는 함수
POS createApple(int);
//다른 플레이어의 사과 위치 반영하는 함수
void updateApplePos(int, int, int);
//스네이크 게임의 기본 화면 출력하는 함수
void initPrint(int);
//특정 위치에 오브젝트 출력하는 함수
void printObject(int, int, int, int);
//스네이크 게임
unsigned __stdcall snakeGame(void*);

#endif