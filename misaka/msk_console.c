#include "misaka.h"
#include "msk_base.h"
#include "msk_console.h"
#include "msk_ui.h"

void MSK_AddCommand(char * Cmd, char * Help, void * Func)
{
	if(Console.CommandCount < 512) {
		Console.Command[Console.CommandCount].Command = Cmd;
		Console.Command[Console.CommandCount].Helptext = Help;
		Console.Command[Console.CommandCount].Function = Func;

		Console.CommandCount++;
	}
}

void MSK_RequestExit()
{
	Console.ExitHandle = MSK_MessageBox("Exit", "Do you really want to exit?", MSK_UI_MSGBOX_YESNO);
}

void MSK_ShowHelp()
{
	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "\nAvailable commands:\n");
	int i = 0;
	int CmdCount = ArraySize(Console.Command);
	while((i < CmdCount) && (Console.Command[i].Function != NULL)) {
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, "- %s: %s\n", Console.Command[i].Command, Console.Command[i].Helptext);
		i++;
	}
}

void MSK_ConsolePrint(int Color, char * Format, ...)
{
	Console.CurrentConsoleLine = Console.TotalConsoleLine;

	char Text[256];
	va_list argp;

	if(Format == NULL) return;

	va_start(argp, Format);
	vsprintf(Text, Format, argp);
	va_end(argp);

	char * Ptr;
	Ptr = strstr(Text, "\n");
	if(Ptr == 0x00) {
		strcat(Text, "\n");
		Console.TotalConsoleLine++;
		Console.CurrentConsoleLine++;
	} else {
		while(Ptr != NULL) {
			Ptr = strstr(Ptr+1, "\n");
			Console.TotalConsoleLine++;
			Console.CurrentConsoleLine++;
		}
	}

	wattron(Console.WindowPad, COLOR_PAIR(Color));
	wprintw(Console.WindowPad, "%s", Text);
	wattroff(Console.WindowPad, COLOR_PAIR(Color));
	wrefresh(Console.WindowPad);

	MSK_Refresh(Console.TotalConsoleLine - 1);
}

void MSK_Refresh(int Line)
{
	// refresh the log, show it from line n
	refresh();
	int PadStart = (Line - PAD_ROWS) + 1;
	if(PadStart < 0) PadStart = 0;

	prefresh(Console.WindowPad, PadStart, 0, 1, 0, PAD_ROWS, PAD_COLS);

	// draw scrollbar - very shitty calculation, but approximate enough...
	float Pos = ((float)(LINES - 2) / (float)(Console.TotalConsoleLine+1) * (float)((PadStart+((LINES-2)/2)+2)+1));
	if((Pos <= 1) || (PadStart == 0)) Pos = 1;
	if(Console.CurrentConsoleLine == Console.TotalConsoleLine) Pos = (LINES-2);

/*	float Pos = ((float)(LINES - 2) * Console.CurrentConsoleLine) / ((float)(Console.TotalConsoleLine) - 1);
	if(Pos <= 1) Pos = 1;
	if(Console.CurrentConsoleLine == Console.TotalConsoleLine) Pos = (LINES-2);
*/
	mvwvline(Console.WindowMain, 1, COLS - 1, ACS_VLINE, LINES - 2);
	mvwvline(Console.WindowMain, (int)Pos, COLS - 1, ACS_BLOCK, 1);
}

void MSK_CurrentTime(char * TimeBuf)
{
	// get current time and format a string
	time_t RawTime;
	struct tm * TimeInfo;
	time(&RawTime);
	TimeInfo = localtime(&RawTime);
	strftime(TimeBuf, 128, "%H:%M:%S", TimeInfo);
}

