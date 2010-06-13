#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

#include <dirent.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#include <conio.h>
#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif
#else
#include <X11/X.h>
#include <X11/keysym.h>
#endif

#include <curses.h>

#define MSK_TITLE		"MISAKA System"
#define MSK_VER			"0.1"

#define ESCAPE_KEY		0x1b
#ifndef CTL_PGUP
#define CTL_PGUP		0x1bd
#endif
#ifndef CTL_PGDN
#define CTL_PGDN		0x1be
#endif
#ifndef PADENTER
#define PADENTER		0x1cb
#endif
#define PAD_ROWS		(LINES - 2)
#define PAD_COLS		COLS

#define ArraySize(x)	(sizeof((x)) / sizeof((x)[0]))

// ----------------------------------------

typedef struct {
	char * Command;
	char * Helptext;

	void (* Function)(void * Params);
} __MSK_Console_Cmd;

typedef struct {
	int Handle;
	int ExitHandle;

	bool IsRunning;
	bool IsLogging;
	FILE * Log;

	void (* FuncStack[256])();
	int FunctionNo;

	__MSK_Console_Cmd Command[512];
	int CommandCount;

	char * Title;

	WINDOW * WindowMain;
	WINDOW * WindowCommand;
	WINDOW * WindowPad;

	char CommandHist[512][512];
	int CommandHistCount;
	int CommandHistCurrent;

	int InCmdPosition;

	int TotalConsoleLine;
	int CurrentConsoleLine;

	char * ValidCharacters;

	bool UpdateHist;
} __MSK_Console;

// ----------------------------------------

extern __MSK_Console Console;

// ----------------------------------------

void MSK_SetMainFunction(void * Function);
void MSK_RestoreMainFunction();

#ifndef WIN32
extern int kbhit(void);
#endif
