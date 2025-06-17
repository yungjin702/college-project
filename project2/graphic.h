//작성자: 박영진
//내용  : 클라이언트 프로그램에서 사용할 그래픽 관련 graphic.h 정의
//수정일: 2025.06.15
//생성일: 2025.06.15

#ifndef GRAPHIC_H
#define GRAPHIC_H

//콘솔 커서의 위치를 이동하는 함수
void gotoxy(int, int);
//콘솔의 x, y 위치에서 W x H 크기의 사각형 테두리를 출력하는 함수
void drawBox(int, int, int, int);
//콘솔의 x, y 위치에서 size만큼 공백 입력하는 함수
void removeLine(int, int, int);

#endif
