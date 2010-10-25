#include "globals.h"

#define BASE_MEMVIEWPAGESIZE	9
#define BASE_MEMVIEWWIDTH		56

#define MEMVIEW_MAXSEL			255

int memCurrentSelLine = 0;
int memCurrentSelRow = 0;

int memCurrentPage = 0;
int memLastPage = 0;

int memPageSize = 0;
int memWidth = 0;

int memMinEntry = 0, memMaxEntry = 0;

void doMemviewInput(SDL_KeyboardEvent * ev)
{
	switch(ev->keysym.sym) {
		case SDLK_ESCAPE: {
			if(!program.isPaused) {
				program.inpReturn = 2;
			} else {
				exitProgram(EXIT_SUCCESS);
			}
			break; }

		case SDLK_PAGEDOWN: {
			if(MEMVIEW_MAXSEL > memPageSize) {
				memCurrentSelLine += memPageSize;
				if(memCurrentSelLine > MEMVIEW_MAXSEL) memCurrentSelLine = MEMVIEW_MAXSEL;
			} else {
				memCurrentSelLine = MEMVIEW_MAXSEL;
			}
			break; }
		case SDLK_DOWN: {
			memCurrentSelLine++;
			if(memCurrentSelLine > MEMVIEW_MAXSEL) memCurrentSelLine = MEMVIEW_MAXSEL;
			break; }

		case SDLK_PAGEUP: {
			if(MEMVIEW_MAXSEL > memPageSize) {
				memCurrentSelLine -= memPageSize;
				if(memCurrentSelLine < 0) memCurrentSelLine = 0;
			} else {
				memCurrentSelLine = 0;
			}
			break; }
		case SDLK_UP: {
			memCurrentSelLine--;
			if(memCurrentSelLine < 0) memCurrentSelLine = 0;
			break; }

		case SDLK_LEFT: {
			if((memCurrentSelRow == 0) && (memCurrentSelLine == 0)) break;
			memCurrentSelRow--;
			if(memCurrentSelRow < 0) {
				memCurrentSelRow = 15;
				if(memCurrentSelLine > 0) memCurrentSelLine--;
			}
			break; }
		case SDLK_RIGHT: {
			if((memCurrentSelRow >= 15) && (memCurrentSelLine >= MEMVIEW_MAXSEL)) break;
			memCurrentSelRow++;
			if(memCurrentSelRow > 15) {
				memCurrentSelRow = 0;
				if(memCurrentSelLine < MEMVIEW_MAXSEL) memCurrentSelLine++;
			}
			break; }

		case SDLK_BACKSPACE: {
			memCurrentSelLine = interpreter.pc / 16;
			memCurrentSelRow = interpreter.pc & 0xF;
			break; }

		case SDLK_HOME: {
			if((memCurrentSelLine > 0) && (memCurrentSelLine <= 32)) {
				memCurrentSelLine = 0;
			} else {
				memCurrentSelLine = 32;
			}
			memCurrentSelRow = 0;
			break; }
		case SDLK_END: {
			memCurrentSelLine = MEMVIEW_MAXSEL;
			break; }

		case SDLK_TAB: {
			program.inpReturn = 10;
			break; }

		case SDLK_F1: {
			messageBox(
				"Memory viewer controls:| |- Cursor keys: Move up/down lines; between bytes|- Page Up/Down: Go back/forward one page| |- Backspace: Jump to PC|- Home: Jump to entry point, to start of memory|- End: Jump to end of memory| |- Tab: Switch to disassembler",
				MB_OKAY, (void*)doOkayKeydown, (void*)doOkayMousedown, (void*)doOkayMousemove, NULL);
			break; }

		default:
			break;
	}
}

int memWndX, memWndY, memWndW, memWndH;

