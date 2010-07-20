#include "misaka.h"
#include "msk_base.h"
#include "msk_console.h"
#include "msk_ui.h"

short ObjectValue[512];
bool ObjectValueSet[512];

int MSK_MessageBox(char * Title, char * Text, int Type)
{
	__MSK_UI_Dialog_Data Temp;

	int FreeObj = 0;
	int i = 0;

	memset(&Temp, 0x00, sizeof(Temp));

	Temp.ParentHandle = Console.Handle;
	Temp.Handle = rand();

//	MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "NEW MESSAGEBOX: parenthandle: %i, handle: %i\n", Temp.ParentHandle, Temp.Handle);

	int TextLines = 0, MaxLineLength = 1;
	char LineData[256][256];

	char Text2[1024];
	strcpy(Text2, Text);

	char * Ptr;
	Ptr = strtok(Text2, "|");
	while (Ptr != NULL) {
		strcpy(LineData[TextLines++], Ptr);
		Ptr = strtok(NULL, "|");
	}

	// base message box settings
	Temp.y = -1;
	Temp.x = -1;
	Temp.Title = Title;
	Temp.BkgColor = 5;
	Temp.HLColor = 4;

	switch(Type) {
		// ok-only box
		case MSK_UI_MSGBOX_OK:
			// button
			FreeObj = MSK_UI_Dialog_GetFreeObject();
			Object[FreeObj].ParentHandle = Temp.Handle;
			Object[FreeObj].Handle = rand();
			Object[FreeObj].Type = MSK_UI_DLGOBJ_BUTTON;
			Object[FreeObj].y = -1;
			Object[FreeObj].x = -1;
			Object[FreeObj].Order = 0;
			Object[FreeObj].ObjParameters = "OK|1";
			Object[FreeObj].Value = NULL;

			// labels
			for(i = 0; i < TextLines; i++) {
				if(strlen(LineData[i]) >= MaxLineLength) MaxLineLength = strlen(LineData[i]);

				FreeObj = MSK_UI_Dialog_GetFreeObject();
				Object[FreeObj].ParentHandle = Temp.Handle;
				Object[FreeObj].Handle = rand();
				Object[FreeObj].Type = MSK_UI_DLGOBJ_LABEL;
				Object[FreeObj].y = (i * 2) + 1;
				Object[FreeObj].x = 1;
				Object[FreeObj].Order = -1;
				Object[FreeObj].ObjParameters = LineData[i];
				Object[FreeObj].Value = NULL;
			}
			break;

		// yes/no box
		case MSK_UI_MSGBOX_YESNO:
			// button yes
			FreeObj = MSK_UI_Dialog_GetFreeObject();
			Object[FreeObj].ParentHandle = Temp.Handle;
			Object[FreeObj].Handle = rand();
			Object[FreeObj].Type = MSK_UI_DLGOBJ_BUTTON;
			Object[FreeObj].y = -1;
			Object[FreeObj].x = 1;
			Object[FreeObj].Order = 1;
			Object[FreeObj].ObjParameters = "Yes|1";
			Object[FreeObj].Value = NULL;

			// button no
			FreeObj = MSK_UI_Dialog_GetFreeObject();
			Object[FreeObj].ParentHandle = Temp.Handle;
			Object[FreeObj].Handle = rand();
			Object[FreeObj].Type = MSK_UI_DLGOBJ_BUTTON;
			Object[FreeObj].y = -1;
			Object[FreeObj].x = -1;
			Object[FreeObj].Order = 0;
			Object[FreeObj].ObjParameters = "No|0";
			Object[FreeObj].Value = NULL;

			// labels
			for(i = 0; i < TextLines; i++) {
				if(strlen(LineData[i]) >= MaxLineLength) MaxLineLength = strlen(LineData[i]);

				FreeObj = MSK_UI_Dialog_GetFreeObject();
				Object[FreeObj].ParentHandle = Temp.Handle;
				Object[FreeObj].Handle = rand();
				Object[FreeObj].Type = MSK_UI_DLGOBJ_LABEL;
				Object[FreeObj].y = (i * 2) + 1;
				Object[FreeObj].x = 1;
				Object[FreeObj].Order = -1;
				Object[FreeObj].ObjParameters = LineData[i];
				Object[FreeObj].Value = NULL;
				FreeObj++;
			}
			break;

		default:
//			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "ERROR WHILE OBJECT INIT!!\n");
			return -1;
	}

//	MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "...done\n");

	Temp.h = (TextLines*2) + 4;
	Temp.w = MaxLineLength + 4;

	i = MSK_UI_Dialog_GetFreeDlgHandle();
	if(i == -1) {
//		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "ERROR NO FREE DLGHANDLE!!\n");
		return -1;
	} else {
//		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "dlghandle: %i\n", i);
	}

//	MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "finishing up box\n");

	memcpy(&Dialog[i], &Temp, sizeof(__MSK_UI_Dialog_Data));
	Dialog_Active = Dialog[i];

	for(i = 0; i < ArraySize(ObjectValue); i++) {
		ObjectValue[i] = 0;
		ObjectValueSet[i] = 0;
	}

//	MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "box %i compiled, setting mainfunc to dlg doevents\n", Dialog[i].Win);
	MSK_SetMainFunction(MSK_DoEvents_Dialog);

	return Dialog_Active.Handle;
}

