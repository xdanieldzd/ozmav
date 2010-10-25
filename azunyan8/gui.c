/*
	gui.c - basic GUI functionality (status messages, message boxes)
*/

#include "globals.h"

static char * msgBoxButtonNames[] = { "   Okay   ", "   Yes   ", "   No   " };

typedef struct {
	int bx, by, bx2, by2;
	int hr, hg, hb, ha;
	int over;
} __msgBoxButton;
__msgBoxButton msgButton[2];

int fontPrint(int x, int y, int hasBg, SDL_Color color, char * text, ...)
{
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;

	SDL_Color bgcolor = { 0, 0, 0, 0 };
	SDL_Surface * textsf;

	if(!(textsf = hasBg ? TTF_RenderText_Shaded(program.ttff, text, color, bgcolor) : TTF_RenderText_Blended(program.ttff, text, color))) {
		printf("Error while printing text (%s)", TTF_GetError());
		return EXIT_FAILURE;
	}

	SDL_BlitSurface(textsf, NULL, program.screen, &dstrect);

	SDL_FreeSurface(textsf);

	return EXIT_SUCCESS;
}

void addStatMessage(char * text)
{
	int no = 0;

	for(no = STATMSG_MAXMSG - 1; no >= 0; no--) {
		memcpy(&program.statmsg[no], &program.statmsg[no - 1], sizeof(__statmsg));
	}

	program.statmsg[0].timer = STATMSG_DURATION;
	sprintf(program.statmsg[0].text, "%s", text);
}

void deleteStatMessage(int id)
{
	memset(program.statmsg[id].text, 0x00, sizeof(program.statmsg[id].text));
	program.statmsg[id].timer = 0;
}

void deleteAllStatMessages()
{
	int no = 0;
	for(no = 0; no < STATMSG_MAXMSG; no++) deleteStatMessage(no);
}

void drawStatMessages()
{
	int i = 0, pos = 0;
	for(i = 0; i < STATMSG_MAXMSG; i++) {
		if(program.statmsg[i].timer > 0) pos++;
	}
	if(pos) {
		boxRGBA(program.screen, 0, (program.scrHeight * program.zoomFactor) - ((FONTHEIGHT+3) * pos), (program.scrWidth * program.zoomFactor), (program.scrHeight * program.zoomFactor), 0, 0, 0, 192);
		hlineRGBA(program.screen, 0, (program.scrWidth * program.zoomFactor), (program.scrHeight * program.zoomFactor) - ((FONTHEIGHT+3) * pos), 0, 128, 0, 128);
	}

	pos = 1;
	for(i = 0; i < STATMSG_MAXMSG; i++) {
		if(program.statmsg[i].timer > 0) {
			fontPrint(0, (program.scrHeight * program.zoomFactor) - ((FONTHEIGHT+3) * pos), 0, program.colWhite, parseFormat("%s", program.statmsg[i].text));
			program.statmsg[i].timer -= (program.zoomFactor - 5);
			pos++;

			if(program.statmsg[i].timer <= 0) deleteStatMessage(i);
		}

		//fontPrint(200, (i+1)*15, 1, program.colWhite, parseFormat("%d", program.statmsg[i].timer));
	}
}

int isOverButton(int mx, int my, int btn)
{
	if((mx > msgButton[btn].bx) && (mx < msgButton[btn].bx2) && (my > msgButton[btn].by) && (my < msgButton[btn].by2)) return 1;

	return 0;
}

void doYesNoKeydown(SDL_KeyboardEvent * ev)
{
	if((ev->keysym.sym == SDLK_RETURN) || (ev->keysym.sym == SDLK_KP_ENTER)) {
		program.inpReturn = 2;
	} else if(ev->keysym.sym == SDLK_ESCAPE) {
		program.inpReturn = 1;
	}
}