void drawMemWindow()
{
	int i = 0, j = 0, scrpos = 17;

	boxRGBA(program.screen, memWndX, memWndY, memWndX + memWndW, memWndY + memWndH + 18, 0, 0, 0, 192);
	boxRGBA(program.screen, memWndX, memWndY, memWndX + memWndW, memWndY + 19, 0, 64, 0, 224);

	memCurrentPage = memCurrentSelLine / memPageSize;

	fontPrint(memWndX + 4, memWndY + 4, 0, program.colWhite, parseFormat("Current Address: 0x%04X", (memCurrentSelLine * 16) + memCurrentSelRow));
	char * PCMsg = "Current PC: 0x%04X";
	fontPrint(memWndX + memWndW - (strlen(PCMsg) * 6) - 4, memWndY + 4, 0, program.colWhite, parseFormat(PCMsg, interpreter.pc));

	hlineRGBA(program.screen, memWndX, memWndX + memWndW, memWndY + 19, 0, 128, 0, 255);

	memMinEntry = 0;
	if(memCurrentSelLine > (memPageSize) / 2) memMinEntry = memCurrentSelLine - (memPageSize) / 2;

	memMaxEntry = (memMinEntry + memPageSize) + 1;
	if(memCurrentSelLine > MEMVIEW_MAXSEL - (memPageSize / 2)) {
		memMinEntry = MEMVIEW_MAXSEL - ((memPageSize));
		memMaxEntry = MEMVIEW_MAXSEL + 1;
	}
	if(memMinEntry < 0) memMinEntry = 0;

	for(i = memMinEntry; ((i < MEMVIEW_MAXSEL + 1) && (i < memMaxEntry)); i++) {
		fontPrint(memWndX + 4, memWndY + 6 + scrpos, 0, ((i == memCurrentSelLine) ? program.colGreen : program.colWhite), parseFormat("0x%04X:", i*16));
		for(j = 0; j < 16; j++) {
			unsigned char byte = interpreter.memory[i*16 + j];
			int thisx = memWndX + 4 + 48;
			if((j == memCurrentSelRow) && (i == memCurrentSelLine)) {
				boxRGBA(program.screen,
					thisx + (j*12), memWndY + 6 + scrpos, thisx + (j*12) + 12, memWndY + scrpos + 17,
					64, 128, 64, 128);
				boxRGBA(program.screen,
					thisx + 197 + (j*6), memWndY + 6 + scrpos, thisx + 197 + (j*6) + 6, memWndY + scrpos + 17,
					64, 128, 64, 128);
			}
			fontPrint(thisx + (j*12), memWndY + 6 + scrpos, 0, (((j == memCurrentSelRow) && (i == memCurrentSelLine)) ? program.colGreen : program.colWhite), parseFormat("%02X", byte));
			fontPrint(thisx + 197 + (j*6), memWndY + 6 + scrpos, 0, (((j == memCurrentSelRow) && (i == memCurrentSelLine)) ? program.colGreen : program.colWhite),
				(((byte >= 0x20) && (byte <= 0x7F)) ? parseFormat("%c", byte) : "."));
		}
		scrpos+=12;
	}
	memLastPage = memCurrentPage;

	boxRGBA(program.screen, memWndX, memWndY + memWndH - 1, memWndX + memWndW, memWndY + memWndH - 1 + 19, 0, 64, 0, 224);
	hlineRGBA(program.screen, memWndX, memWndX + memWndW, memWndY + memWndH - 1, 0, 128, 0, 255);

	char * HelpMsg = "F1:Help";
	fontPrint(memWndX + memWndW - (strlen(HelpMsg) * 6) - 4, memWndY + memWndH + 3, 0, program.colWhite, HelpMsg);

	rectangleRGBA(program.screen, memWndX, memWndY, memWndX + memWndW, memWndY + memWndH + 18, 0, 128, 0, 255);
}

int showMemview()
{
	memCurrentSelLine = 32;
	memCurrentSelRow = 0;
	if(program.inpReturn == 10) {
		memCurrentSelLine = disasmCurrentSel / 8;
		memCurrentSelRow = disasmBasePC + (disasmCurrentSel & 0x7) * 2;
	}

	program.inpReturn = -1;

	void * oldkd = program.func_keydown;
	program.func_keydown = doMemviewInput;

	memPageSize = (int)(BASE_MEMVIEWPAGESIZE + ((program.zoomFactor - 6) + 0.5f) * 2.5f) + 1;
	memWidth = (int)(BASE_MEMVIEWWIDTH + ((program.zoomFactor - 6) + 0.5f) * 4.5f);

	int textlen = (memWidth * 6) + 1;
	memWndW = textlen;
	memWndH = (memPageSize * 12) + 39;
	memWndX = (((program.scrWidth * program.zoomFactor) / 2)) - (textlen / 2);
	memWndY = (((program.scrHeight * program.zoomFactor) / 2)) - (memWndH / 2) - 10;

	program.func_draw_gui = drawMemWindow;

	while(!doSDLEvents()) {
		if(program.func_draw_main != NULL) program.func_draw_main();
		if(program.func_draw_gui != NULL) program.func_draw_gui();
		if(program.func_draw_box != NULL) program.func_draw_box();

		endDrawing();
	}

	program.func_draw_gui = NULL;
	program.func_keydown = oldkd;

	if(program.inpReturn == 10) showDisassembly();

	program.inpReturn = -1;

	return EXIT_SUCCESS;
}
