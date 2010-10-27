/*
	filebrowse.c - file browser
*/

#include "globals.h"

#define BASE_PAGESIZE		16
#define BASE_BROWSERWIDTH	45
#define MAX_ENTRIES			1000

typedef struct {
	char name[MAX_PATH];
	int isDir;
} __fileEntry;
__fileEntry fileEntry[MAX_ENTRIES];

typedef struct {
	int bx, by, bx2, by2;
	int hr, hg, hb, ha;
	int over;
} __fileEntrySelect;
__fileEntrySelect fileEntrySelect[MAX_ENTRIES];

char selFilename[MAX_PATH] = "\0";
char currentDir[MAX_PATH];

int fileEntriesTotal = 0;
int fileDirSelected = 0;

int currentSel = 0;
int currentPage = 0;
int lastPage = 0;

int pageSize = 0;
int browserWidth = 0;

int minEntry = 0, maxEntry = 0;

int isEntryFile(char * filename)
{
	struct stat buf;
	if(stat(filename, &buf) == -1) return -1;

	if(buf.st_mode & S_IFDIR) return 0;
	if(buf.st_mode & S_IFREG) return 1;

	return -1;
}

int cacheDirectory()
{
	DIR * dir;
	struct dirent *dirptr;
	memset(&dirptr, 0, sizeof(dirptr));
	memset(fileEntry, 0, MAX_ENTRIES);

	int count = 0;

	if((dir = opendir(currentDir)) == NULL) return EXIT_FAILURE;
	while((dirptr = readdir(dir)) != NULL) {
		if(!(isEntryFile(parseFormat("%s%c%s", currentDir, FILESEP, (*dirptr).d_name)))) {
			strcpy(fileEntry[count].name, (*dirptr).d_name);
			fileEntry[count].isDir = 0;
			count++;

			if(count > MAX_ENTRIES - 1) count = MAX_ENTRIES - 1;
			fileEntriesTotal = count;
			if(fileEntriesTotal < 0) fileEntriesTotal = 0;
		}
	}
	closedir(dir);

	if((dir = opendir(currentDir)) == NULL) return EXIT_FAILURE;
	while((dirptr = readdir(dir)) != NULL) {
		if(isEntryFile(parseFormat("%s%c%s", currentDir, FILESEP, (*dirptr).d_name))) {
			strcpy(fileEntry[count].name, (*dirptr).d_name);
			fileEntry[count].isDir = 1;
			count++;

			if(count > MAX_ENTRIES - 1) count = MAX_ENTRIES - 1;
			fileEntriesTotal = count;
			if(fileEntriesTotal < 0) fileEntriesTotal = 0;
		}
	}
	closedir(dir);

	if(currentSel > fileEntriesTotal - 1) currentSel = fileEntriesTotal - 1;

	return EXIT_SUCCESS;
}

void doBrowserInput(SDL_KeyboardEvent * ev)
{
	switch(ev->keysym.sym) {
		case SDLK_ESCAPE: {
			if(!program.isPaused) {
				program.inpReturn = 2;
			} else {
				exitProgram(EXIT_SUCCESS);
			}
			break; }

		case SDLK_KP_ENTER:
		case SDLK_RETURN: {
			char temp[200];
			strcpy(temp, currentDir);

			if(strcmp(".", fileEntry[currentSel].name) != 0 && strcmp( "..", fileEntry[currentSel].name) != 0) {
				if(currentDir[strlen(currentDir) - 1] != FILESEP) strcat(temp, parseFormat("%c", FILESEP));
				strcat(temp, fileEntry[currentSel].name);
				if(isEntryFile(temp)) {
					strcpy(selFilename, temp);
					strcpy(program.lastpath, currentDir);
					program.inpReturn = 1;
				} else {
					currentSel = 0;
					strcpy(currentDir, temp);
					strcat(currentDir, parseFormat("%c", FILESEP));
					cacheDirectory();
					currentSel = 0;
				}
			} else if(strcmp(".", fileEntry[currentSel].name) == 0) {
				getRoot(program.apppath, currentDir);
				cacheDirectory();
				currentSel = 0;
			} else if(strcmp("..", fileEntry[currentSel].name) == 0) {
				int i = 0;
				int max = strlen(currentDir);
				int dir_no = 0;
				while((i < max)) {
					if(currentDir[i] == FILESEP) dir_no++;
					i++;
				}

				if(dir_no > 1) {
					char * temp2 = strtok(currentDir, parseFormat("%c", FILESEP));
					strcpy(temp, temp2);
					strcat(temp, parseFormat("%c", FILESEP));
					i = 2;
					while((i < dir_no)) {
						temp2 = strtok(NULL, parseFormat("%c", FILESEP));
						strcat(temp, temp2);
						strcat(temp, parseFormat("%c", FILESEP));
						i++;
					}
					strcpy(currentDir, temp);
				}
				cacheDirectory();
				currentSel = 0;
			}
			break; }

		case SDLK_RIGHT: {
			if(fileEntriesTotal > pageSize) {
				currentSel += pageSize;
				if(currentSel > fileEntriesTotal - 1) currentSel = fileEntriesTotal - 1;
			} else {
				currentSel = fileEntriesTotal - 1;
			}
			break; }
		case SDLK_DOWN: {
			currentSel++;
			if(currentSel > fileEntriesTotal - 1) currentSel = fileEntriesTotal - 1;
			break; }

		case SDLK_LEFT: {
			if(fileEntriesTotal > pageSize) {
				currentSel -= pageSize;
				if(currentSel < 0) currentSel = 0;
			} else {
				currentSel = 0;
			}
			break; }
		case SDLK_UP: {
			currentSel--;
			if(currentSel < 0) currentSel = 0;
			break; }

		case SDLK_HOME: {
			currentSel = 0;
			break; }
		case SDLK_END: {
			currentSel = fileEntriesTotal - 1;
			break; }
		default:
			break;
	}

	int i = 0;
	for(i = minEntry; ((i < fileEntriesTotal) && (i < maxEntry)); i++) {
		fileEntrySelect[i].over = 0;
	}
	fileEntrySelect[currentSel].over = 1;

	if(program.inpReturn == 2) strcpy(selFilename, program.filename);
}

