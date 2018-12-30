// DrawByPrintfAndAnsiEscape.cpp : Defines the entry point for the console application.
//

/*
	>Description: Console drawing test using printf and ANSI Escape sequence
	>Summary:
		Flicking: None
		Speed   : Nomral (better than [DrawByPrintfAndClscr] but less than [DrawByConsoleAPI])
	>Note:	- Using a console that support ANSI Escape sequence to run this program. Ex: comEMU (https://conemu.github.io/)
			- Resize console to maximum
			- turn-off unikey
*/

#include "stdafx.h"
#include <windows.h>
#include <string.h>

using namespace std;

/* Console dimension, need to set the corresponding console buffer size */
#define SCREEN_WIDTH	120
#define SCREEN_HEIGHT	40


typedef struct{
	char c;
	int x;
	int y;
	int w;
	int h;
} Rectangle_t;


Rectangle_t _movableObject = { (char)'#', 0, 0, 10, 10 };
Rectangle_t _animationObject = { (char)'#', 0, 0, 13, 3 };

/* 2D graphic console buffer */
char _consoleBuff[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
HANDLE _hOutputConsole = NULL;


/* Draw character c at position x, y */
void drawChr(char *buff, char c, int x, int y)
{
	buff[x + y*SCREEN_WIDTH] = c;
}

/* Draw Text */
void drawText(char *buff, char* str, int x, int y)
{
	memcpy(&buff[x + y*SCREEN_WIDTH], str, strlen(str));
}

/* Draw rectangle (by character c) at psition x, y with width w, height h */
void drawRectangle(char *buff, char c, int x, int y, int w, int h)
{
	/* Draw horiontal */
	int tmpX, tmpY;
	tmpY = y + h - 1;
	for (int i = 0; i < w; ++i){
		tmpX = x + i;
		drawChr(buff, c, tmpX, y);
		drawChr(buff, c, tmpX, tmpY);
	}

	/* Draw vertical */
	tmpX = x + w - 1;
	for (int i = 0; i < h; ++i){
		tmpY = y + i;
		drawChr(buff, c, x, tmpY);
		drawChr(buff, c, tmpX, tmpY);
	}
}

/* From object info, draw it into graphic buffer */
void render(void)
{
	/* Clear console (by filling all with space) */
	memset(_consoleBuff, ' ', sizeof(_consoleBuff));

	/* Draw movable object */
	drawRectangle(_consoleBuff, _movableObject.c, _movableObject.x, _movableObject.y, _movableObject.w, _movableObject.h);
	drawText(_consoleBuff, "MOVABLE", _movableObject.x + 2, _movableObject.y + _movableObject.h / 2 - 1);
	drawText(_consoleBuff, "OBJECT", _movableObject.x + 2, _movableObject.y + _movableObject.h / 2);

	/* Draw aniamtion object */
	drawRectangle(_consoleBuff, (char)'#', _animationObject.x, _animationObject.y, _animationObject.w, _animationObject.h);
	drawText(_consoleBuff, "ANIMATION", _animationObject.x + 2, _animationObject.y + _animationObject.h / 2);
}

/* From graphic buffer, push to console buffer */
void refresh(void)
{
	/* Move cursor back first position (line 1, column 1) */
	printf("\033[1;1H");

	/* Call printf to push all line of console buffer into console */
	char line[SCREEN_WIDTH + 3];
	for (int i = 0; i < SCREEN_HEIGHT; ++i){
		memcpy(line, &_consoleBuff[i*SCREEN_WIDTH], SCREEN_WIDTH);
		/* padding new line and end of string */
		strcpy(&line[SCREEN_WIDTH], "\r\n");
		printf(line);
	}
}

/* For 10mS time base */
void update(void){
	static int timeCnt = 0;
	timeCnt++; if (timeCnt == 10000) timeCnt = 0;

	/* Each 30mS, update position of animation object */
	if (timeCnt % 3 == 0){
		_animationObject.x++;
		if (_animationObject.x + _animationObject.w > SCREEN_WIDTH){
			_animationObject.x = 0;
		}
	}

	/* Each 500mS, blink _movableObject */
	if (timeCnt % 50 == 0){
		_movableObject.c = (char)'#';
		if (timeCnt % 100 == 0)
			_movableObject.c = (char)'.';
	}

}

/* Hanlde console input (key event)
Todo: handle mouse event, creat mouseMovableObject
*/
void processInput(void)
{
#if 0
	static HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	char keyIn = 0;
	DWORD n;
	INPUT_RECORD record = { 0 };
	/* Read event from console */
	ReadConsoleInput(hStdin, &record, 1, &n);
	if (n == 1 && record.EventType == KEY_EVENT){
		keyIn = record.Event.KeyEvent.uChar.AsciiChar;
	}
	/* Fix blocking issue when call ReadConsoleInput()
	--> Put back a dummy record to console, by this way, console buffer is never emty, so no blocking
	--> new issue: key press delay
	*/
	record.EventType = MENU_EVENT;
	WriteConsoleInput(hStdin, &record, 1, &n);

	if (keyIn == 'a' && _movableObject.x > 0)
		_movableObject.x--;
	else if (keyIn == 'd' && _movableObject.x < SCREEN_WIDTH - _movableObject.w)
		_movableObject.x++;
	else if (keyIn == 'w'&& _movableObject.y > 0)
		_movableObject.y--;
	else if (keyIn == 's' && _movableObject.y < SCREEN_HEIGHT - _movableObject.h)
		_movableObject.y++;
#endif
#define IS_KEY_PRESSING(key)	(GetAsyncKeyState(key) && 0x8000)

	if (IS_KEY_PRESSING('A') && _movableObject.x > 0)
		_movableObject.x--;
	else if (IS_KEY_PRESSING('D') && _movableObject.x + _movableObject.w < SCREEN_WIDTH)
		_movableObject.x++;
	else if (IS_KEY_PRESSING('W') && _movableObject.y > 0)
		_movableObject.y--;
	else if (IS_KEY_PRESSING('S') && _movableObject.y + _movableObject.h < SCREEN_HEIGHT)
		_movableObject.y++;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/* Hide the cursor of console */
	printf("\033[?25l");
	while (1)
	{
		processInput();
		update();
		render();
		refresh();
		Sleep(10);
	}
}
