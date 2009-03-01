/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_tri.c - F3DEX2 vertex, triangle & matrix functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_VTX()
{
	unsigned int VertList_Temp = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TempVertCount = ((VertList_Temp & 0x000FFF) / 2);
	unsigned int TempVertListStartEntry = TempVertCount - ((VertList_Temp & 0xFFF000) >> 12);

	unsigned int TempVertListBank = Readout_CurrentByte5;
	unsigned long TempVertListOffset = Readout_CurrentByte6 << 16;
	TempVertListOffset = TempVertListOffset + (Readout_CurrentByte7 << 8);
	TempVertListOffset = TempVertListOffset + Readout_CurrentByte8;

	F3DEX2_GetVertexList(TempVertListBank, TempVertListOffset, TempVertCount, TempVertListStartEntry);

	glEnable(GL_TEXTURE_2D);
	Renderer_GLTexture = F3DEX2_LoadTexture(0);
	glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);

	return 0;
}

int F3DEX2_GetVertexList(unsigned int Bank, unsigned long Offset, unsigned int VertCount, unsigned int TempVertListStartEntry)
{
	unsigned long CurrentVert = TempVertListStartEntry;
	unsigned long VertListPosition = 0;

	unsigned char * VertListTempBuffer;
	VertListTempBuffer = (unsigned char *) malloc ((VertCount + 1) * 0x10);
	memset(VertListTempBuffer, 0x00, sizeof(VertListTempBuffer));

	Debug_MallocOperations++;

	if(Zelda_MemCopy(Bank, Offset, VertListTempBuffer, ((VertCount + 1) * 0x10)) == -1) {
		sprintf(ErrorMsg, "Invalid Vertex data source 0x%02X!", Bank);
//		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	WavefrontObjVertCount_Previous = WavefrontObjVertCount;

	sprintf(WavefrontObjMsg, "usemtl material\n");
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	while(CurrentVert < VertCount) {
		// X
		Vertex[CurrentVert].X = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// Y
		Vertex[CurrentVert].Y = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// Z
		Vertex[CurrentVert].Z = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=4;

		// H
		Vertex[CurrentVert].H = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// V
		Vertex[CurrentVert].V = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;

		// R, G, B, A
		Vertex[CurrentVert].R = VertListTempBuffer[VertListPosition];
		Vertex[CurrentVert].G = VertListTempBuffer[VertListPosition + 1];
		Vertex[CurrentVert].B = VertListTempBuffer[VertListPosition + 2];
		Vertex[CurrentVert].A = VertListTempBuffer[VertListPosition + 3];
		VertListPosition+=4;

		/* shitty obj extraction time! */

		sprintf(WavefrontObjMsg, "v %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].X / 32, (float)Vertex[CurrentVert].Y / 32, (float)Vertex[CurrentVert].Z / 32);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
		sprintf(WavefrontObjMsg, "vt %4.2f %4.2f\n", (float)Vertex[CurrentVert].H, (float)Vertex[CurrentVert].V);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
		sprintf(WavefrontObjMsg, "vn %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
		fprintf(FileWavefrontObj, WavefrontObjMsg);

		sprintf(WavefrontMtlMsg, "newmtl material\n");
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "Ka %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R / 2, (float)Vertex[CurrentVert].G / 2, (float)Vertex[CurrentVert].B / 2);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "Ka %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "illum 1\n");
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);

		WavefrontObjVertCount++;

/*		sprintf(ErrorMsg, "Vertex: %x, %x, %x, %x, %x, %x, %x, %x, %x",
			Vertex[CurrentVert].X, Vertex[CurrentVert].Y, Vertex[CurrentVert].Z,
			Vertex[CurrentVert].H, Vertex[CurrentVert].V,
			Vertex[CurrentVert].R, Vertex[CurrentVert].G, Vertex[CurrentVert].B, Vertex[CurrentVert].A);
		MessageBox(hwnd, ErrorMsg, "Vertex", MB_OK | MB_ICONINFORMATION);
*/
		CurrentVert++;
	}

	free(VertListTempBuffer);
	Debug_FreeOperations++;

	return 0;
}

int F3DEX2_Cmd_TRI1()
{
	F3DEX2_UpdateGeoMode();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte2 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte3 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte4 / 2);
	glEnd();

	/* MORE shitty obj extraction time! */

	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	return 0;
}