int MSK_Dialog(__MSK_UI_Dialog * Dlg)
{
	__MSK_UI_Dialog_Data Temp;

	int FreeObj = -1;

	memset(&Temp, 0x00, sizeof(Temp));

	Temp.ParentHandle = Console.Handle;
	Temp.Handle = rand();

	Temp.y = -1;
	Temp.x = -1;
	Temp.Title = Dlg->Title;
	Temp.BkgColor = 5;
	Temp.HLColor = 4;

	Temp.h = Dlg->h;
	Temp.w = Dlg->w;

	int i = 0;
	while(i < ArraySize(Dlg->Object)) {
		FreeObj = MSK_UI_Dialog_GetFreeObject();

		Object[FreeObj].ParentHandle = Temp.Handle;
		Object[FreeObj].Handle = rand();
		Object[FreeObj].Type = Dlg->Object[i].Type;
		Object[FreeObj].y = Dlg->Object[i].y;
		Object[FreeObj].x = Dlg->Object[i].x;
		Object[FreeObj].Order = Dlg->Object[i].Order;
		Object[FreeObj].ObjParameters = Dlg->Object[i].ObjParameters;
		Object[FreeObj].Value = Dlg->Object[i].Value;
		i++;
	}

	int FreeDlgH = MSK_UI_Dialog_GetFreeDlgHandle();
	if(FreeDlgH == -1) return -1;

	memcpy(&Dialog[FreeDlgH], &Temp, sizeof(__MSK_UI_Dialog_Data));
	Dialog_Active = Dialog[FreeDlgH];

	for(i = 0; i < ArraySize(ObjectValue); i++) {
		ObjectValue[i] = 0;
		ObjectValueSet[i] = 0;
	}

	MSK_SetMainFunction(MSK_DoEvents_Dialog);

	return Dialog_Active.Handle;
}