void MSK_DoEvents_Console()
{
	MSK_Refresh(Console.CurrentConsoleLine - 1);

	char CmdProcess[256];
	bool InputDone = FALSE;

	// check if a key has been hit, if not return
	// without this, the whole program would stall unless a key is actually pressed down
	if(kbhit()) {
		// get the pressed key
		int Character = getch();

		// update display
		wmove(Console.WindowCommand, 0, 0);
		wprintw(Console.WindowCommand, "> ");

		// check what key has been pressed
		switch(Character) {
			// page up -> go up a page in log
			case KEY_PPAGE:
				if(Console.CurrentConsoleLine > PAD_ROWS) {
					Console.CurrentConsoleLine-=20;
					if(Console.CurrentConsoleLine <= PAD_ROWS) Console.CurrentConsoleLine = PAD_ROWS;
					MSK_Refresh(Console.CurrentConsoleLine - 1);
				}
				break;

			// page down -> go down a page in log
			case KEY_NPAGE:
				Console.CurrentConsoleLine+=20;
				if(Console.CurrentConsoleLine <= Console.TotalConsoleLine) {
					MSK_Refresh(Console.CurrentConsoleLine - 1);
				} else {
					Console.CurrentConsoleLine = Console.TotalConsoleLine;
					MSK_Refresh(Console.CurrentConsoleLine - 1);
				}
				break;

			// ctrl & page up -> go up a line in log
			case CTL_PGUP:
				if(Console.CurrentConsoleLine > PAD_ROWS) {
					--Console.CurrentConsoleLine;
					MSK_Refresh(Console.CurrentConsoleLine - 1);
				}
				break;

			// ctrl & page down -> go down a line in log
			case CTL_PGDN:
				++Console.CurrentConsoleLine;
				if(Console.CurrentConsoleLine <= Console.TotalConsoleLine) {
					MSK_Refresh(Console.CurrentConsoleLine - 1);
				} else {
					Console.CurrentConsoleLine--;
				}
				break;

			// cursor up -> go up in command history
			case KEY_UP:
				if(Console.CommandHistCurrent > 0) {
					--Console.CommandHistCurrent;
					strcpy(Console.CommandHist[Console.CommandHistCount], Console.CommandHist[Console.CommandHistCurrent]);
					Console.InCmdPosition = strlen(Console.CommandHist[Console.CommandHistCount]);
				}
				break;

			// cursor down -> go down in command history
			case KEY_DOWN:
				++Console.CommandHistCurrent;
				if(Console.CommandHistCurrent == Console.CommandHistCount) {
					memset(Console.CommandHist[Console.CommandHistCount], 0x00, sizeof(Console.CommandHist[Console.CommandHistCount]));
					Console.InCmdPosition = 0;
				} else if(Console.CommandHistCurrent <= Console.CommandHistCount) {
					strcpy(Console.CommandHist[Console.CommandHistCount], Console.CommandHist[Console.CommandHistCurrent]);
					Console.InCmdPosition = strlen(Console.CommandHist[Console.CommandHistCount]);
				} else {
					Console.CommandHistCurrent--;
				}
				break;

			// escape -> clear entered command string
			case ESCAPE_KEY:
				memset(Console.CommandHist[Console.CommandHistCount], 0x00, sizeof(Console.CommandHist[Console.CommandHistCount]));
				Console.InCmdPosition = 0;
				break;

			// backspace -> delete a character backwards
			case '\b':
			case KEY_BACKSPACE:
				if(Console.InCmdPosition > 0) Console.CommandHist[Console.CommandHistCount][--Console.InCmdPosition] = '\0';
				break;

			// return/enter -> finish command entry and go to processing
			case '\n':
			case PADENTER:
				if(strlen(Console.CommandHist[Console.CommandHistCount]) == 0) break;
				strcpy(CmdProcess, Console.CommandHist[Console.CommandHistCount]);
				Console.InCmdPosition = 0;
				InputDone = TRUE;
				break;

			// everything else -> check character validity and add to command string, if passed
			default: {
				char * p;
				if((p = strchr(Console.ValidCharacters, Character)) == NULL) break;		// if invalid, break
				if((Character == ' ') && (Console.InCmdPosition == 0)) break;			// if first character is space, break
				if(Console.InCmdPosition >= 256) break;									// if cmd too long, break;
				Console.CommandHist[Console.CommandHistCount][Console.InCmdPosition++] = Character;
				break; }
		}
	}

	// update display
	wmove(Console.WindowCommand, 0, 2);
	wclrtoeol(Console.WindowCommand);
	wprintw(Console.WindowCommand, Console.CommandHist[Console.CommandHistCount]);
	if(ReturnVal.Handle == Console.Handle) {
		//wprintw(Console.WindowCommand, "_");
		mvwaddch(Console.WindowCommand, 0, (2 + Console.InCmdPosition), ACS_CKBOARD);
	}

	// if enter has been pressed, process the command
	if(InputDone) {
		wmove(Console.WindowCommand, 0, 2);
		wclrtoeol(Console.WindowCommand);
		wrefresh(Console.WindowCommand);
		MSK_DoEvents_Console_Command(CmdProcess);
		InputDone = FALSE;
	}

	// update display
	refresh();
	wrefresh(Console.WindowCommand);
	wrefresh(Console.WindowMain);
}

void MSK_DoEvents_Console_Command(char * Command)
{
	// assume that the command history must be updated
	Console.UpdateHist = TRUE;

	// figure out where the command itself ends, and where params start
	char CmdName[256];
	strcpy(CmdName, Command);
	char *Ptr;
	if((Ptr = strstr(CmdName, " "))) *Ptr = 0x00;

	// go through all existing commands to find the entered one
	int i = 0; bool CmdNotFound = TRUE;
	int CmdCount = ArraySize(Console.Command);
	while((i < CmdCount) && (Console.Command[i].Function != NULL)) {
		if(!strcmp(CmdName, Console.Command[i].Command)) {
			Console.Command[i].Function(Ptr);
			CmdNotFound = FALSE;
			break;
		}
		i++;
	}

	// if the command couldn't be found, spit out unknown command message and disable history update
	if(CmdNotFound) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "> Unknown command: %s\n", Command);
		Console.UpdateHist = FALSE;
	}

	// if the command and the command previously issued are the same, don't add it to the history again
	if(strcmp(Console.CommandHist[Console.CommandHistCount], Console.CommandHist[Console.CommandHistCount - 1]) == 0) {
		Console.UpdateHist = FALSE;
		Console.CommandHistCurrent = Console.CommandHistCount;
	}

	// if the history is supposed to be update, do it, else reset the current slot
	if(Console.UpdateHist) {
		Console.CommandHistCount++;
		Console.CommandHistCurrent = Console.CommandHistCount;
	} else {
		memset(Console.CommandHist[Console.CommandHistCount], 0x00, sizeof(Console.CommandHist[Console.CommandHistCount]));
		Console.CommandHistCurrent = Console.CommandHistCount;
	}

	// update display
	wmove(Console.WindowCommand, 0, 2);
	wclrtoeol(Console.WindowCommand);
	wprintw(Console.WindowCommand, Console.CommandHist[Console.CommandHistCount]);
}
