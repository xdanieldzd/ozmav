/*
	disasm.c - Chip-8 disassembler
*/

#include "globals.h"

#define BASE_DISASMPAGESIZE		7
#define BASE_DISASMWIDTH		50

#define DISASM_MAXSEL			2047

char opcodeText[BUFSIZ];

int disasmBasePC = 0;
int disasmCurrentSel = 0;

int disasmCurrentPage = 0;
int disasmLastPage = 0;

int disasmPageSize = 0;
int disasmWidth = 0;

int disasmMinEntry = 0, disasmMaxEntry = 0;

int disasmRunForOps = 0;

void doDisasmInput(SDL_KeyboardEvent * ev)
{
	switch(ev->keysym.sym) {
		case SDLK_ESCAPE: {
			if(!program.isPaused) {
				program.inpReturn = 2;
			} else {
				exitProgram(EXIT_SUCCESS);
			}
			break; }

		case SDLK_RIGHT: {
			if(DISASM_MAXSEL > disasmPageSize) {
				disasmCurrentSel += disasmPageSize;
				if(disasmCurrentSel > DISASM_MAXSEL) disasmCurrentSel = DISASM_MAXSEL;
			} else {
				disasmCurrentSel = DISASM_MAXSEL;
			}
			break; }
		case SDLK_DOWN: {
			if((disasmCurrentSel != DISASM_MAXSEL) && (disasmBasePC)) {
				disasmCurrentSel++;
				if(disasmCurrentSel > DISASM_MAXSEL) disasmCurrentSel = DISASM_MAXSEL;
				disasmBasePC = 0;
			} else {
				disasmBasePC = 1;
			}
			break; }

		case SDLK_LEFT: {
			if(DISASM_MAXSEL > disasmPageSize) {
				disasmCurrentSel -= disasmPageSize;
				if(disasmCurrentSel < 0) disasmCurrentSel = 0;
			} else {
				disasmCurrentSel = 0;
			}
			break; }
		case SDLK_UP: {
			if((disasmCurrentSel != 0) && (!disasmBasePC)) {
				disasmCurrentSel--;
				if(disasmCurrentSel < 0) disasmCurrentSel = 0;
				disasmBasePC = 1;
			} else {
				disasmBasePC = 0;
			}
			break; }

		case SDLK_KP_ENTER:
		case SDLK_RETURN: {
			disasmRunForOps = 1;
			break; }
		case SDLK_SPACE: {
			disasmRunForOps = program.disasmDefRunOps;
			break; }
		case SDLK_BACKSPACE: {
			disasmBasePC = (interpreter.pc & 1);
			disasmCurrentSel = interpreter.pc / 2;
			break; }

		case SDLK_HOME: {
			if((disasmCurrentSel > 0) && (disasmCurrentSel <= 256)) {
				disasmCurrentSel = 0;
				disasmBasePC = 0;
			} else {
				disasmCurrentSel = 256;
				disasmBasePC = 0;
			}
			break; }
		case SDLK_END: {
			disasmCurrentSel = DISASM_MAXSEL;
			break; }

		case SDLK_TAB: {
			program.inpReturn = 10;
			break; }

		case SDLK_F1: {
			messageBox(
				parseFormat(
					"Disassembler controls:|"
					" |"
					"- Cursor Up/Down: Go up/down one opcode|"
					"- Cursor Left/Right: Go back/forward one page|"
					" |"
					"- Return: Execute next opcode|"
					"- Space: Execute next %i opcodes|"
					" |"
					"- Backspace: Jump to PC|"
					"- Home: Jump to entry point, to start of memory|"
					"- End: Jump to end of memory|"
					" |"
					"- Tab: Switch to memory viewer",
					program.disasmDefRunOps),
				MB_OKAY, (void*)doOkayKeydown, (void*)doOkayMousedown, (void*)doOkayMousemove, NULL);
			break; }

		default:
			break;
	}
}

