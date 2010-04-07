typedef struct {
	int ParentHandle;
	int Handle;

	int Type;
	int y, x;
	int Order;

	void * ObjParameters;
	int * Value;
} __MSK_UI_Object_Data;

typedef struct {
	int ParentHandle;
	int Handle;

	int h, w, y, x;
	char * Title;
	int BkgColor;
	int HLColor;

	int ObjCount[2];			// set automatically, 0 = total, 1 = selectable
	int ObjSelected[2];			// 0 = total, 1 = selectable

	WINDOW * Win;
	WINDOW * WinShade;
} __MSK_UI_Dialog_Data;

void MSK_DoEvents_Dialog();
void MSK_UI_Dialog_Draw(__MSK_UI_Dialog_Data * Dlg);
void MSK_UI_Dialog_CalculateObjCount(__MSK_UI_Dialog_Data * Dlg);
int MSK_UI_Dialog_ObjNumSelect_GetCount(__MSK_UI_Object_Data * Obj);
int MSK_UI_Dialog_ObjNumSelect_GetDisplayType(__MSK_UI_Object_Data * Obj);
bool MSK_UI_Dialog_ObjButton_IsOkay(__MSK_UI_Object_Data * Obj);
void MSK_UI_Dialog_SyncSelection(__MSK_UI_Dialog_Data * Dlg);
int MSK_UI_Dialog_GetFreeDlgHandle();
void MSK_UI_Dialog_DestroyDlgHandle(int DstHandle);
int MSK_UI_Dialog_GetFreeObject();
void MSK_UI_Dialog_DestroyObject(int ObjHandle);

extern __MSK_UI_Dialog_Data Dialog[512];
extern __MSK_UI_Object_Data Object[512];
extern __MSK_UI_Dialog_Data Dialog_Active;
extern int ObjectValue[512];
