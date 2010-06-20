// place all code for level viewing shit here!

#include "globals.h"

int sv_EnableViewer(unsigned char * Ptr)
{
	if(!Program.IsROMLoaded) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded, cannot enable level viewer!\n");
		return EXIT_FAILURE;
	}

	if(Ptr == NULL) {
		Viewer.LevelID = 0;
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		Viewer.LevelID = Var;
	}

	return sv_Init();
}

int sv_Init()
{
	if(ThisGame.DMATableOffset != 0x0D9A90) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: ROM version not yet supported by viewer!");
		return EXIT_FAILURE;
	}

	sv_ClearAllSegments();
	sv_ClearStructures(true);
	gl_ClearRenderer(true);
	dl_InitCombiner();

	Viewer.LevelFile = 0;
	Viewer.ObjCount = 0;

	unsigned int Offset = Read32(ROM.Data, (0xCF1A8 + (Viewer.LevelID * 0x04)));
	unsigned char Segment = (Offset & 0xFF000000) >> 24;
	Offset = (Offset & 0x00FFFFFF);

	switch(Viewer.LevelID) {
		case 0x00: Viewer.LevelFile = 18; break;	// Corneria
		case 0x01: Viewer.LevelFile = 19; break;	// Meteo
		case 0x02: Viewer.LevelFile = 26; break;	// Sector X
		case 0x03: Viewer.LevelFile = 29; break;	// Area 6
		case 0x04: Viewer.LevelFile = 29; break;	// Glitch
		case 0x05: Viewer.LevelFile = 35; break;	// Sector Y
		case 0x06: Viewer.LevelFile = -1; break;	// Venom 1
		case 0x07: Viewer.LevelFile = -1; break;	// Solar
		case 0x08: Viewer.LevelFile = -1; break;	// Zoness
		case 0x09: Viewer.LevelFile = -1; break;	// Venom 2
		case 0x0A: Viewer.LevelFile = 53; break;	// Training Mode
		case 0x0B: Viewer.LevelFile = -1; break;	// Macbeth
		case 0x0C: Viewer.LevelFile = -1; break;	// Titania
		case 0x0D: Viewer.LevelFile = -1; break;	// Aquas
		case 0x0E: Viewer.LevelFile = -1; break;	// Fortuna
		case 0x0F: Viewer.LevelFile = -1; break;	// ????
		case 0x10: Viewer.LevelFile = -1; break;	// Katina
		case 0x11: Viewer.LevelFile = 33; break;	// Bolse
		case 0x12: Viewer.LevelFile = 27; break;	// Sector Z
		case 0x13: Viewer.LevelFile = 31; break;	// Venom (Star Wolf)
		case 0x14: Viewer.LevelFile = 12; break;	// Corneria (Multi)
		default: Viewer.LevelFile = -1; break;
	}

	if(Viewer.LevelFile == -1) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Unknown level file or invalid level!");
		return EXIT_FAILURE;
	}

	sv_LoadToSegment(Segment, ROM.Data, DMA[Viewer.LevelFile].PStart, (DMA[Viewer.LevelFile].PEnd - DMA[Viewer.LevelFile].PStart));

	Offset += 0x44;	// skip over header

	while(1) {
		if(!sv_CheckAddressValidity((Segment << 24) | Offset)) break;

		Object[Viewer.ObjCount].LvlPos = Read32(RAM[Segment].Data, Offset);
		Object[Viewer.ObjCount].Z = Read16(RAM[Segment].Data, Offset + 4);
		Object[Viewer.ObjCount].X = Read16(RAM[Segment].Data, Offset + 6);
		Object[Viewer.ObjCount].Y = Read16(RAM[Segment].Data, Offset + 8);
		Object[Viewer.ObjCount].ZRot = Read16(RAM[Segment].Data, Offset + 10);
		Object[Viewer.ObjCount].XRot = Read16(RAM[Segment].Data, Offset + 12);
		Object[Viewer.ObjCount].YRot = Read16(RAM[Segment].Data, Offset + 14);
		Object[Viewer.ObjCount].ID = Read16(RAM[Segment].Data, Offset + 16);
		Object[Viewer.ObjCount].Unk = Read16(RAM[Segment].Data, Offset + 18);

		Object[Viewer.ObjCount].DListOffset = 0x00;
		if(Object[Viewer.ObjCount].ID < 0x190) {
			Object[Viewer.ObjCount].DListOffset = Read32(ROM.Data, (0xC8334 + (Object[Viewer.ObjCount].ID * 0x24)));
			if((Object[Viewer.ObjCount].DListOffset & 0xFF000000) == 0x80000000) Object[Viewer.ObjCount].DListOffset = 0x00;
		}

		if(Object[Viewer.ObjCount].ID == 0xFFFF) break;

		Offset += 0x14;
		Viewer.ObjCount++;
	}

	sv_ExecuteDisplayLists();

	ca_Reset();

	Program.IsViewerEnabled = true;
	oz_ShowWindow(true);

	char Temp[256];
	sprintf(Temp, "Level Viewer (ID %i, file %i)", Viewer.LevelID, Viewer.LevelFile);
	oz_SetWindowTitle(Temp);

	return EXIT_SUCCESS;
}

void sv_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size)
{
	RAM[Segment].Data = (unsigned char*) malloc (sizeof(char) * Size);
	memcpy(RAM[Segment].Data, &Buffer[Offset], Size);
	RAM[Segment].IsSet = true;
	RAM[Segment].Size = Size;
}