void doOkayKeydown(SDL_KeyboardEvent * ev)
{
	if((ev->keysym.sym == SDLK_RETURN) || (ev->keysym.sym == SDLK_KP_ENTER) || (ev->keysym.sym == SDLK_ESCAPE)) {
		program.inpReturn = 1;
	} else {
		program.inpReturn = 0;
	}
}

void doYesNoMousedown(SDL_MouseButtonEvent * ev)
{
	if(ev->button != SDL_BUTTON_LEFT) return;

	// check yes
	if(isOverButton(ev->x, ev->y, 0)) program.inpReturn = 2;

	// check no
	if(isOverButton(ev->x, ev->y, 1)) program.inpReturn = 1;
}

void doOkayMousedown(SDL_MouseButtonEvent * ev)
{
	if(ev->button != SDL_BUTTON_LEFT) return;

	if(isOverButton(ev->x, ev->y, 0)) {
		program.inpReturn = 1;
	} else {
		program.inpReturn = 0;
	}
}

void doYesNoMousemove(SDL_MouseMotionEvent * ev)
{
	// check yes
	if(isOverButton(ev->x, ev->y, 0)) {
		msgButton[0].over = 1;
	} else {
		msgButton[0].over = 0;
	}

	// check no
	if(isOverButton(ev->x, ev->y, 1)) {
		msgButton[1].over = 1;
	} else {
		msgButton[1].over = 0;
	}
}

void doOkayMousemove(SDL_MouseMotionEvent * ev)
{
	if(isOverButton(ev->x, ev->y, 0)) {
		msgButton[0].over = 1;
	} else {
		msgButton[0].over = 0;
	}
}

int boxX, boxY, boxW, boxH;
int i, textLines, boxType;
char lineData[BUFSIZ][BUFSIZ];

void drawMessageBox()
{
	// background
	boxRGBA(program.screen, boxX, boxY, boxX + boxW, boxY + boxH + 18, 0, 0, 0, 192);

	// contents
	for(i = 0; i < textLines; i++) {
		fontPrint(boxX + 4, boxY + 4 + (i * (FONTHEIGHT+2)), 0, program.colWhite, lineData[i]);
	}
	// buttons
	switch(boxType) {
		case MB_OKAY: {
			msgButton[0].bx = (((program.scrWidth * program.zoomFactor) / 2)) - ((strlen(msgBoxButtonNames[0]) * FONTWIDTH) / 2) - 1;
			msgButton[0].by = boxY + 4 + (textLines * (FONTHEIGHT+2)) + 6;
			msgButton[0].bx2 = msgButton[0].bx + (strlen(msgBoxButtonNames[0]) * FONTWIDTH);
			msgButton[0].by2 = msgButton[0].by + (FONTHEIGHT+3);

			if(msgButton[0].over) {
				boxRGBA(program.screen,
					msgButton[0].bx, msgButton[0].by, msgButton[0].bx2, msgButton[0].by2,
					msgButton[0].hr, msgButton[0].hg, msgButton[0].hb, msgButton[0].ha);
			}
			fontPrint(msgButton[0].bx, msgButton[0].by, 0, program.colWhite, msgBoxButtonNames[0]);
			rectangleRGBA(program.screen, msgButton[0].bx, msgButton[0].by, msgButton[0].bx2, msgButton[0].by2, 0, 128, 0, 255);
			break; }
		case MB_YESNO: {
			msgButton[0].bx = boxX + 4;
			msgButton[0].by = boxY + 4 + (textLines * (FONTHEIGHT+2)) + 6;
			msgButton[0].bx2 = msgButton[0].bx + (strlen(msgBoxButtonNames[1]) * FONTWIDTH);
			msgButton[0].by2 = msgButton[0].by + (FONTHEIGHT+3);
			msgButton[1].bx = (boxX + boxW) - (strlen(msgBoxButtonNames[2]) * FONTWIDTH) - 2;
			msgButton[1].by = msgButton[0].by;
			msgButton[1].bx2 = msgButton[1].bx + (strlen(msgBoxButtonNames[2]) * FONTWIDTH);
			msgButton[1].by2 = msgButton[1].by + (FONTHEIGHT+3);

			if(msgButton[0].over) {
				boxRGBA(program.screen,
					msgButton[0].bx, msgButton[0].by, msgButton[0].bx2, msgButton[0].by2,
					msgButton[0].hr, msgButton[0].hg, msgButton[0].hb, msgButton[0].ha);
			}
			if(msgButton[1].over) {
				boxRGBA(program.screen,
					msgButton[1].bx, msgButton[1].by, msgButton[1].bx2, msgButton[1].by2,
					msgButton[1].hr, msgButton[1].hg, msgButton[1].hb, msgButton[1].ha);
			}
			fontPrint(msgButton[0].bx, msgButton[0].by, 0, program.colWhite, msgBoxButtonNames[1]);
			fontPrint(msgButton[1].bx, msgButton[1].by, 0, program.colWhite, msgBoxButtonNames[2]);
			rectangleRGBA(program.screen, msgButton[0].bx, msgButton[0].by, msgButton[0].bx2, msgButton[0].by2, 0, 128, 0, 255);
			rectangleRGBA(program.screen, msgButton[1].bx, msgButton[1].by, msgButton[1].bx2, msgButton[1].by2, 0, 128, 0, 255);
			break; }
	}
	hlineRGBA(program.screen, boxX, boxX + boxW, (boxY + (textLines * (FONTHEIGHT+2)) + 7), 0, 128, 0, 255);

	// border
	rectangleRGBA(program.screen, boxX, boxY, boxX + boxW, boxY + boxH + 18, 0, 128, 0, 255);
}