void makeAbbrevFilename(char * target, char * orgFilename, int gap)
{
	memset(target, 0, sizeof(target));

	int orgLen = strlen(orgFilename);

	int part1Len = (browserWidth / 3) + 5;
	int part3Pos = -(browserWidth - part1Len - orgLen) + 4;
	// weird but works

	if(orgLen >= browserWidth - gap) {
		strncpy(target, orgFilename, part1Len - gap);
		strcpy(&target[part1Len - gap], "...");
		strcpy(&target[(part1Len + 3) - gap], &orgFilename[part3Pos]);
	} else {
		strcpy(target, orgFilename);
	}
}

int isOverEntry(int mx, int my, int entry)
{
	if(	(mx > fileEntrySelect[entry].bx) && (mx < fileEntrySelect[entry].bx2) &&
		(my > fileEntrySelect[entry].by) && (my < fileEntrySelect[entry].by2)) return 1;

	return 0;
}

void doBrowserMousedown(SDL_MouseButtonEvent * ev)
{
	int i = 0;
	SDL_Event new_event;

	switch(ev->button) {
		case SDL_BUTTON_LEFT: {
			for(i = minEntry; ((i < fileEntriesTotal) && (i < maxEntry)); i++) {
				if(isOverEntry(ev->x, ev->y, i)) {
					if(currentSel == i) {
						new_event.type = SDL_KEYDOWN;
						new_event.key.keysym.sym = SDLK_RETURN;
						SDL_PushEvent(&new_event);
					} else {
						currentSel = i;
					}
				}
			}
			break; }

		case SDL_BUTTON_WHEELUP: {
			new_event.type = SDL_KEYDOWN;
			new_event.key.keysym.sym = SDLK_UP;
			SDL_PushEvent(&new_event);
			break; }

		case SDL_BUTTON_WHEELDOWN: {
			new_event.type = SDL_KEYDOWN;
			new_event.key.keysym.sym = SDLK_DOWN;
			SDL_PushEvent(&new_event);
			break; }
	}
}

void doBrowserMousemove(SDL_MouseMotionEvent * ev)
{
	int i = 0;
	for(i = minEntry; ((i < fileEntriesTotal) && (i < maxEntry)); i++) {
		if(isOverEntry(ev->x, ev->y, i)) {
			fileEntrySelect[i].over = 1;
		} else {
			fileEntrySelect[i].over = 0;
		}
	}
}

char tempfn[BUFSIZ];
int fbWndX, fbWndY, fbWndW, fbWndH;

