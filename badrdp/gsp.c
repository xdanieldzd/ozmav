#include "globals.h"

void gSP_Vertex(unsigned int Vtx, int N, int V0)
{
	if(!RDP_CheckAddressValidity(Vtx)) return;

	if((N > 32) || (V0 > 32)) return;

	if(Gfx.Update & CHANGED_MULT_MAT) {
		Gfx.Update ^= CHANGED_MULT_MAT;
		RDP_Matrix_MulMatrices(Matrix.Model, Matrix.Proj, Matrix.Comb);
	}

	unsigned char TempSegment = Vtx >> 24;
	unsigned int TempOffset = (Vtx & 0x00FFFFFF);

	int i = 0;
	for(i = 0; i < N; i++) {
		Vertex[V0 + i].Vtx.S = Read16(RAM[TempSegment].Data, TempOffset + (i * 16) + 8);
		Vertex[V0 + i].Vtx.T = Read16(RAM[TempSegment].Data, TempOffset + (i * 16) + 10);
		Vertex[V0 + i].Vtx.R = RAM[TempSegment].Data[TempOffset + (i * 16) + 12];
		Vertex[V0 + i].Vtx.G = RAM[TempSegment].Data[TempOffset + (i * 16) + 13];
		Vertex[V0 + i].Vtx.B = RAM[TempSegment].Data[TempOffset + (i * 16) + 14];
		Vertex[V0 + i].Vtx.A = RAM[TempSegment].Data[TempOffset + (i * 16) + 15];

		short X = Read16(RAM[TempSegment].Data, TempOffset + (i * 16));
		short Y = Read16(RAM[TempSegment].Data, TempOffset + (i * 16) + 2);
		short Z = Read16(RAM[TempSegment].Data, TempOffset + (i * 16) + 4);
		short W = Read16(RAM[TempSegment].Data, TempOffset + (i * 16) + 6);

		Vertex[V0 + i].Vtx.X = X;//X * (short)Matrix.Comb[0][0] + Y * (short)Matrix.Comb[1][0] + Z * (short)Matrix.Comb[2][0] + (short)Matrix.Comb[3][0];
		Vertex[V0 + i].Vtx.Y = Y;//X * (short)Matrix.Comb[0][1] + Y * (short)Matrix.Comb[1][1] + Z * (short)Matrix.Comb[2][1] + (short)Matrix.Comb[3][1];
		Vertex[V0 + i].Vtx.Z = Z;//X * (short)Matrix.Comb[0][2] + Y * (short)Matrix.Comb[1][2] + Z * (short)Matrix.Comb[2][2] + (short)Matrix.Comb[3][2];
		Vertex[V0 + i].Vtx.W = W;

		if(Gfx.GeometryMode & G_ZBUFFER) {
			Vertex[V0 + i].Vtx.Z = -Vertex[V0 + i].Vtx.W;
		}
	}
}

void gSP_VertexMtxHack(unsigned int Vtx, int N, int V0, unsigned int Mtx)
{
//	dbgprintf(0, 0, "%s(%08X, %i, %i, %08X)", __FUNCTION__, Vtx, N, V0, Mtx);

	if(!RDP_CheckAddressValidity(Vtx)) return;
	if(!RDP_CheckAddressValidity(Mtx)) return;

	if((N > 32) || (V0 > 32)) return;

	unsigned char VSeg = Vtx >> 24, MSeg = Mtx >> 24;
	unsigned int VOffset = (Vtx & 0x00FFFFFF), MOffset = (Mtx & 0x00FFFFFF);

	int x, y; float TempMatrix[4][4];
	for(x = 0; x < 4; x++) {
		for(y = 0; y < 4; y++) {
			int MtxTemp1 = Read16(RAM[MSeg].Data, MOffset);
			int MtxTemp2 = Read16(RAM[MSeg].Data, MOffset + 32);
			TempMatrix[x][y] = ((MtxTemp1 << 16) | MtxTemp2) * (1.0f / 65536.0f);
			MOffset += 2;
		}
	}

	short TX = -(short)TempMatrix[3][0];
	short TY = -(short)TempMatrix[3][1];
	short TZ = -(short)TempMatrix[3][2];

	int i = 0;
	for(i = 0; i < N; i++) {
		Vertex[V0 + i].Vtx.S = Read16(RAM[VSeg].Data, VOffset + (i * 16) + 8);
		Vertex[V0 + i].Vtx.T = Read16(RAM[VSeg].Data, VOffset + (i * 16) + 10);
		Vertex[V0 + i].Vtx.R = RAM[VSeg].Data[VOffset + (i * 16) + 12];
		Vertex[V0 + i].Vtx.G = RAM[VSeg].Data[VOffset + (i * 16) + 13];
		Vertex[V0 + i].Vtx.B = RAM[VSeg].Data[VOffset + (i * 16) + 14];
		Vertex[V0 + i].Vtx.A = RAM[VSeg].Data[VOffset + (i * 16) + 15];

		short X = Read16(RAM[VSeg].Data, VOffset + (i * 16));
		short Y = Read16(RAM[VSeg].Data, VOffset + (i * 16) + 2);
		short Z = Read16(RAM[VSeg].Data, VOffset + (i * 16) + 4);

		Vertex[V0 + i].Vtx.X = X + TX;
		Vertex[V0 + i].Vtx.Y = Y + TY;
		Vertex[V0 + i].Vtx.Z = Z + TZ;

/*		float X = _FIXED2FLOAT((Read16(RAM[VSeg].Data, VOffset + (i * 16))), 14) / 32.0f;
		float Y = _FIXED2FLOAT((Read16(RAM[VSeg].Data, VOffset + (i * 16) + 2)), 14) / 32.0f;
		float Z = _FIXED2FLOAT((Read16(RAM[VSeg].Data, VOffset + (i * 16) + 4)), 14) / 32.0f;

		Vertex[V0 + i].Vtx.X = (int)(X * TempMatrix[0][0] + Y * TempMatrix[1][0] + Z * TempMatrix[2][0] + TempMatrix[3][0]);
		Vertex[V0 + i].Vtx.Y = (int)(X * TempMatrix[0][1] + Y * TempMatrix[1][1] + Z * TempMatrix[2][1] + TempMatrix[3][1]);
		Vertex[V0 + i].Vtx.Z = (int)(X * TempMatrix[0][2] + Y * TempMatrix[1][2] + Z * TempMatrix[2][2] + TempMatrix[3][2]);*/
	}
}

