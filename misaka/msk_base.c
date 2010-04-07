#include "misaka.h"
#include "msk_base.h"
#include "msk_console.h"
#include "msk_ui.h"

__MSK_Console Console;
__MSK_UI_Dialog_Data Dialog[512];
__MSK_UI_Object_Data Object[512];

__MSK_UI_Dialog_Data Dialog_Active;

__MSK_Return ReturnVal;

int MSK_Init(char * Apptitle)
{
	srand((unsigned)time(0));

	memset(&Console, 0x00, sizeof(Console));
	memset(&Dialog, -1, ArraySize(Dialog));
	memset(&Object, -1, ArraySize(Object));

	initscr();
	clear();
	noecho();

	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	// init colors
	start_color();
	init_color(COLOR_RED, 1000, 0, 0);
	init_color(COLOR_YELLOW, 800, 400, 0);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_WHITE, COLOR_BLUE);
	init_pair(5, COLOR_BLUE, COLOR_WHITE);
	color_set(1, 0);

	// init command bar
	Console.WindowCommand = newwin(1, COLS, LINES - 1, 0);
	wbkgd(Console.WindowCommand, COLOR_PAIR(4));

	// init log
	Console.WindowPad = newpad(16384, COLS - 1);
	wbkgd(Console.WindowPad, COLOR_PAIR(0));

	// init main window / title bar
	Console.WindowMain = newwin(LINES - 1, COLS, 0, 0);
	wbkgd(Console.WindowMain, COLOR_PAIR(4));

	scrollok(Console.WindowMain, TRUE);
	idlok(Console.WindowMain, TRUE);

	// initial command bar display
	wmove(Console.WindowCommand, 0, 0);
	wprintw(Console.WindowCommand, "> ");

	refresh();
	wrefresh(Console.WindowCommand);
	wrefresh(Console.WindowMain);

	memset(Console.CommandHist, 0x00, sizeof(Console.CommandHist));

	// set title bar
	Console.Title = Apptitle;

	// set main function
	Console.FunctionNo = 0;
	MSK_SetMainFunction(MSK_DoEvents_Console);

	// add basic commands
	MSK_AddCommand("exit", "Exit the program", MSK_RequestExit);
	MSK_AddCommand("help", "Show command help", MSK_ShowHelp);

	Console.IsRunning = TRUE;

	// set and return console main handle
	Console.Handle = rand();
	return Console.Handle;
}

void MSK_SetValidCharacters(char * Chars)
{
	Console.ValidCharacters = Chars;
}

void MSK_Exit()
{
	delwin(Console.WindowMain);
	delwin(Console.WindowCommand);
	delwin(Console.WindowPad);
	endwin();
}

void MSK_SetMainFunction(void * Function)
{
	if(Console.FunctionNo < 256) Console.FuncStack[++Console.FunctionNo] = Function;
}

void MSK_RestoreMainFunction()
{
	if(Console.FunctionNo > 0) Console.FuncStack[Console.FunctionNo--] = NULL;
}

int MSK_DoEvents()
{
	ReturnVal.Handle = Console.Handle;

	// display current time
  	char Time[256];
  	MSK_CurrentTime(Time);

  	wmove(Console.WindowMain, 0, COLS - 10);
	wprintw(Console.WindowMain, "| %s", Time);
	wrefresh(Console.WindowMain);

	if(Console.FuncStack[Console.FunctionNo] != NULL) Console.FuncStack[Console.FunctionNo]();

	return Console.IsRunning;
}

#ifndef WIN32
int kbhit(void)
{
	struct timeval tv;
	fd_set read_fd;
	tv.tv_sec=0;
	tv.tv_usec=0;
	FD_ZERO(&read_fd);
	FD_SET(0,&read_fd);
	if(select(1, &read_fd,NULL, NULL, &tv) == -1) return 0;
	if(FD_ISSET(0,&read_fd)) return 1;
	return 0;
}
#endif