void messageBox(char * text, int type, void * keydown, void * mousedown, void * mousemove, void (*callback)())
{
	textLines = 0; boxType = type;

	unsigned int maxLineLen = 1;
	char text2[BUFSIZ];
	strcpy(text2, text);
	char *p;
	p = strtok(text2, "|");
	while(p != NULL) {
		strcpy(lineData[textLines++], p);
		p = strtok(NULL, "|");
	}

	for(i = 0; i < textLines; i++) {
		if(strlen(lineData[i]) >= maxLineLen) maxLineLen = strlen(lineData[i]);
	}

	int textlen = ((maxLineLen + 1) * FONTWIDTH) + 1;
	boxW = textlen;
	boxH = (textLines * (FONTHEIGHT+2)) + 8;
	boxX = (((program.scrWidth * program.zoomFactor) / 2)) - (textlen / 2) - 1;
	boxY = (((program.scrHeight * program.zoomFactor) / 2)) - (boxH / 2) - 10;

	void * oldkd = program.func_keydown;
	void * oldmd = program.func_mousedown;
	void * oldmm = program.func_mousemove;
	program.func_keydown = keydown;
	program.func_mousedown = mousedown;
	program.func_mousemove = mousemove;

	msgButton[0].hr = 64;
	msgButton[0].hg = 128;
	msgButton[0].hb = 64;
	msgButton[0].ha = 128;

	msgButton[1].hr = 64;
	msgButton[1].hg = 128;
	msgButton[1].hb = 64;
	msgButton[1].ha = 128;

	msgButton[0].over = 0;
	msgButton[1].over = 0;
	if(boxType == MB_OKAY) msgButton[0].over = 1;
	if(boxType == MB_YESNO) msgButton[0].over = 1;

	program.func_draw_box = drawMessageBox;

	while(!doSDLEvents()) {
		if(program.func_draw_main != NULL) program.func_draw_main();
		if(program.func_draw_gui != NULL) program.func_draw_gui();
		if(program.func_draw_box != NULL) program.func_draw_box();

		endDrawing();
	}

	program.func_draw_box = NULL;

	if(callback != NULL) callback();

	program.func_keydown = oldkd;
	program.func_mousedown = oldmd;
	program.func_mousemove = oldmm;
	program.inpReturn = -1;
}
