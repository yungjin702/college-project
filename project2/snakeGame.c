//작성자: 박영진
//내용  : 스네이크 게임 구현을 위한 snakeGame.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <windows.h>
#include <conio.h>
#include "snakeGame.h"
#include "linkedList.h"
#include "queue.h"
#include "graphic.h"

//전역 변수 선언
volatile int running = 0;                                   //게임 중인지 확인
volatile int key = 0;                                       //이동 방향
const char* object[3] = {" ", "■", "★"};                   //게임에서 사용할 오브젝트 문자
int printX[4] = {1, 18, 35, 52};                           //출력 위치
POS direction[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};     //방향
int score = 0;                                             //점수
QUEUE player[4] = {0};                                     //플레이어들의 위치
int board[4][BOARD_Y][BOARD_X] = {0,};                     //맵 정보

unsigned WINAPI inputThread(void* arg)
{
    while(running)
    {
        //키보드 입력이 들어온 경우
        if(kbhit())
        {
            char ch = getch();

            switch (toupper(ch))
            {
            case 'W':
                key = 0;
                break;
            case 'A':
                key = 1;
                break;
            case 'S':
                key = 2;
                break;
            case 'D':
                key = 3;
                break;
            default:
                break;
            }
        }
    }
}

void init()
{
    running = 0;
    key = 0;
    score = 0;

    //플레이어들의 위치 정보 초기화
    for(int i = 0; i < 4; ++i)
    {
        while(!isEmpty(&player[i]))
            free(dequeue(&player[i]));

        //초기 위치 설정
        POS* temp = (POS*)malloc(sizeof(POS));
        temp->x = BOARD_X / 2;
        temp->y = BOARD_Y / 2;
        enqueue(&player[i], temp);
    }

    //맵 정보 초기화
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < BOARD_Y; ++j)
        {
            memset(board[i][j], 0, sizeof(int) * BOARD_X);
        }  
        board[i][BOARD_Y / 2][BOARD_X / 2] = PLAYER;
    }
}

int move(int playerNum, int key) //0: 충돌 발생, 1: 정상 이동, 2: 사과을 먹음
{
    POS* pos = (POS*)player[playerNum].rear->data;
    POS nextPos = direction[key];
    int result = 0;

    //다음 이동 위치 계산
    nextPos.x = nextPos.x + pos->x;
    nextPos.y = nextPos.y + pos->y;

    //벽과 충돌하는 지 확인 
    if(nextPos.x < 0 || nextPos.x >= BOARD_X) return result;
    if(nextPos.y < 0 || nextPos.y >= BOARD_Y) return result;

    //다음 이동 위치에 사과가 있는 경우
    if(board[playerNum][nextPos.y][nextPos.x] == APPLE)
    {
        ++score, result = 2;
    }
    else
    {
        //충돌 처리 전 꼬리 이동
        pos = dequeue(&player[playerNum]);
        board[playerNum][pos->y][pos->x] = EMPTY;
        printObject(playerNum, EMPTY, pos->x, pos->y);

        free(pos);

        //다음 이동 위치에 플레이어가 있는 경우
        if(board[playerNum][nextPos.y][nextPos.x] == PLAYER)
            return result;
        else
            result = 1;
    }

    board[playerNum][nextPos.y][nextPos.x] = PLAYER;
    printObject(playerNum, PLAYER, nextPos.x, nextPos.y);

    //위치 저장
    POS* temp = (POS*)malloc(sizeof(POS));
    temp->x = nextPos.x;
    temp->y = nextPos.y;
    enqueue(&player[playerNum], temp);

    return result;
}

POS createApple(int playerNum)
{
    POS pos;

    //사과 생성 위치 결정
    do{
        pos.x = rand() % BOARD_X;
        pos.y = rand() % BOARD_Y;

    }while(board[playerNum][pos.y][pos.x] != EMPTY);

    board[playerNum][pos.y][pos.x] = APPLE;
    printObject(playerNum, APPLE, pos.x, pos.y);

    return pos;
}

void updateApplePos(int playerNum, int x, int y)
{
    printObject(playerNum, APPLE, x, y);
    board[playerNum][y][x] = APPLE;
}

void initPrint(int playerNum)
{
    for(int i = 0; i < BOARD_Y; ++i)
    {
        gotoxy(printX[playerNum], 4 + i);

        for(int j = 0; j < BOARD_X; ++j)
        {
            int objNum = board[playerNum][i][j];
            printf("%s", object[objNum]);
        }
    }
}

void printObject(int playerNum, int objNum, int x, int y)
{
    gotoxy(printX[playerNum] + x, 4 + y);
    printf("%s", object[objNum]);
}

unsigned WINAPI snakeGame(void* arg)
{
    //시드 초기화
    srand(time(NULL));
    running = 1;

    HANDLE inputHandle = (HANDLE)_beginthreadex(NULL, 0, inputThread, NULL, 0, NULL);
    PLAYERINFO playerInfo = *((PLAYERINFO*)arg);
    int tempKey;
    int result;
    extern char msg[];

    POS temp = createApple(playerInfo.playerNum);
    gotoxy(0, BOARD_Y + 8);
    printf("%d %d", temp.x, temp.y);

    do{
        tempKey = key;
        result = move(playerInfo.playerNum, tempKey);
        //메세지 전송
        if(result == 2)
        {
            temp = createApple(playerInfo.playerNum);
        }
    
    Sleep(100);
    }while(result != 0);

    running = 0;

    return 0;
}

// int main()
// {   
//     HANDLE game;

//     PLAYERINFO playerinfo;
//     playerinfo.playerNum = 0;

//     for(int i = 0; i < 4; ++i)
//     {
//         player[i].front = NULL;
//         player[i].rear = NULL;
//     }

//     system("cls");

//     drawBox(0, 3, BOARD_X + 2, BOARD_Y + 1);

//     init();
//     initPrint(0);
//     game = (HANDLE)_beginthreadex(NULL, 0, snakeGame, (void*)&playerinfo, 0, NULL);

//     WaitForSingleObject(game, INFINITE);

//     system("pause");

//     return 0;
// }