void drawFileBrowser()
{
	boxRGBA(program.screen, fbWndX, fbWndY, fbWndX + fbWndW, fbWndY + fbWndH + (FONTHEIGHT_BIG + 9), 0, 0, 0, 192);
	boxRGBA(program.screen, fbWndX, fbWndY, fbWndX + fbWndW, fbWndY + (FONTHEIGHT_BIG + 10), 0, 64, 0, 224);

	int i = 0, scrpos = 17;

	currentPage = currentSel / pageSize;

	makeAbbrevFilename(tempfn, currentDir, 5);
	fontPrint(fbWndX + 4, fbWndY + 4, 0, program.colWhite, program.ttff_big, parseFormat("Dir: %s", tempfn));
	hlineRGBA(program.screen, fbWndX, fbWndX + fbWndW, fbWndY + (FONTHEIGHT_BIG + 10), 0, 128, 0, 255);

	minEntry = 0;
	if(currentSel > (pageSize - 1) / 2) minEntry = currentSel - (pageSize - 1) / 2;

	maxEntry = (minEntry + pageSize);
	if(currentSel > fileEntriesTotal - 1 - (pageSize / 2)) {
		minEntry = fileEntriesTotal - 1 - ((pageSize - 1));
		maxEntry = fileEntriesTotal;
	}
	if(minEntry < 0) minEntry = 0;

//	fontPrint(0, 0, 0, program.colWhite, program.ttff_big, parseFormat("%d %d", minEntry, maxEntry));

	fileEntrySelect[currentSel].over = 1;
	for(i = minEntry; ((i < fileEntriesTotal) && (i < maxEntry)); i++) {
		fileEntrySelect[i].bx = fbWndX + 1;
		fileEntrySelect[i].by = fbWndY + FONTHEIGHT_BIG + scrpos;
		fileEntrySelect[i].bx2 = fbWndX + fbWndW - 1;
		fileEntrySelect[i].by2 = fbWndY + FONTHEIGHT_BIG + scrpos + FONTHEIGHT_BIG + 2;
		fileEntrySelect[i].hr = 64;
		fileEntrySelect[i].hg = 128;
		fileEntrySelect[i].hb = 64;
		fileEntrySelect[i].ha = 128;

		if(fileEntrySelect[i].over) {
			boxRGBA(program.screen,
				fileEntrySelect[i].bx, fileEntrySelect[i].by, fileEntrySelect[i].bx2, fileEntrySelect[i].by2,
				fileEntrySelect[i].hr, fileEntrySelect[i].hg, fileEntrySelect[i].hb, fileEntrySelect[i].ha);
		}
		makeAbbrevFilename(tempfn, fileEntry[i].name, 6);
		fontPrint(
			fileEntrySelect[i].bx+3, fileEntrySelect[i].by, 0, ((i == currentSel) ? program.colGreen : program.colWhite), program.ttff_big,
			parseFormat("[%s] %s", (fileEntry[i].isDir ? "   " : "DIR"), tempfn)
		);
		scrpos+=FONTHEIGHT_BIG+3;
	}
	lastPage = currentPage;

	boxRGBA(program.screen, fbWndX, fbWndY + fbWndH - 1, fbWndX + fbWndW, fbWndY + fbWndH - 1 + FONTHEIGHT_BIG + 9, 0, 64, 0, 224);
	hlineRGBA(program.screen, fbWndX, fbWndX + fbWndW, fbWndY + fbWndH - 1, 0, 128, 0, 255);

	char * Msg1 = "Cursor: Select";
	char * Msg2 = "Return: Open directory/file";
	fontPrint(fbWndX + 4, fbWndY + fbWndH + 3, 0, program.colWhite, program.ttff_big, Msg1);
	fontPrint(fbWndX + fbWndW - (strlen(Msg2) * FONTWIDTH_BIG) - 4, fbWndY + fbWndH + 3, 0, program.colWhite, program.ttff_big, Msg2);

	rectangleRGBA(program.screen, fbWndX, fbWndY, fbWndX + fbWndW, fbWndY + fbWndH + FONTHEIGHT_BIG + 9, 0, 128, 0, 255);
}

char * fileBrowser(char * initialDir)
{
	strcpy(currentDir, program.apppath);
	if(initialDir != NULL) {
		strcpy(currentDir, initialDir);
	}

	if(cacheDirectory()) return NULL;

	pageSize = BASE_PAGESIZE;
	browserWidth = BASE_BROWSERWIDTH;

	int textlen = (browserWidth * FONTWIDTH_BIG) + 1;
	fbWndW = textlen;
	fbWndH = ((pageSize + 5) * FONTHEIGHT_BIG);
	fbWndX = (((program.scrWidth) / 2)) - (textlen / 2);
	fbWndY = (((program.scrHeight) / 2)) - (fbWndH / 2) - 10;

	void * oldkd = program.func_keydown;
	void * oldmd = program.func_mousedown;
	void * oldmm = program.func_mousemove;
	program.func_keydown = doBrowserInput;
	program.func_mousedown = doBrowserMousedown;
	program.func_mousemove = doBrowserMousemove;

	program.func_draw_gui = drawFileBrowser;

	while(!doSDLEvents()) {
		if(program.func_draw_main != NULL) program.func_draw_main();
		if(program.func_draw_gui != NULL) program.func_draw_gui();
		if(program.func_draw_box != NULL) program.func_draw_box();

		endDrawing();
	}

	program.func_draw_gui = NULL;

	program.func_keydown = oldkd;
	program.func_mousedown = oldmd;
	program.func_mousemove = oldmm;
	program.inpReturn = -1;

	return selFilename;
}
