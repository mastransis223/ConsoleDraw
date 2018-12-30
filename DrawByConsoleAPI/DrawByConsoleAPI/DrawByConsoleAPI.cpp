// DrawByConsoleAPI.cpp : Defines the entry point for the console application.
//

/*
	>Description: Console drawing test using printf and ANSI Escape sequence
	>Summary:
		Flicking: a little
		Speed   : Fast
	>Note:	- Set console buffer size to 150x80 (Console --> Propertice --> Layout --> Screen buffer size)
			- turn-off unikey
*/

#include "stdafx.h"
#include <windows.h>
#include <string.h>

using namespace std;

/* Console dimension, need to set the corresponding console buffer size */
#define SCREEN_WIDTH	150
#define SCREEN_HEIGHT	80


typedef struct{
	wchar_t c;
	int x;
	int y;
	int w;
	int h;
} Rectangle_t;


Rectangle_t _movableObject = { (wchar_t)'#', 0, 0, 10, 10 };
Rectangle_t _animationObject = { (wchar_t)'#', 0, 0, 13, 3 };

/* 2D graphic console buffer */
wchar_t _consoleBuff[SCREEN_WIDTH * SCREEN_HEIGHT + 1] = { 0 };
HANDLE _hOutputConsole = NULL;


/* Draw character c at position x, y */
void drawChr(wchar_t *buff, wchar_t c, int x, int y)
{
	buff[x + y*SCREEN_WIDTH] = c;
}

/* Draw Text */
void drawText(wchar_t *buff, wchar_t* str, int x, int y)
{
	wmemcpy(&buff[x + y*SCREEN_WIDTH], str, wcslen(str));
}

/* Draw rectangle (by character c) at psition x, y with width w, height h */
void drawRectangle(wchar_t *buff, wchar_t c, int x, int y, int w, int h)
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

/* Init console */
void consoleInit(void)
{
	_hOutputConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(_hOutputConsole);
}

/* From object info, draw it into graphic buffer */
void render(void)
{
	/* Clear console (by filling all with space) */
	wmemset(_consoleBuff, ' ', (sizeof(_consoleBuff)-1) / 2);

	/* Draw movable object */
	drawRectangle(_consoleBuff, _movableObject.c, _movableObject.x, _movableObject.y, _movableObject.w, _movableObject.h);
	drawText(_consoleBuff, L"MOVABLE", _movableObject.x + 2, _movableObject.y + _movableObject.h / 2 - 1);
	drawText(_consoleBuff, L"OBJECT", _movableObject.x + 2, _movableObject.y + _movableObject.h / 2);

	/* Draw aniamtion object */
	drawRectangle(_consoleBuff, (wchar_t)'#', _animationObject.x, _animationObject.y, _animationObject.w, _animationObject.h);
	drawText(_consoleBuff, L"ANIMATION", _animationObject.x + 2, _animationObject.y + _animationObject.h / 2);
}

/* From graphic buffer, push to console buffer */
void refresh(void)
{
	DWORD n;
	WriteConsoleOutputCharacter(_hOutputConsole, _consoleBuff, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &n);
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
	else if (timeCnt % 50 == 0){
		_movableObject.c = (wchar_t)'#';
		if (timeCnt % 100 == 0)
			_movableObject.c = (wchar_t)'.';
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
	INPUT_RECORD record = {0};
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
	consoleInit();
	while (1)
	{
		processInput();
		update();
		render();
		refresh();
		Sleep(10);
	}
}