void gSP_ModifyVertex(unsigned int Vtx, unsigned int Type, unsigned int Val)
{
	switch(Type) {
		case G_MWO_POINT_RGBA:
			Vertex[Vtx].Vtx.R = _SHIFTR(Val, 24, 8);
			Vertex[Vtx].Vtx.G = _SHIFTR(Val, 16, 8);
			Vertex[Vtx].Vtx.B = _SHIFTR(Val, 8, 8);
			Vertex[Vtx].Vtx.A = _SHIFTR(Val, 0, 8);
			break;
		case G_MWO_POINT_ST:
			Vertex[Vtx].Vtx.S = _SHIFTR(Val, 16, 16);
			Vertex[Vtx].Vtx.T = _SHIFTR(Val, 0, 16);
			break;
	}
}

void gSP_Matrix(unsigned int Mtx, unsigned char Param)
{
	if(!RDP_CheckAddressValidity(Mtx)) return;

	unsigned char Segment = Mtx >> 24;
	unsigned int Offset = (Mtx & 0x00FFFFFF);

	int MtxTemp1 = 0, MtxTemp2 = 0;
	int x = 0, y = 0;

	float TempMatrix[4][4];

	for(x = 0; x < 4; x++) {
		for(y = 0; y < 4; y++) {
			MtxTemp1 = Read16(RAM[Segment].Data, Offset);
			MtxTemp2 = Read16(RAM[Segment].Data, Offset + 32);
			TempMatrix[x][y] = ((MtxTemp1 << 16) | MtxTemp2) * (1.0f / 65536.0f);
			Offset += 2;
		}
	}

	if(Param & G_MTX_PROJECTION) {
		if(Param & G_MTX_LOAD) {
			RDP_Matrix_ProjectionLoad(TempMatrix);
		} else {
			RDP_Matrix_ProjectionMul(TempMatrix);
		}
	} else {
		if((Param & G_MTX_PUSH) && (Matrix.ModelIndex < (Matrix.ModelStackSize - 1))) {
			RDP_Matrix_ModelviewPush();
		}

		if(Param & G_MTX_LOAD) {
			RDP_Matrix_ModelviewLoad(TempMatrix);
		} else {
			RDP_Matrix_ModelviewMul(TempMatrix);
		}
	}
}

void gSP_GeometryMode(unsigned int Clear, unsigned int Set)
{
	Gfx.GeometryMode = (Gfx.GeometryMode & ~Clear) | Set;

	Gfx.Update |= CHANGED_GEOMETRYMODE;
/*
	MSK_ConsolePrint( 0, "gSPGeometryMode( %s%s%s%s%s%s%s%s%s%s, %s%s%s%s%s%s%s%s%s%s );",
		Clear & G_SHADE ? "G_SHADE | " : "",
		Clear & G_LIGHTING ? "G_LIGHTING | " : "",
		Clear & G_SHADING_SMOOTH ? "G_SHADING_SMOOTH | " : "",
		Clear & G_ZBUFFER ? "G_ZBUFFER | " : "",
		Clear & G_TEXTURE_GEN ? "G_TEXTURE_GEN | " : "",
		Clear & G_TEXTURE_GEN_LINEAR ? "G_TEXTURE_GEN_LINEAR | " : "",
		Clear & G_CULL_FRONT ? "G_CULL_FRONT | " : "",
		Clear & G_CULL_BACK ? "G_CULL_BACK | " : "",
		Clear & G_FOG ? "G_FOG | " : "",
		Clear & G_CLIPPING ? "G_CLIPPING" : "",
		Set & G_SHADE ? "G_SHADE | " : "",
		Set & G_LIGHTING ? "G_LIGHTING | " : "",
		Set & G_SHADING_SMOOTH ? "G_SHADING_SMOOTH | " : "",
		Set & G_ZBUFFER ? "G_ZBUFFER | " : "",
		Set & G_TEXTURE_GEN ? "G_TEXTURE_GEN | " : "",
		Set & G_TEXTURE_GEN_LINEAR ? "G_TEXTURE_GEN_LINEAR | " : "",
		Set & G_CULL_FRONT ? "G_CULL_FRONT | " : "",
		Set & G_CULL_BACK ? "G_CULL_BACK | " : "",
		Set & G_FOG ? "G_FOG | " : "",
		Set & G_CLIPPING ? "G_CLIPPING" : "" );*/
}
