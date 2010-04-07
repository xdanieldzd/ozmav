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
	unsigned long TempVertListOffset = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

/*		memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));
		sprintf(SystemLogMsg, "    offset %02X%06X, count %d (%s), entry %d\n",
			TempVertListBank, TempVertListOffset,
			TempVertCount, (TempVertCount > 32) ? "bounds!" : "okay",
			TempVertListStartEntry);
		Helper_LogMessage(1, SystemLogMsg);
*/
	F3DEX2_GetVertexList(TempVertListBank, TempVertListOffset, TempVertCount, TempVertListStartEntry);

	if((Renderer_EnableFragShader) && (GLExtension_MultiTexture)) {
		glEnable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		Renderer_GLTexture = F3DEX2_LoadTexture(0);
		glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);

		if(IsMultiTexture) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			Renderer_GLTexture = F3DEX2_LoadTexture(1);
			glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	} else {
		glEnable(GL_TEXTURE_2D);
		Renderer_GLTexture = F3DEX2_LoadTexture(0);
		glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
	}

	return 0;
}

int F3DEX2_GetVertexList(unsigned int Bank, unsigned long Offset, unsigned int VertCount, unsigned int TempVertListStartEntry)
{
	unsigned long CurrentVert = TempVertListStartEntry;
	unsigned long VertListPosition = 0;

	if(VertCount > 32) return -1;
	if(CurrentVert > 32) return -1;

	unsigned int VertBufferSize = (VertCount + 1) * 0x10;

	unsigned char * VertListTempBuffer;
	VertListTempBuffer = (unsigned char *) malloc (VertBufferSize);
	memset(VertListTempBuffer, 0x00, sizeof(VertListTempBuffer));

	Debug_MallocOperations++;

	if(Zelda_MemCopy(Bank, Offset, VertListTempBuffer, VertBufferSize) == -1) {
		sprintf(ErrorMsg, "Invalid Vertex data source 0x%02X!", Bank);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	if(Renderer_EnableWavefrontDump) {
		WavefrontObjVertCount_Previous = WavefrontObjVertCount;

		sprintf(WavefrontObjMsg, "usemtl material%d\n", WavefrontObjMaterialCnt);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
	}

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

		if(Renderer_EnableWavefrontDump) {
			/* shitty obj extraction time! */

/*			float TempH = (((float)Vertex[CurrentVert].H * Texture[CurrentTextureID].S_Scale) / 32.0f);
			if(TempH != 0.0f) TempH /= (float)Texture[CurrentTextureID].Width;
			float TempV = (-((float)Vertex[CurrentVert].V * Texture[CurrentTextureID].T_Scale) / 32.0f);
			if(TempV != 0.0f) TempV /= (float)Texture[CurrentTextureID].Height;
*/
			float TempH = _FIXED2FLOAT(Vertex[CurrentVert].H, 16) * (Texture[CurrentTextureID].S_Scale * Texture[CurrentTextureID].S_ShiftScale) / 32.0f / _FIXED2FLOAT(Texture[CurrentTextureID].Width, 16);
			float TempV = -_FIXED2FLOAT(Vertex[CurrentVert].V, 16) * (Texture[CurrentTextureID].T_Scale * Texture[CurrentTextureID].T_ShiftScale) / 32.0f / _FIXED2FLOAT(Texture[CurrentTextureID].Height, 16);

			sprintf(WavefrontObjMsg, "v %4.8f %4.8f %4.8f\n", (float)Vertex[CurrentVert].X / 32, (float)Vertex[CurrentVert].Y / 32, (float)Vertex[CurrentVert].Z / 32);
			fprintf(FileWavefrontObj, WavefrontObjMsg);
			sprintf(WavefrontObjMsg, "vt %4.8f %4.8f\n", TempH, -TempV);
			fprintf(FileWavefrontObj, WavefrontObjMsg);
			sprintf(WavefrontObjMsg, "vn %4.8f %4.8f %4.8f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
			fprintf(FileWavefrontObj, WavefrontObjMsg);
			sprintf(WavefrontObjMsg, "vc %1.0f %1.0f %1.0f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
			fprintf(FileWavefrontObj, WavefrontObjMsg);

			WavefrontObjVertCount++;
		}

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

	if(Renderer_EnableWavefrontDump) {
		/* MORE shitty obj extraction time! */

		unsigned int Vert1 = (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert2 = (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert3 = (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous;
		sprintf(WavefrontObjMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			Vert1, Vert1, Vert1,
			Vert2, Vert2, Vert2,
			Vert3, Vert3, Vert3);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
	}

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

	F3DEX2_UpdateGeoMode();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte6 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte7 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte8 / 2);
	glEnd();

	if(Renderer_EnableWavefrontDump) {
		/* MORE AND MORE shitty obj extraction time!! */

		unsigned int Vert1 = (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert2 = (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert3 = (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous;
		sprintf(WavefrontObjMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			Vert1, Vert1, Vert1,
			Vert2, Vert2, Vert2,
			Vert3, Vert3, Vert3);
		fprintf(FileWavefrontObj, WavefrontObjMsg);

		unsigned int Vert4 = (Readout_CurrentByte6 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert5 = (Readout_CurrentByte7 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert6 = (Readout_CurrentByte8 / 2) + 1 + WavefrontObjVertCount_Previous;
		sprintf(WavefrontObjMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			Vert4, Vert4, Vert4,
			Vert5, Vert5, Vert5,
			Vert6, Vert6, Vert6);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
	}

	return 0;
}

int F3DEX2_Cmd_QUAD()
{
	/* is this correct? dunno, zelda doesn't use quads */

	F3DEX2_UpdateGeoMode();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte2 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte3 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte4 / 2);
	glEnd();

	F3DEX2_UpdateGeoMode();

	glBegin(GL_TRIANGLES);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte6 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte7 / 2);
		F3DEX2_DrawVertexPoint(Readout_CurrentByte8 / 2);
	glEnd();

	if(Renderer_EnableWavefrontDump) {
		unsigned int Vert1 = (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert2 = (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert3 = (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous;
		sprintf(WavefrontObjMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			Vert1, Vert1, Vert1,
			Vert2, Vert2, Vert2,
			Vert3, Vert3, Vert3);
		fprintf(FileWavefrontObj, WavefrontObjMsg);

		unsigned int Vert4 = (Readout_CurrentByte6 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert5 = (Readout_CurrentByte7 / 2) + 1 + WavefrontObjVertCount_Previous;
		unsigned int Vert6 = (Readout_CurrentByte8 / 2) + 1 + WavefrontObjVertCount_Previous;
		sprintf(WavefrontObjMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			Vert4, Vert4, Vert4,
			Vert5, Vert5, Vert5,
			Vert6, Vert6, Vert6);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
	}

	return 0;
}

int F3DEX2_DrawVertexPoint(unsigned int VertexID)
{
	float TempH0 = (float)Vertex[VertexID].H * (Texture[0].S_Scale * Texture[0].S_ShiftScale) / 32 / Texture[0].Width;
	float TempV0 = (float)Vertex[VertexID].V * (Texture[0].T_Scale * Texture[0].T_ShiftScale) / 32 / Texture[0].Height;
	float TempH1 = (float)Vertex[VertexID].H * (Texture[1].S_Scale * Texture[1].S_ShiftScale) / 32 / Texture[1].Width;
	float TempV1 = (float)Vertex[VertexID].V * (Texture[1].T_Scale * Texture[1].T_ShiftScale) / 32 / Texture[1].Height;

	if((Renderer_EnableFragShader) && (GLExtension_MultiTexture)) {
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempH0, TempV0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempH1, TempV1);
	} else {
		glTexCoord2f(TempH0, TempV0);
	}

	glNormal3b(Vertex[VertexID].R, Vertex[VertexID].G, Vertex[VertexID].B);
	if(!(N64_GeometryMode & G_LIGHTING)) glColor4ub(Vertex[VertexID].R, Vertex[VertexID].G, Vertex[VertexID].B, Vertex[VertexID].A);

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

	/* no idea what segments 0x01, 0x0C & 0x0D are, so let's just return when we get those */
	if((MtxSegment == 0x01) ||(MtxSegment == 0x0C) || (MtxSegment == 0x0D)) {
		//????
		return 0;
	}

	/* also, if the dlist tries to read from ram, just pop the ogl matrix and return */
	/* (wonder if those 0x80 references are related to the wobblyness in jabu-jabu?) */
	if(!(MtxSegment == 0x80)) {
		/* load the raw data into the buffer */
		if(Zelda_MemCopy(MtxSegment, MtxOffset, RawMatrixData, 16 * 4) == -1) {
			sprintf(ErrorMsg, "Raw matrix data load FAILED! Offset 0x%02X|%06X", MtxSegment, (unsigned int)MtxOffset);
			MessageBox(hwnd, ErrorMsg, "", MB_OK | MB_ICONERROR);
			return 0;
		}
	} else {
		/* here's the pop */
		glPopMatrix();
		return 0;
	}

	sprintf(ErrorMsg, "MATRIX: offset 0x%02X|%06X\n\n", MtxSegment, (unsigned int)MtxOffset);

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

//	Helper_LogMessage(2, ErrorMsg);
	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));

	/* now push the matrix, multiply the existing one with the one we just loaded */
	glPushMatrix();
	glMultMatrixf(*Matrix);

	return 0;
}

int F3DEX2_Cmd_POPMTX()
{
	glPopMatrix(); /* ...i guess. works for majora 0x3b, etc */

	return 0;
}
