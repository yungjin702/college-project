//작성자: 박영진
//내용  : 클라이언트 프로그램에서 사용할 그래픽 관련 graphic.c 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#include <stdio.h>
#include <windows.h>
#include "graphic.h"

void gotoxy(int x, int y)
{
    COORD pos = {x , y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void drawBox(int x, int y, int w, int h)
{
    //윗 면 그리기
    gotoxy(x, y);
    printf("┏");
    for(int i = x + 1; i <= (x + w) - 1; ++i)
    {
        printf("━");
    }
    printf("┓");

    //옆 면 그리기
    for(int i = y + 1; i <= (y + h) - 1; ++i)
    {
        gotoxy(x, i);
        printf("┃");
        gotoxy(x + w, i);
        printf("┃");
    }

    //아랫 면 그리기
    gotoxy(x, y + h);
    printf("┗");
    for(int i = x + 1; i <= (x + w) - 1; ++i)
    {
        printf("━");
    }
    printf("┛");
}