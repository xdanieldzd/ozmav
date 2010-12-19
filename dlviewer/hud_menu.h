typedef struct {
	char Name[256];
	short * Value;
	int Type;
	int Disp;
} __zHUDMenuEntry;

extern void hudMenu_Init();
extern void hudMenu_HandleInput(__zHUDMenuEntry * Menu, int Len);
extern void hudMenu_Render(char Title[], int X, int Y, __zHUDMenuEntry * Menu, int Len);