int F3DEX2_Cmd_TRI2()
{
	F3DEX2_UpdateGeoMode();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte2 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte3 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte4 / 2);
	glEnd();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte6 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte7 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte8 / 2);
	glEnd();

	/* MORE AND MORE shitty obj extraction time!! */

	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);
	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte6 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte7 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte8 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	return 0;
}

int F3DEX2_DrawVertexPoint(unsigned int VertexID)
{
	float TempH = (float)Vertex[VertexID].H * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
	float TempV = (float)Vertex[VertexID].V * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
	glTexCoord2f(TempH, TempV);

	glNormal3b(Vertex[VertexID].R, Vertex[VertexID].G, Vertex[VertexID].B);

	if(!(N64_GeometryMode & G_LIGHTING)) {
		glColor4ub(Vertex[VertexID].R, Vertex[VertexID].G, Vertex[VertexID].B, Vertex[VertexID].A);
	} else {
		F3DEX2_HACKSelectClrAlpSource();
	}

	glVertex3d(Vertex[VertexID].X, Vertex[VertexID].Y, Vertex[VertexID].Z);

	return 0;
}

int F3DEX2_Cmd_MTX()
{
	/* hacky implementation of F3DEX2_MTX - doesn't bloddy care about parameters or whatever */
	/* we just get a matrix, multiply it with the existing one and be done with it*/

	/* get shit ready */
	int i = 0, j = 0, RawMatrixCnt = 0;
	signed long TempMatrixData1 = 0, TempMatrixData2 = 0;

	/* get the matrix data offset */
	unsigned int MtxSegment = Readout_CurrentByte5;
	unsigned long MtxOffset = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	/* make some space for the raw data */
	unsigned char * RawMatrixData = (unsigned char *) malloc (sizeof(char) * 0x40);

	/* and get more shit ready */
	GLfloat Matrix[4][4];
	float fRecip = 1.0f / 65536.0f;

	/* no idea what segments 0x0C & 0x0D are, so let's just return when we get those */
	if((MtxSegment == 0x0C) || (MtxSegment == 0x0D)) {
		//????
		return 0;
	}

	/* also, if the dlist tries to read from ram, just pop the ogl matrix and return */
	/* (wonder if those 0x80 references are related to the wobblyness in jabu-jabu?) */
	if(!(MtxSegment == 0x80)) {
		/* load the raw data into the buffer */
		if(Zelda_MemCopy(MtxSegment, MtxOffset, RawMatrixData, 16 * 4) == -1) {
			sprintf(ErrorMsg, "Raw matrix data load FAILED! Offset 0x%02X|%06X", MtxSegment, MtxOffset);
			MessageBox(hwnd, ErrorMsg, "", MB_OK | MB_ICONERROR);
			return 0;
		}
	} else {
		/* here's the pop */
		glPopMatrix();
		return 0;
	}

	sprintf(ErrorMsg, "MATRIX: offset 0x%02X|%06X\n\n", MtxSegment, MtxOffset);

	/* getting ready to convert the raw data into a ogl compatible matrix */
	/* (the original daedalus graphics 0.08 source helped in understanding the raw data layout) */
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			TempMatrixData1 = ((RawMatrixData[RawMatrixCnt		] * 0x100) + RawMatrixData[RawMatrixCnt + 1		]);
			TempMatrixData2 = ((RawMatrixData[RawMatrixCnt + 32	] * 0x100) + RawMatrixData[RawMatrixCnt + 33	]);
			Matrix[i][j] = ((TempMatrixData1 << 16) | TempMatrixData2) * fRecip;

			sprintf(ErrorMsg, "%s[%12.5f]", ErrorMsg, Matrix[i][j]);

			RawMatrixCnt+=2;
		}
		sprintf(ErrorMsg, "%s\n", ErrorMsg);
	}

	Helper_LogMessage(2, ErrorMsg);
	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));

	/* now push the matrix, multiply the existing one with the one we just loaded */
	glPushMatrix();
	glMultMatrixf(*Matrix);

	return 0;
}