void MSK_DoEvents_Dialog()
{
	MSK_UI_Dialog_Draw(&Dialog_Active);

	if(!kbhit()) return;

	int OnObject = Dialog_Active.ObjSelected[0];

	int Character = getch();

	bool IsOkay = FALSE;
	bool ExitDlg = FALSE;
	switch(Character) {
		// exit dialog
		case ESCAPE_KEY:
			ExitDlg = TRUE;
			break;

		// select prev object
		case KEY_UP:
			if(Dialog_Active.ObjSelected[1] > 0) {
				Dialog_Active.ObjSelected[1]--;
			} else {
				Dialog_Active.ObjSelected[1] = Dialog_Active.ObjCount[1];
			}
			break;

		// select next object
		case KEY_DOWN:
			if(Dialog_Active.ObjSelected[1] < Dialog_Active.ObjCount[1]) {
				Dialog_Active.ObjSelected[1]++;
			} else {
				Dialog_Active.ObjSelected[1] = 0;
			}
			break;

		case KEY_LEFT:
			if(Object[OnObject].Type == MSK_UI_DLGOBJ_NUMBERSEL) {
				// select prev option in object
				// see if negative vals allowed
				bool AllowNegative = MSK_UI_Dialog_ObjNumSelect_GetAllowNegativeFlag(&Object[OnObject]);
				// if not neg allowed AND value is neg, reset to org value
				if((!AllowNegative) && (ObjectValue[OnObject] < 0)) ObjectValue[OnObject] = *Object[OnObject].Value;
				// get max value
				int Total = MSK_UI_Dialog_ObjNumSelect_GetCount(&Object[OnObject]) - 1;
				// set min value
				int MinValue = 0;
				// if neg allowed, set min value to (total / 2) - total
				if(AllowNegative) MinValue = (Total / 2) - Total;
				if(ObjectValue[OnObject] > MinValue) ObjectValue[OnObject]--;

			// select prev object
			} else if((Object[OnObject].Type == MSK_UI_DLGOBJ_BUTTON) || (Object[OnObject].Type == MSK_UI_DLGOBJ_CHECKBOX)) {
				if(Dialog_Active.ObjSelected[1] > 0) {
					Dialog_Active.ObjSelected[1]--;
				} else {
					Dialog_Active.ObjSelected[1] = Dialog_Active.ObjCount[1];
				}
			}
			break;

		case KEY_RIGHT:
			// select next option in object
			if(Object[OnObject].Type == MSK_UI_DLGOBJ_NUMBERSEL) {
				bool AllowNegative = MSK_UI_Dialog_ObjNumSelect_GetAllowNegativeFlag(&Object[OnObject]);
				if((!AllowNegative) && (ObjectValue[OnObject] < 0)) ObjectValue[OnObject] = *Object[OnObject].Value;
				int Total = MSK_UI_Dialog_ObjNumSelect_GetCount(&Object[OnObject]) - 1;
				int MaxValue = 0;
				if(AllowNegative) MaxValue = (Total / 2);
				if(ObjectValue[OnObject] < MaxValue) ObjectValue[OnObject]++;

			// select next object
			} else if((Object[OnObject].Type == MSK_UI_DLGOBJ_BUTTON) || (Object[OnObject].Type == MSK_UI_DLGOBJ_CHECKBOX)) {
				if(Dialog_Active.ObjSelected[1] < Dialog_Active.ObjCount[1]) {
					Dialog_Active.ObjSelected[1]++;
				} else {
					Dialog_Active.ObjSelected[1] = 0;
				}
			}
			break;

		// toggle object
		case ' ':
			// toggle checkbox state
			if(Object[OnObject].Type == MSK_UI_DLGOBJ_CHECKBOX) {
				if(ObjectValueSet[OnObject] == 0) {
					ObjectValue[OnObject] = *Object[OnObject].Value;
					ObjectValueSet[OnObject] = 1;
				}
				ObjectValue[OnObject] = !ObjectValue[OnObject];
			}
			break;

		// activate object
		case '\n':
		case PADENTER:
			// if button
			if(Object[OnObject].Type == MSK_UI_DLGOBJ_BUTTON) {
				ExitDlg = TRUE;
				if(MSK_UI_Dialog_ObjButton_IsOkay(&Object[OnObject])) IsOkay = TRUE;
			}
			break;
	}

	if(ExitDlg) {
		Dialog_Active.ObjSelected[0] = 0;
		Dialog_Active.ObjSelected[1] = 0;

		MSK_Refresh(Console.CurrentConsoleLine - 1);

		ReturnVal.Handle = Dialog_Active.Handle;
		ReturnVal.s8 = Object[OnObject].Order;

		int i = 0;
		while(i < ArraySize(Object)) {
			if(IsOkay && (!(Object[i].Value == NULL)) && ObjectValueSet[i]) {
				*Object[i].Value = ObjectValue[i];
			}

			if(Object[i].ParentHandle == Dialog_Active.Handle) {
				if(Object[i].ObjParameters == NULL) break;

				MSK_UI_Dialog_DestroyObject(i);
			}
			i++;
		}

		MSK_UI_Dialog_DestroyDlgHandle(Dialog_Active.Handle);

		MSK_RestoreMainFunction();

		if((Dialog_Active.Handle == Console.ExitHandle) && (IsOkay)) Console.IsRunning = FALSE;
	}
}

