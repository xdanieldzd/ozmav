typedef struct {
	int Handle;
	signed char s8;
	unsigned char u8;
	signed short int s16;
	unsigned short int u16;
	signed int s32;
	unsigned int u32;
	signed long long s64;
	unsigned long long u64;
	char * str;
} __MSK_Return;

typedef struct {
	int Type;
	int y, x;
	int Order;

	void * ObjParameters;
	short * Value;
} __MSK_UI_Object;

typedef struct {
	char * Title;
	int h, w;

	__MSK_UI_Object Object[512];
} __MSK_UI_Dialog;

extern __MSK_Return ReturnVal;

extern int MSK_Init(char * Apptitle);
extern void MSK_SetValidCharacters(char * Chars);
extern void MSK_AddCommand(char * Cmd, char * Help, void * Func);
extern void MSK_Exit();
extern void MSK_ConsolePrint(int Color, char * Format, ...);
extern void MSK_InitLogging(char * Path);
extern void MSK_SetLogging(int Toggle);
extern int MSK_DoEvents();
extern int MSK_MessageBox(char * Title, char * Text, int Type);
extern int MSK_Dialog(__MSK_UI_Dialog * Dlg);

typedef enum {
	MSK_COLORTYPE_INFO, MSK_COLORTYPE_OKAY, MSK_COLORTYPE_WARNING, MSK_COLORTYPE_ERROR
} __MSK_Console_Colors;

typedef enum {
	MSK_UI_DLGOBJ_LABEL,
	MSK_UI_DLGOBJ_BUTTON,
	MSK_UI_DLGOBJ_CHECKBOX,
	MSK_UI_DLGOBJ_NUMBERSEL,
	MSK_UI_DLGOBJ_LINE
} __MSK_UI_ObjTypes;

typedef enum {
	MSK_UI_MSGBOX_OK, MSK_UI_MSGBOX_YESNO
} __MSK_UI_MsgBoxTypes;

typedef enum {
	MSK_UI_MSGBOX_RETOKYES, MSK_UI_MSGBOX_RETNO
} __MSK_UI_MsgBoxRetTypes;