void getOpcodeText(unsigned short op)
{
	strcpy(opcodeText, "\0");

	switch(op & 0xF000) {
		case 0x0000: {
			switch(op & 0x000F) {
				case 0x0000: strcpy(opcodeText, "CLS"); break;
				case 0x000E: strcpy(opcodeText, "RET"); break;
			}
			break;
		}
		case 0x1000: strcpy(opcodeText, parseFormat("JP 0x%04X", getNNN(op))); break;
		case 0x2000: strcpy(opcodeText, parseFormat("CALL 0x%04X", getNNN(op))); break;
		case 0x3000: strcpy(opcodeText, parseFormat("SE V%1X, 0x%02X", getX(op), getKK(op))); break;
		case 0x4000: strcpy(opcodeText, parseFormat("SNE V%1X, 0x%02X", getX(op), getKK(op))); break;
		case 0x5000: strcpy(opcodeText, parseFormat("SE V%1X, V%1X", getX(op), getY(op))); break;
		case 0x6000: strcpy(opcodeText, parseFormat("LD V%1X, 0x%02X", getX(op), getKK(op))); break;
		case 0x7000: strcpy(opcodeText, parseFormat("ADD V%1X, 0x%02X", getX(op), getKK(op))); break;
		case 0x8000: {
			switch(op & 0x000F) {
				case 0x0000: strcpy(opcodeText, parseFormat("LD V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0001: strcpy(opcodeText, parseFormat("OR V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0002: strcpy(opcodeText, parseFormat("AND V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0003: strcpy(opcodeText, parseFormat("XOR V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0004: strcpy(opcodeText, parseFormat("ADD V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0005: strcpy(opcodeText, parseFormat("SUB V%1X, V%1X", getX(op), getY(op))); break;
				case 0x0006: strcpy(opcodeText, parseFormat("SHR V%1X{, V%1X}", getX(op), getY(op))); break;
				case 0x0007: strcpy(opcodeText, parseFormat("SUBN V%1X, V%1X", getX(op), getY(op))); break;
				case 0x000E: strcpy(opcodeText, parseFormat("SHL V%1X{, V%1X}", getX(op), getY(op))); break;
			}
			break;
		}
		case 0x9000: strcpy(opcodeText, parseFormat("SNE V%1X, V%1X", getX(op), getY(op))); break;
		case 0xA000: strcpy(opcodeText, parseFormat("LD I, 0x%04X", getNNN(op))); break;
		case 0xB000: strcpy(opcodeText, parseFormat("JP V0, 0x%04X", getNNN(op))); break;
		case 0xC000: strcpy(opcodeText, parseFormat("RND V%1X, 0x%02X", getX(op), getKK(op))); break;
		case 0xD000: strcpy(opcodeText, parseFormat("DRW V%1X, V%1X, %1X", getX(op), getY(op), getN(op))); break;
		case 0xE000: {
			switch(getKK(op)) {
				case 0x009E: strcpy(opcodeText, parseFormat("SKP V%1X", getX(op))); break;
				case 0x00A1: strcpy(opcodeText, parseFormat("SKPN V%1X", getX(op))); break;
			}
			break;
		}
		case 0xF000: {
			switch(getKK(op)) {
				case 0x0007: strcpy(opcodeText, parseFormat("LD V%1X, DT", getX(op))); break;
				case 0x000A: strcpy(opcodeText, parseFormat("LD V%1X, K", getX(op))); break;
				case 0x0015: strcpy(opcodeText, parseFormat("LD DT, V%1X", getX(op))); break;
				case 0x0018: strcpy(opcodeText, parseFormat("LD ST, V%1X", getX(op))); break;
				case 0x001E: strcpy(opcodeText, parseFormat("ADD I, V%1X", getX(op))); break;
				case 0x0029: strcpy(opcodeText, parseFormat("LD F, V%1X", getX(op))); break;
				case 0x0033: strcpy(opcodeText, parseFormat("LD B, V%1X", getX(op))); break;
				case 0x0055: strcpy(opcodeText, parseFormat("LD [I], V%1X", getX(op))); break;
				case 0x0065: strcpy(opcodeText, parseFormat("LD V%1X, [I]", getX(op))); break;
				case 0x0075: strcpy(opcodeText, parseFormat("LD R, V%1X", getX(op))); break;
				case 0x0085: strcpy(opcodeText, parseFormat("LD V%1X, R", getX(op))); break;
			}
			break;
		}
		default: break;
	}
}

int disasmWndX, disasmWndY, disasmWndW, disasmWndH;

void drawDisasmWindow()
{
	int i = 0, scrpos = 27;

	// main background
	boxRGBA(program.screen, disasmWndX, disasmWndY, disasmWndX + disasmWndW, disasmWndY + disasmWndH + 29, 0, 0, 0, 192);

	// header
	char * Msgs[][32] = {
		{"PC:0x%04X"}, {"I:0x%04X"}, {"DT:%3i"}, {"ST:%3i"}, {""}, {"Stack Pos:%i"}, {"Stack Val:0x%04X"}
	};
	unsigned short Vals[16] = {
		interpreter.pc, interpreter.addri, interpreter.tDelay, interpreter.tSound, 0, interpreter.sp, interpreter.stack[interpreter.sp]
	};

	int curx = 0, cury = 0;
	// -> bg
	boxRGBA(program.screen, disasmWndX, disasmWndY, disasmWndX + disasmWndW, disasmWndY + scrpos + 3, 0, 64, 0, 224);
	// -> text
	//  -> loop through all msgs
	for(i = 0; i < (int)arraySize(Msgs); i++) {
		// if msg isn't empty, print it out
		if(strcmp(*Msgs[i], "")) {
			fontPrint(disasmWndX + 4 + curx, disasmWndY + 4 + cury, 0, program.colWhite, parseFormat(*Msgs[i], Vals[i]));
			curx += ((strlen(*Msgs[i]) + 3) * 6);
		// else do "line break"
		} else {
			curx = 0;
			cury += 11;
		}
	}
	//  -> help
	char * HelpMsg = "F1:Help";
	fontPrint(disasmWndX + disasmWndW - (strlen(HelpMsg) * 6) - 4, disasmWndY + 15, 0, program.colWhite, HelpMsg);

	// -> sep line
	hlineRGBA(program.screen, disasmWndX, disasmWndX + disasmWndW, disasmWndY + scrpos + 3, 0, 128, 0, 255);

	disasmCurrentPage = disasmCurrentSel / disasmPageSize;
	disasmMinEntry = 0;
	if(disasmCurrentSel > (disasmPageSize - 1) / 2) disasmMinEntry = disasmCurrentSel - (disasmPageSize - 1) / 2;

	disasmMaxEntry = (disasmMinEntry + disasmPageSize);
	if(disasmCurrentSel > DISASM_MAXSEL - (disasmPageSize / 2)) {
		disasmMinEntry = DISASM_MAXSEL - ((disasmPageSize - 1));
		disasmMaxEntry = DISASM_MAXSEL + 1;
	}
	if(disasmMinEntry < 0) disasmMinEntry = 0;
//	fontPrint(0, 0, 1, program.colWhite, parseFormat("%i %i %i (0x%04x)", disasmCurrentSel, disasmMinEntry, disasmMaxEntry, disasmCurrentSel * 2));

	for(i = disasmMinEntry; ((i < DISASM_MAXSEL + 1) && (i < disasmMaxEntry)); i++) {
		unsigned short op = (interpreter.memory[disasmBasePC+i*2] << 8) | interpreter.memory[disasmBasePC+i*2 + 1];
		getOpcodeText(op);

		if(i == disasmCurrentSel) {
			boxRGBA(program.screen,
				disasmWndX + 4, disasmWndY + 6 + scrpos, disasmWndX + disasmWndW - 4, disasmWndY + scrpos + 17,
				64, 128, 64, 128);
		}

		fontPrint(disasmWndX + 4, disasmWndY + 6 + scrpos,
			0, ((i == disasmCurrentSel) ? program.colGreen : program.colWhite),
			(
				(i == DISASM_MAXSEL && disasmBasePC) ?
				parseFormat("0x%04X: %02X  %s", disasmBasePC + i*2, op >> 8, opcodeText) :
				parseFormat("0x%04X: %04X  %s", disasmBasePC + i*2, op, opcodeText)
			)
		);
		scrpos+=12;
	}
	disasmLastPage = disasmCurrentPage;

	// footer
	//  -> bg
	boxRGBA(program.screen, disasmWndX, disasmWndY + disasmWndH - 1, disasmWndX + disasmWndW, disasmWndY + disasmWndH - 1 + 30, 0, 64, 0, 224);
	//  -> sep line
	hlineRGBA(program.screen, disasmWndX, disasmWndX + disasmWndW, disasmWndY + disasmWndH - 1, 0, 128, 0, 255);
	//  -> text
	fontPrint(disasmWndX + 4, disasmWndY + disasmWndH + 3, 0, program.colWhite, parseFormat(
		"V0:%02X V1:%02X V2:%02X V3:%02X V4:%02X V5:%02X V6:%02X V7:%02X",
		interpreter.regs[0], interpreter.regs[1], interpreter.regs[2], interpreter.regs[3], interpreter.regs[4], interpreter.regs[5], interpreter.regs[6], interpreter.regs[7]
	));
	fontPrint(disasmWndX + 4, disasmWndY + disasmWndH + 14, 0, program.colWhite, parseFormat(
		"V8:%02X V9:%02X VA:%02X VB:%02X VC:%02X VD:%02X VE:%02X VF:%02X",
		interpreter.regs[8], interpreter.regs[9], interpreter.regs[10], interpreter.regs[11], interpreter.regs[12], interpreter.regs[13], interpreter.regs[14], interpreter.regs[15]
	));

	// main border
	rectangleRGBA(program.screen, disasmWndX, disasmWndY, disasmWndX + disasmWndW, disasmWndY + disasmWndH + 29, 0, 128, 0, 255);
}

int showDisassembly()
{
	disasmBasePC = (interpreter.pc & 1);
	disasmCurrentSel = interpreter.pc / 2;
	if(program.inpReturn == 10) {
		disasmBasePC = (((memCurrentSelLine * 16 + memCurrentSelRow)) & 1);
		disasmCurrentSel = (memCurrentSelLine * 16 + memCurrentSelRow) / 2;
	}

	program.inpReturn = -1;

	void * oldkd = program.func_keydown;
	program.func_keydown = doDisasmInput;

	disasmPageSize = (int)(BASE_DISASMPAGESIZE + ((program.zoomFactor - 6) + 0.5f) * 2.5f);
	disasmWidth = (int)(BASE_DISASMWIDTH + ((program.zoomFactor - 6) + 0.5f) * 4.5f);

	int textlen = (disasmWidth * 6) + 1;
	disasmWndW = textlen;
	disasmWndH = (disasmPageSize * 12) + 39;
	disasmWndX = (((program.scrWidth * program.zoomFactor) / 2)) - (textlen / 2);
	disasmWndY = (((program.scrHeight * program.zoomFactor) / 2)) - (disasmWndH / 2) - 16;

	program.func_draw_gui = drawDisasmWindow;

	while(!doSDLEvents()) {
		if(program.func_draw_main != NULL) program.func_draw_main();
		if(program.func_draw_gui != NULL) program.func_draw_gui();
		if(program.func_draw_box != NULL) program.func_draw_box();

		if(disasmRunForOps) {
			cpuRun();
			decreaseTimers();
			disasmBasePC = (interpreter.pc & 1);
			disasmCurrentSel = interpreter.pc / 2;
			disasmRunForOps--;
		}

		endDrawing();
	}

	if(program.inpReturn == 10) showMemview();

	program.func_draw_gui = NULL;

	program.func_keydown = oldkd;
	program.inpReturn = -1;

	return EXIT_SUCCESS;
}
