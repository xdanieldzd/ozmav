/*
	draw.c - Chip-8 screen drawing & related functions
*/

#include "globals.h"

unsigned char characterSet[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,	//0
	0x20, 0x60, 0x20, 0x20, 0x70,	//1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,	//2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,	//3
	0x90, 0x90, 0xF0, 0x10, 0x10,	//4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,	//5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,	//6
	0xF0, 0x10, 0x20, 0x40, 0x40,	//7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,	//8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,	//9
	0xF0, 0x90, 0xF0, 0x90, 0x90,	//A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,	//B
	0xF0, 0x80, 0x80, 0x80, 0xF0,	//C
	0xE0, 0x90, 0x90, 0x90, 0xE0,	//D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,	//E
	0xF0, 0x80, 0xF0, 0x80, 0x80	//F
};

void prepareChars()
{
	memcpy(&interpreter.memory[0], characterSet, 0x10 * 5);
}

void resizeScreen()
{
	SDL_Event event;
	event.type = SDL_VIDEORESIZE;
	event.resize.w = program.scrWidth;
	event.resize.h = program.scrHeight;
	SDL_PushEvent(&event);
}

void drawScreen()
{
	int i, j, xcent, ycent;

	xcent = (program.scrWidth / 2) - ((64 * program.zoomFactor) / 2);
	ycent = (program.scrHeight / 2) - ((32 * program.zoomFactor) / 2);

	SDL_Rect scrrect;
	scrrect.x = xcent;
	scrrect.y = ycent;
	scrrect.w = 64 * program.zoomFactor;
	scrrect.h = 32 * program.zoomFactor;
	SDL_FillRect(program.screen, &scrrect, SDL_MapRGB(program.screen->format, 0, 0, 0));

	for(i = 0; i < 32; i++) {
		for(j = 0; j < 64; j++) {
			if(interpreter.screen[j][i]) drawRect((j * program.zoomFactor) + xcent, (i * program.zoomFactor) + ycent, program.zoomFactor, program.zoomFactor, (program.colorMode ? program.colWhite : program.colGreen));
		}
	}
}

void drawRect(int x, int y, int w, int h, SDL_Color col)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_FillRect(program.screen, &rect, SDL_MapRGB(program.screen->format, col.r, col.g, col.b));
}