void sv_ClearSegment(unsigned char Segment)
{
	if(RAM[Segment].IsSet == true) {
		free(RAM[Segment].Data);
		RAM[Segment].Size = 0;
		RAM[Segment].IsSet = false;
	}
}

void sv_ClearAllSegments()
{
	int i = 0;
	for(i = 0; i < 64; i++) {
		sv_ClearSegment(i);
	}
}

void sv_ClearStructures(bool Full)
{
	int i = 0, j = 0;

	static const struct __Vertex Vertex_Empty;
	for(i = 0; i < ArraySize(Vertex); i++) Vertex[i] = Vertex_Empty;

	static const struct __Texture Texture_Empty;
	Texture[0] = Texture_Empty;
	Texture[1] = Texture_Empty;

	static const struct __TextureCache TextureCache_Empty;
	for(i = 0; i < ArraySize(TextureCache); i++) TextureCache[i] = TextureCache_Empty;
	Program.TextureCachePosition = 0;

	static const struct __RGBA RGBA_Empty;
	Gfx.BlendColor = RGBA_Empty;
	Gfx.EnvColor = RGBA_Empty;
	Gfx.FogColor = RGBA_Empty;
	static const struct __FillColor FillColor_Empty;
	Gfx.FillColor = FillColor_Empty;
	static const struct __PrimColor PrimColor_Empty;
	Gfx.PrimColor = PrimColor_Empty;

	Gfx.DLStackPos = 0;

	Gfx.ChangedModes = 0;
	Gfx.GeometryMode = 0;
	Gfx.OtherModeL = 0;
	Gfx.OtherModeH = 0;
	Gfx.Store_RDPHalf1 = 0; Gfx.Store_RDPHalf2 = 0;
	Gfx.Combiner0 = 0; Gfx.Combiner1 = 0;

	Texture[0].ScaleS = 1.0f;
	Texture[0].ScaleT = 1.0f;
	Texture[1].ScaleS = 1.0f;
	Texture[1].ScaleT = 1.0f;

	Texture[0].ShiftScaleS = 1.0f;
	Texture[0].ShiftScaleT = 1.0f;
	Texture[1].ShiftScaleS = 1.0f;
	Texture[1].ShiftScaleT = 1.0f;

	if(Full) {
		i = 0; j = 0;

		static const struct __FragmentCache FragmentCache_Empty;
		for(i = 0; i < ArraySize(FragmentCache); i++) FragmentCache[i] = FragmentCache_Empty;
		Program.FragCachePosition = 0;

		static const struct __Camera Camera_Empty;
		Camera = Camera_Empty;
	}
}

bool sv_CheckAddressValidity(unsigned int Address)
{
	if(Address == 0) return false;

	unsigned char Segment = (Address & 0xFF000000) >> 24;
	unsigned int Offset = (Address & 0x00FFFFFF);

	if(RAM[Segment].IsSet == false) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- Warning: Segment 0x%02X was not initialized, cannot access offset 0x%06X!\n", Segment, Offset);
		return false;
	} else if(RAM[Segment].Size < Offset) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- Warning: Offset 0x%06X is out of bounds for segment 0x%02X!\n", Offset, Segment);
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, " (Segment size: 0x%06X)\n", RAM[Segment].Size);
		return false;
	}

	return true;
}

void sv_ExecuteDisplayLists()
{
	Gfx.DLStackPos = 0;
	int ObjectNo = 0;

	Gfx.GLListCount = glGenLists(Viewer.ObjCount);
	glListBase(Gfx.GLListCount);

	while(ObjectNo < Viewer.ObjCount) {
		glNewList(Gfx.GLListCount + ObjectNo, GL_COMPILE);
		{
			glPushMatrix();

			u.ul = Object[ObjectNo].LvlPos;

			glTranslatef((float)Object[ObjectNo].X, (float)Object[ObjectNo].Y, ((float)Object[ObjectNo].Z - u.f));
			glRotated(Object[ObjectNo].XRot / 180, 1, 0, 0);
			glRotated(Object[ObjectNo].YRot / 180, 0, 1, 0);
			glRotated(Object[ObjectNo].ZRot / 180, 0, 0, 1);

			if(sv_CheckAddressValidity(Object[ObjectNo].DListOffset)) {
				Gfx.DLStackPos = 0;
				dl_ParseDisplayList(Object[ObjectNo].DListOffset);

			} else {
				glDisable(GL_LIGHTING);
				glDisable(GL_TEXTURE_2D);

				glBegin(GL_QUADS);
					glColor3f(1.0f, 0.0f, 0.0f);

					glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
					glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
					glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
					glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

					glVertex3f( 15.0f, 15.0f,-15.0f);   //V4
					glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
					glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
					glVertex3f(-15.0f, 15.0f,-15.0f);   //V6

					glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
					glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
					glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
					glVertex3f(-15.0f, 15.0f, 15.0f);   //V8

					glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
					glVertex3f(-15.0f, 15.0f, 15.0f);   //V8
					glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
					glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

					glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
					glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
					glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
					glVertex3f( 15.0f,-15.0f, 15.0f);   //V1

					//front
					glColor3f(1.0f, 1.0f, 1.0f);

					glVertex3f(-15.0f, 15.0f, 15.0f);   //V8
					glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
					glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
					glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
				glEnd();

				glEnable(GL_TEXTURE_2D);
				glEnable(GL_LIGHTING);
			}

			glPopMatrix();
		}
		glEndList();

		ObjectNo++;
	}
}

int sv_DeInit()
{
	Program.IsViewerEnabled = false;
	oz_ShowWindow(false);

	return EXIT_SUCCESS;
}
