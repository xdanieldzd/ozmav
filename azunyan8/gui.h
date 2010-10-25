#define MB_OKAY		0
#define MB_YESNO	1

int fontPrint(int x, int y, int hasBg, SDL_Color color, char * fmt, ...);
void addStatMessage(char * text);
void deleteStatMessage(int id);
void deleteAllStatMessages();
void drawStatMessages();
void doYesNoKeydown(SDL_KeyboardEvent * ev);
void doOkayKeydown(SDL_KeyboardEvent * ev);
void doYesNoMousedown(SDL_MouseButtonEvent * ev);
void doOkayMousedown(SDL_MouseButtonEvent * ev);
void doYesNoMousemove(SDL_MouseMotionEvent * ev);
void doOkayMousemove(SDL_MouseMotionEvent * ev);
void messageBox(char * text, int type, void * keydown, void * mousedown, void * mousemove, void (*callback)());