void MSK_UI_Dialog_Draw(__MSK_UI_Dialog_Data * Dlg)
{
	int y = Dlg->y;
	int x = Dlg->x;
	if(y == -1) y = (LINES / 2) - (Dlg->h / 2);
	if(x == -1) x = (COLS / 2) - (Dlg->w / 2);
	Dlg->Win = newwin(Dlg->h, Dlg->w, y, x);
	Dlg->WinShade = newwin(Dlg->h, Dlg->w, y + 1, x + 1);

	wbkgd(Dlg->Win, COLOR_PAIR(Dlg->BkgColor));
	wbkgd(Dlg->WinShade, COLOR_PAIR(Dlg->HLColor));

	box(Dlg->Win, 0, 0);
	mvwhline(Dlg->Win, 0, 0, ACS_CKBOARD, Dlg->w);

	wmove(Dlg->Win, 0, 1);
	wprintw(Dlg->Win, "[%s]", Dlg->Title);

	char * EscMsg = "[X]";
	wmove(Dlg->Win, 0, Dlg->w - strlen(EscMsg) - 1);
	wprintw(Dlg->Win, EscMsg);

	int ObjCountTotal = 0;
	int ObjCountSelectable = 0;

	int i = 0;
	while(i < ArraySize(Object)) {
		if(Object[i].ParentHandle == Dlg->Handle) {
			if(Object[i].ObjParameters == NULL) break;

			ObjCountTotal++;
			if(Object[i].Order != -1) ObjCountSelectable++;

			switch(Object[i].Type) {
				case MSK_UI_DLGOBJ_LABEL:
					wmove(Dlg->Win, Object[i].y + 1, Object[i].x + 1);
					wprintw(Dlg->Win, Object[i].ObjParameters);
					break;

				case MSK_UI_DLGOBJ_BUTTON:
					if(Dlg->ObjSelected[1] == Object[i].Order) wattron(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
					char Text[64];
					strcpy(Text, Object[i].ObjParameters);
					char *Ptr;
					if((Ptr = strstr(Text, "|"))) *Ptr = 0x00;

					if(Object[i].y == -1) Object[i].y = Dlg->h - 3;
					if(Object[i].x == -1) Object[i].x = (Dlg->w - strlen(Text) - 9);
					wmove(Dlg->Win, Object[i].y + 1, Object[i].x + 1);
					wprintw(Dlg->Win, "[  %s  ]", Text);
					if(Dlg->ObjSelected[1] == Object[i].Order) wattroff(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
					break;

				case MSK_UI_DLGOBJ_CHECKBOX:
					if(Dlg->ObjSelected[1] == Object[i].Order) wattron(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
					wmove(Dlg->Win, Object[i].y + 1, Object[i].x + 1);
					if(ObjectValueSet[i] == 0) {
						ObjectValue[i] = *Object[i].Value;
						ObjectValueSet[i] = 1;
					}
					wprintw(Dlg->Win, "[%s]", ObjectValue[i] ? "X" : " ");
					if(Dlg->ObjSelected[1] == Object[i].Order) wattroff(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
					wprintw(Dlg->Win, " %s", Object[i].ObjParameters);
					break;

				case MSK_UI_DLGOBJ_NUMBERSEL: {
					wmove(Dlg->Win, Object[i].y + 1, Object[i].x + 1);
					char Text[64];
					strcpy(Text, Object[i].ObjParameters);
					char *Ptr;
					if((Ptr = strstr(Text, "|"))) *Ptr = 0x00;

					int Current = 0;
					int Amount = MSK_UI_Dialog_ObjNumSelect_GetCount(&Object[i]);

					wprintw(Dlg->Win, "%s : ", Text);

					if(MSK_UI_Dialog_ObjNumSelect_GetDisplayType(&Object[i]) == 1) {
						if(ObjectValueSet[i] == 0) {
							ObjectValue[i] = *Object[i].Value;
							ObjectValueSet[i] = 1;
						}
						if(Dlg->ObjSelected[1] == Object[i].Order) wattron(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
						char Format[64];
						sprintf(Format, "%5i", ObjectValue[i]);
						if(MSK_UI_Dialog_ObjNumSelect_GetStringFormat(&Object[i]) == 1) sprintf(Format, "0x%04X", ObjectValue[i]);
						wprintw(Dlg->Win, "< %s >", Format);
						if(Dlg->ObjSelected[1] == Object[i].Order) wattroff(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
					} else {
						for(Current = 0; Current < Amount; Current++) {
							if(ObjectValueSet[i] == 0) {
								ObjectValue[i] = *Object[i].Value;
								ObjectValueSet[i] = 1;
							}
							if(ObjectValue[i] == Current) {
								if(Dlg->ObjSelected[1] == Object[i].Order) wattron(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
								wprintw(Dlg->Win, "[ %i ]", Current);
							} else {
								wprintw(Dlg->Win, "  %i  ", Current);
							}
							if((Dlg->ObjSelected[1] == Object[i].Order) && (ObjectValue[i] == Current)) wattroff(Dlg->Win, COLOR_PAIR(Dlg->HLColor));
						}
					}

					break; }

				case MSK_UI_DLGOBJ_LINE: {
					int Length = 0;
					sscanf(Object[i].ObjParameters, "%i", &Length);
					if(Length == -1) Length = Dlg->w - 4;
					mvwhline(Dlg->Win, Object[i].y + 1, Object[i].x + 1, ACS_HLINE, Length);
					break; }
			}
		}

		i++;
	}

	Dlg->ObjCount[0] = ObjCountTotal;
	Dlg->ObjCount[1] = ObjCountSelectable - 1;

	MSK_UI_Dialog_SyncSelection(Dlg);

//	wmove(Dlg->Win, 1, 1);
//	wprintw(Dlg->Win, "true: %i %i  sel: %i %i", Dlg->ObjSelected[0], Dlg->ObjCount[0], Dlg->ObjSelected[1], Dlg->ObjCount[1]);

	wnoutrefresh(Dlg->WinShade);
	wnoutrefresh(Dlg->Win);
	refresh();

	delwin(Dlg->WinShade);
	delwin(Dlg->Win);
}

void MSK_UI_Dialog_CalculateObjCount(__MSK_UI_Dialog_Data * Dlg)
{
	int i = 0;
	while(i < ArraySize(Object)) {
		if(Object[i].ParentHandle == Dlg->Handle) Dlg->ObjCount[0]++;
		if(Object[i].Order != -1) Dlg->ObjCount[1]++;
		i++;
	}
}

int MSK_UI_Dialog_ObjNumSelect_GetCount(__MSK_UI_Object_Data * Obj)
{
	int Ret = 0;

	char * Ptr = strchr(Obj->ObjParameters, '|');
	sscanf(Ptr+1, "%i", &Ret);

	return Ret;
}

int MSK_UI_Dialog_ObjNumSelect_GetDisplayType(__MSK_UI_Object_Data * Obj)
{
	int Ret = 0;

	char * Ptr = strchr(Obj->ObjParameters, '|');
	Ptr = strchr(Ptr+1, '|');
	sscanf(Ptr+1, "%i", &Ret);

	return Ret;
}

int MSK_UI_Dialog_ObjNumSelect_GetStringFormat(__MSK_UI_Object_Data * Obj)
{
	int Ret = 0;

	char * Ptr = strchr(Obj->ObjParameters, '|');
	Ptr = strchr(Ptr+1, '|');
	Ptr = strchr(Ptr+1, '|');
	sscanf(Ptr+1, "%i", &Ret);

	return Ret;
}

int MSK_UI_Dialog_ObjNumSelect_GetAllowNegativeFlag(__MSK_UI_Object_Data * Obj)
{
	int Ret = 0;

	char * Ptr = strchr(Obj->ObjParameters, '|');
	Ptr = strchr(Ptr+1, '|');
	Ptr = strchr(Ptr+1, '|');
	Ptr = strchr(Ptr+1, '|');
	sscanf(Ptr+1, "%i", &Ret);

	return Ret;
}

bool MSK_UI_Dialog_ObjButton_IsOkay(__MSK_UI_Object_Data * Obj)
{
	int Get = 0;

	char Format[64];
	sprintf(Format, "%%*%dc|%%i", (strlen(Obj->ObjParameters) - 2));
	sscanf(Obj->ObjParameters, Format, &Get);

//	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "'%s' -> '%s' -> '%i'\n", Format, Obj->ObjParameters, Get);

	return (bool)Get;
}

void MSK_UI_Dialog_SyncSelection(__MSK_UI_Dialog_Data * Dlg)
{
	int i = 0;
	while(i < ArraySize(Object)) {
		if(Object[i].ParentHandle == Dlg->Handle) {
			if(Object[i].Order == Dlg->ObjSelected[1]) {
				Dlg->ObjSelected[0] = i;
				break;
			}
		}

		i++;
	}
}

int MSK_UI_Dialog_GetFreeDlgHandle()
{
	int i = 0;
	while(i < ArraySize(Dialog)) {
		if(Dialog[i].Handle == -1)
			return i;
		i++;
	}

	return -1;
}

void MSK_UI_Dialog_DestroyDlgHandle(int DstHandle)
{
	int i = 0;
	while(i < ArraySize(Dialog)) {
		if(Dialog[i].Handle == DstHandle) {
			memset(&Dialog[i], -1, sizeof(Dialog[i]));
			Dialog[i].Handle = -1;
			break;
		}
	}
}

int MSK_UI_Dialog_GetFreeObject()
{
	int i = 0;
	while(i < ArraySize(Object)) {
		if(Object[i].ObjParameters == NULL)
			return i;
		i++;
	}

	return -1;
}

void MSK_UI_Dialog_DestroyObject(int ObjHandle)
{
	memset(&Object[ObjHandle], -1, sizeof(Object[ObjHandle]));
	Object[ObjHandle].ObjParameters = NULL;
}
