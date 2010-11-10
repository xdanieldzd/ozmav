#include "globals.h"

void swapRAMSegments(unsigned char Seg1, unsigned char Seg2)
{
	unsigned int SegTemp_Size = RAM[Seg1].Size;
	unsigned char * SegTemp_Data = RAM[Seg1].Data;
	bool SegTemp_IsSet = RAM[Seg1].IsSet;

	RAM[Seg1].Size = RAM[Seg2].Size;
	RAM[Seg1].Data = RAM[Seg2].Data;
	RAM[Seg1].IsSet = RAM[Seg2].IsSet;

	RAM[Seg2].Size = SegTemp_Size;
	RAM[Seg2].Data = SegTemp_Data;
	RAM[Seg2].IsSet = SegTemp_IsSet;

//	dbgprintf(0,0,"SWAP! Seg 0x%02X size: 0x%04X bytes, Seg 0x%02X size: 0x%04X bytes", Seg1, RAM[Seg1].Size, Seg2, RAM[Seg2].Size);
}

void initActorParsing(int objFileNo)
{
	// if func param is > 0, do not use an actor file
	if(objFileNo >= 0) {
		vCurrentActor.useActorOvl = false;

	// if func param is -1, use an actor overlay file
	} else if(objFileNo == -1) {
		vCurrentActor.useActorOvl = true;

	// if func param is -2, use external anim file
	} else if(objFileNo == -2) {
		vCurrentActor.useExtAnim = true;
	}

//	dbgprintf(0,0,"useActorOvl:%i, useExtAnim:%i", vCurrentActor.useActorOvl, vCurrentActor.useExtAnim);

	if(objFileNo != -2) {
		RDP_ClearStructures(true);
		RDP_ClearTextures();

		zl_ClearAllSegments();

		vCurrentActor.variable = 0;

		vCurrentActor.actorScale = 0;
		vCurrentActor.offsetDList = 0;

		memset(vCurrentActor.offsetBoneSetup, 0, sizeof(vCurrentActor.offsetBoneSetup));
		vCurrentActor.boneSetupTotal = 0;
		vCurrentActor.boneSetupCurrent = 0;

		memset(vCurrentActor.offsetAnims, 0, sizeof(vCurrentActor.offsetAnims));
		vCurrentActor.animTotal = 0;
		vCurrentActor.animCurrent = 0;

		vCurrentActor.frameTotal = 0;
		vCurrentActor.frameCurrent = 0;

		memset(vCurrentActor.oName, 0, sizeof(vCurrentActor.oName));

		vCurrentActor.useExtAnim = false;

		vProgram.animPlay = false;
		vProgram.targetFPS = 30.0f;
	}

	setMipsWatchers();

	if(vCurrentActor.useActorOvl) {
		// use actor overlay file
		if(vActors[vCurrentActor.actorNumber].ObjectNumber > 0) {
			RDP_LoadToSegment(	vActors[vCurrentActor.actorNumber].ObjectSegment,
								vObjects[vActors[vCurrentActor.actorNumber].ObjectNumber].ObjectData,
								0,
								vObjects[vActors[vCurrentActor.actorNumber].ObjectNumber].ObjectSize);

			processActor();
		}

	} else {
		if(objFileNo != -2) {
			// parse object directly
			DMA FileInfo = zl_DMAGetFile(objFileNo);
			FileInfo = zl_DMAVirtualToPhysical(FileInfo.VStart, FileInfo.VEnd);

			if((FileInfo.PStart != 0) && (FileInfo.PEnd != 0)) {
				strcpy(vCurrentActor.oName, FileInfo.Filename);

				RDP_ClearSegment(0x06);

				RAM[0x06].Size = FileInfo.PEnd - FileInfo.PStart;
				RAM[0x06].Data = zl_DMAToBuffer(FileInfo);
				RAM[0x06].IsSet = true;

				processActor();
			}
		} else {
			processActor();
		}
	}

	mips_ResetWatch();
	mips_ResetResults();
}

void setMipsWatchers()
{
	mips_SetFuncWatch(0x80035260);// u32 a1 = display list
	mips_SetFuncWatch(0x800A457C);// u32 a2 = bones; u32 a3 = animation
	mips_SetFuncWatch(0x8002D62C);// f32 a1 = scale
	mips_SetFuncWatch(0x80093D18);// u32 a3 = display list
	mips_SetFuncWatch(0x800A51A0);// u32 a1 = animation
	mips_SetFuncWatch(0x800A46F8);// u32 a2 = bones
	mips_SetFuncWatch(0x800A2000);// a0 = animation
	mips_SetFuncWatch(0x800D0984);// u32 a0 = display list

	//mips_SetFuncWatch(0x800A663C);// a2 = bones a3 = ani Note: different bone structure, possibly different animation type!

	//mips_SetFuncWatch(0x80077A00); // a2 = unknown float, probably not scale
	mips_SetFuncWatch(0x800A4FE4); // a1 = animation ???
	//mips_SetFuncWatch(0x80B4FD00); // a1 = animation from external file ???
	mips_SetFuncWatch(0x80035324); // a1 = dlist

	//mips_SetFuncWatch(0x80041880); // a0 = collision-related address, a1 = offset to address (negative)  ???
	//mips_SetFuncWatch(0x808A3068); // a0 = collision-related address, a1 = offset to address (negative)  ???

	return;
}

struct actorSections getActorSections(unsigned char * Data, size_t Size, unsigned long VStart)
{
	struct actorSections ret;
	int indent, sections_addr;

	indent = Read32(Data, Size-4);
	sections_addr = Size - indent;

	ret.text	= Data;
	ret.text_va	= VStart;
	ret.text_s	= Read32(Data, sections_addr+0x0);

	ret.data	= ret.text + ret.text_s;
	ret.data_va	= ret.text_va + ret.text_s;
	ret.data_s	= Read32(Data, sections_addr+0x4);

	ret.rodata	= ret.data + ret.data_s;
	ret.rodata_va	= ret.data_va + ret.data_s;
	ret.rodata_s	= Read32(Data, sections_addr+0x8);

	ret.bss		= ret.rodata + ret.rodata_s;
	ret.bss_va	= ret.rodata_va + ret.rodata_s;
	ret.bss_s	= Read32(Data, sections_addr+0xC);

	return ret;
}

void processActor()
{
	if(vCurrentActor.useActorOvl) {
		// use actor overlay file
		float * scale = NULL;
		int *anim = NULL, *dlist = NULL, *bones = NULL;
		struct actorSections Sections;

		mips_ResetSpecialOps();
		mips_SetSpecialOp(MIPS_LH(mips_r0, 0x1C, mips_a0), vCurrentActor.variable);
		mips_SetSpecialOp(MIPS_LH(mips_r0, 0x1C, mips_s0), vCurrentActor.variable);

		Sections = getActorSections(vActors[vCurrentActor.actorNumber].ActorData, vActors[vCurrentActor.actorNumber].ActorSize, 0);

		mips_ResetMap();

		if(Sections.data_s) mips_SetMap(Sections.data, Sections.data_s, Sections.data_va);
		if(Sections.rodata_s) mips_SetMap(Sections.rodata, Sections.rodata_s, Sections.rodata_va);
		if(Sections.bss_s) mips_SetMap(Sections.bss, Sections.bss_s, Sections.bss_va);

		mips_EvalWords((unsigned int *)Sections.text, Sections.text_s / 4);

		scale = mips_GetFuncArg(0x8002D62C, 1, true);
		if(scale == NULL) {
			scale = mips_GetFuncArg(0x80077A00, 2, true);
		}
		dlist = mips_GetFuncArg(0x80035260, 1, true);
		if(dlist == NULL || *dlist >= 0x80000000){
			dlist = mips_GetFuncArg(0x80093D18, 3, true);
			if(dlist == NULL || *dlist >= 0x80000000){
				dlist = mips_GetFuncArg(0x800D0984, 0, true);
				if(dlist == NULL || *dlist >= 0x80000000) {
					dlist = mips_GetFuncArg(0x80035324, 1, true);
				}
			}
		}

		vCurrentActor.actorScale = (scale != NULL) ? *scale : 0.01f;
		vCurrentActor.offsetDList = (dlist != NULL) ? *dlist : 0;

		scanAnimations(vActors[vCurrentActor.actorNumber].ObjectSegment);

		if(vCurrentActor.offsetAnims[0] == 0) {
			anim = mips_GetFuncArg(0x800A457C, 3, true);
			if(anim == NULL || !*anim){
				anim = mips_GetFuncArg(0x800A51A0, 1, true);
				if(anim == NULL || !*anim){
					anim = mips_GetFuncArg(0x80B4FD00, 1, true);
					if(anim == NULL || !*anim) {
						anim = mips_GetFuncArg(0x800A4FE4, 1, true);
						if(anim == NULL || !*anim) {
							anim = mips_GetFuncArg(0x800A2000, 0, true);
						}
					}
				}
			}
			vCurrentActor.offsetAnims[0] = (anim != NULL) ? *anim : 0;
			if(vCurrentActor.offsetAnims[0]) vCurrentActor.animTotal = 0;
		}

		bones = mips_GetFuncArg(0x800A457C, 2, true);
		if(bones == NULL || !*bones){
			bones = mips_GetFuncArg(0x800A46F8, 2, true);
		}
		vCurrentActor.offsetBoneSetup[0] = (bones != NULL) ? *bones : 0;
		if(vCurrentActor.offsetBoneSetup[0]) vCurrentActor.boneSetupTotal = 0;

		if(vCurrentActor.offsetBoneSetup[0] == 0) {
			scanBones(vActors[vCurrentActor.actorNumber].ObjectSegment);
		}

		if(!(vCurrentActor.offsetDList >> 24) && vCurrentActor.offsetDList){
			vCurrentActor.offsetDList = 0;
		}

		if(vCurrentActor.actorScale < 0.0f) vCurrentActor.actorScale = 0.01f;

		dbgprintf(0, MSK_COLORTYPE_INFO, "actor 0x%04X (%s):\n", vCurrentActor.actorNumber, vActors[vCurrentActor.actorNumber].ActorName);
		dbgprintf(0, MSK_COLORTYPE_INFO, "known -> object 0x%04X (%s) seg:%i\n",
			vActors[vCurrentActor.actorNumber].ObjectNumber, vObjects[vActors[vCurrentActor.actorNumber].ObjectNumber].ObjectName, vActors[vCurrentActor.actorNumber].ObjectSegment);
		dbgprintf(0, MSK_COLORTYPE_INFO, "scanned -> bones[0]:%08x, dlist:%08x, anim[0]:%08x, scale:%.2f\n",
			vCurrentActor.offsetBoneSetup[0], vCurrentActor.offsetDList, vCurrentActor.offsetAnims[0], vCurrentActor.actorScale);

	} else {
		// parse object directly
		dbgprintf(0, MSK_COLORTYPE_INFO, "direct object load, scanning now...\n");

		if(vCurrentActor.useExtAnim) {
			swapRAMSegments(0x06, 0x01);
			scanAnimations(0x06);
			swapRAMSegments(0x06, 0x01);
		} else {
			scanAnimations(0x06);
		}

		scanBones(0x06);
		vCurrentActor.actorScale = 0.01f;

		dbgprintf(0, MSK_COLORTYPE_INFO, "scanned -> bones[0]:%08x, dlist:%08x, anim[0]:%08x, scale:%.2f\n",
			vCurrentActor.offsetBoneSetup[0], vCurrentActor.offsetDList, vCurrentActor.offsetAnims[0], vCurrentActor.actorScale);
	}
}

void drawBone(actorBone Bones[], int CurrentBone, int ParentBone)
{
	if(vProgram.showBones) {
		// borrowed from UoT
		if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
		glDisable(GL_TEXTURE);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);

		glDepthRange(0, 0);
		glLineWidth(4);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3d(0, 0, 0);
		glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
		glEnd();
		glPointSize(6);
		glBegin(GL_POINTS);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
		glEnd();
		glPointSize(3);
		glBegin(GL_POINTS);
		glColor3f(vBoneColorFactor.R, vBoneColorFactor.G, vBoneColorFactor.B);
		glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
		glEnd();
		glPointSize(1);
		glLineWidth(1);
		glDepthRange(0, 1);

		glEnable(GL_TEXTURE);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
	}

	glPushMatrix();

	glTranslated(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
	glRotated(Bones[CurrentBone].RZ / 182.0444444, 0, 0, 1);
	glRotated(Bones[CurrentBone].RY / 182.0444444, 0, 1, 0);
	glRotated(Bones[CurrentBone].RX / 182.0444444, 1, 0, 0);

	//Draw display list
	if(Bones[CurrentBone].DList && RDP_CheckAddressValidity(Bones[CurrentBone].DList)){
		RDP_ClearStructures(false);
		RDP_ParseDisplayList(Bones[CurrentBone].DList, true);
	}

	//Draw child
	if((Bones[CurrentBone].Child > -1) && (Bones[CurrentBone].Child != CurrentBone)) {
		vBoneColorFactor.R = 1.0f;
		vBoneColorFactor.G = 0.0f;
		vBoneColorFactor.B = 0.0f;
		drawBone(Bones, Bones[CurrentBone].Child, CurrentBone);
	} else {
		vBoneColorFactor.R = 1.0f;
		vBoneColorFactor.G = 1.0f;
		vBoneColorFactor.B = 1.0f;
	}

	glPopMatrix();

	//Draw sibling/next bone
	if((Bones[CurrentBone].Sibling > -1) && (Bones[CurrentBone].Sibling != CurrentBone)) {
		vBoneColorFactor.R = 0.0f;
		vBoneColorFactor.G = 0.0f;
		vBoneColorFactor.B = 1.0f;
		drawBone(Bones, Bones[CurrentBone].Sibling, ParentBone);
	} else {
		vBoneColorFactor.R = 1.0f;
		vBoneColorFactor.G = 1.0f;
		vBoneColorFactor.B = 1.0f;
	}
}
/*
#define S1(row, col)	Src1[(col << 2) + row]
#define S2(row, col)	Src1[(col << 2) + row]
#define T(row, col)		Target[(col << 2) + row]

void zl_MulMatrices(int Src1[16], int Src2[16], int Target[16])
{
	int i;
	for(i = 0; i < 4; i++) {
		const int si0 = S1(i, 0), si1 = S1(i, 1), si2 = S1(i, 2), si3 = S1(i, 3);
		T(i, 0) = si0 * S2(0, 0) + si1 * S2(1, 0) + si2 * S2(2, 0) + si3 * S2(3,0);
		T(i, 1) = si0 * S2(0, 1) + si1 * S2(1, 1) + si2 * S2(2, 1) + si3 * S2(3,1);
		T(i, 2) = si0 * S2(0, 2) + si1 * S2(1, 2) + si2 * S2(2, 2) + si3 * S2(3,2);
		T(i, 3) = si0 * S2(0, 3) + si1 * S2(1, 3) + si2 * S2(2, 3) + si3 * S2(3,3);
	}
}
*/
void drawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ)
{
	int BoneCount, BoneListListOffset, Seg, _Seg, i, AniSeg=0, RotIndexOffset=0, RotValOffset=0;

	int Limit = 0;

	RDP_ClearStructures(false);

	if(!RDP_CheckAddressValidity(BoneOffset)){
		return;
	}
	if(Scale < 0.001f){
		Scale = 0.02f;
	}
	Seg = (BoneOffset >> 24) & 0xFF;
	BoneOffset &= 0xFFFFFF;

	//parse bones
	BoneCount = RAM[Seg].Data[(BoneOffset) + 4];
	BoneListListOffset = Read32(RAM[Seg].Data, BoneOffset);
	if(!RDP_CheckAddressValidity(BoneListListOffset)){
		return;
	}

	actorBone Bones[BoneCount];
	memset(Bones, 0, sizeof(actorBone) * BoneCount);

	// SWAP segments
	if(vCurrentActor.useExtAnim) swapRAMSegments(0x06, 0x01);

	if(RDP_CheckAddressValidity(AnimationOffset)){
		AniSeg = AnimationOffset >> 24;
		AnimationOffset &= 0xFFFFFF;
		RotIndexOffset = Read32((RAM[AniSeg].Data), (AnimationOffset+8));
		RotIndexOffset &= 0xFFFFFF;
		RotValOffset = Read32((RAM[AniSeg].Data), (AnimationOffset+4));
		RotValOffset &= 0xFFFFFF;
		vCurrentActor.frameTotal = (Read16((RAM[AniSeg].Data), (AnimationOffset))) - 1;
		Limit = Read16((RAM[AniSeg].Data), (AnimationOffset+12));
		/*Bones[0].X = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset) * 2) );
		Bones[0].Y = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset+2) * 2) );
		Bones[0].Z = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset+4) * 2) );*/

		RotIndexOffset += 6;
	}

	// SWAP segments
	if(vCurrentActor.useExtAnim) swapRAMSegments(0x06, 0x01);

	Seg = (BoneListListOffset >> 24) & 0xFF;
	BoneListListOffset &= 0xFFFFFF;

	for(i=0; i<BoneCount; i++)
	{
		BoneOffset = Read32(RAM[Seg].Data, BoneListListOffset + (i << 2));
		if(!RDP_CheckAddressValidity(BoneOffset)){
			return;
		}
		_Seg = (BoneOffset >> 24) & 0xFF;
		BoneOffset &= 0xFFFFFF;
		Bones[i].X += Read16(RAM[_Seg].Data, BoneOffset);
		Bones[i].Y += Read16(RAM[_Seg].Data, BoneOffset + 2);
		Bones[i].Z += Read16(RAM[_Seg].Data, BoneOffset + 4);
		Bones[i].Child = RAM[_Seg].Data[BoneOffset+6];
		Bones[i].Sibling = RAM[_Seg].Data[BoneOffset+7];
		Bones[i].DList = Read32(RAM[_Seg].Data, BoneOffset+8);
		Bones[i].isSet = 1;

		// SWAP segments
		if(vCurrentActor.useExtAnim) swapRAMSegments(0x06, 0x01);

		if(AniSeg && RDP_CheckAddressValidity((AniSeg<<24)|(RotIndexOffset + (i * 6) + 4) ) ){
			unsigned short RXIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6));
			unsigned short RYIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6) + 2);
			unsigned short RZIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6) + 4);

			if(RXIndex >= Limit) RXIndex += vCurrentActor.frameCurrent;
			if(RYIndex >= Limit) RYIndex += vCurrentActor.frameCurrent;
			if(RZIndex >= Limit) RZIndex += vCurrentActor.frameCurrent;

			if(	!RDP_CheckAddressValidity((AniSeg<<24)|(RotValOffset + (RXIndex * 2))) ||
				!RDP_CheckAddressValidity((AniSeg<<24)|(RotValOffset + (RYIndex * 2))) ||
				!RDP_CheckAddressValidity((AniSeg<<24)|(RotValOffset + (RZIndex * 2)))
				) return;

			Bones[i].RX = Read16(RAM[AniSeg].Data, RotValOffset + (RXIndex * 2));
			Bones[i].RY = Read16(RAM[AniSeg].Data, RotValOffset + (RYIndex * 2));
			Bones[i].RZ = Read16(RAM[AniSeg].Data, RotValOffset + (RZIndex * 2));

//			dbgprintf(0, MSK_COLORTYPE_INFO, " Bone %2i (%08X): (%6i %6i %6i) (%2i %2i) %08X", i, BoneOffset, Bones[i].X, Bones[i].Y, Bones[i].Z, Bones[i].Child, Bones[i].Sibling, Bones[i].DList);
		}

		// SWAP segments
		if(vCurrentActor.useExtAnim) swapRAMSegments(0x06, 0x01);
	}

	vBoneColorFactor.R = 0.0f;
	vBoneColorFactor.G = 0.0f;
	vBoneColorFactor.B = 0.0f;
/*
	RAM[0x0D].Size = (BoneCount+1)*0x40;
	RAM[0x0D].Data = (unsigned char*)malloc(sizeof(char)*RAM[0x0D].Size);
	RAM[0x0D].IsSet = true;
	memset(RAM[0x0D].Data, 0x00, RAM[0x0D].Size);

	for(i = 1; i < BoneCount; i++) {
		glPushMatrix();
		glLoadIdentity();
			glTranslated(Bones[i].X, Bones[i].Y, Bones[i].Z);

			// get current matrix
			glGetIntegerv(GL_MODELVIEW_MATRIX, Bones[i].Matrix);
			int j;
			zl_MulMatrices(Bones[i].Matrix, Bones[i-1].Matrix, Bones[i].Matrix);

			dbgprintf(0, MSK_COLORTYPE_WARNING, "Bone %i matrix, target %08X:", i, 0x0D << 24 | (i-1)*0x40);
			for(j = 0; j < 16; j+=4) {
				dbgprintf(0, MSK_COLORTYPE_INFO, "[%6i] [%6i] [%6i] [%6i]",
					Bones[i].Matrix[j], Bones[i].Matrix[j + 1], Bones[i].Matrix[j + 2], Bones[i].Matrix[j + 3]);
			}

			// write to RAM
			int Offset = (i-1)* 0x40;
			for(j = 0; j < 16; j++) {
				Write16(RAM[0x0D].Data, Offset + 0x20, (Bones[i].Matrix[j] & 0xFFFF0000) >> 16);
				Write16(RAM[0x0D].Data, Offset, (Bones[i].Matrix[j] & 0xFFFF));
				Offset+=2;
			}
		glPopMatrix();
	}
*/
	//render
	glPushMatrix();
		glScalef(Scale, Scale, Scale);
		drawBone(Bones, 0, -1);
	glPopMatrix();
/*
	if(RAM[0x0D].Data != NULL) {
		memset(RAM[0x0D].Data, 0x00, RAM[0x0D].Size);
		RDP_ClearSegment(0x0D);
	}*/
}

int scanAnimations(unsigned char bank)
{
	vCurrentActor.animTotal = -1;

	int i;
	for(i = 0; i < RAM[bank].Size; i += 4) {
		if ((!RAM[bank].Data[i])	&&
			(RAM[bank].Data[i+1] > 1)	&&
			(!RAM[bank].Data[i+2])	&&
			(!RAM[bank].Data[i+3])	&&
			(RAM[bank].Data[i+4] == bank)	&&
			((int) ((RAM[bank].Data[i+5] << 16)|(RAM[bank].Data[i+6]<<8)|(RAM[bank].Data[i+7])) < RAM[bank].Size)	&&
			(RAM[bank].Data[i+8] == bank)	&&
			((int) ((RAM[bank].Data[i+9] << 16)|(RAM[bank].Data[i+10]<<8)|(RAM[bank].Data[i+11])) < RAM[bank].Size)	&&
			(!RAM[bank].Data[i+12]) &&		// ???
			(!RAM[bank].Data[i+14])	&&
			(!RAM[bank].Data[i+15])) {
				vCurrentActor.animTotal++;
				vCurrentActor.offsetAnims[vCurrentActor.animTotal] = (bank << 24) | i;
//					dbgprintf(0,0,"ANIM %i:%08x", vCurrentActor.animTotal,vCurrentActor.offsetAnims[vCurrentActor.animTotal]);
			}
	}

	return 0;
}

int scanBones(unsigned char bank)
{
	vCurrentActor.boneSetupTotal = -1;

	int i, j;
	for (i = 0; i < RAM[bank].Size; i += 4) {
		if ( (RAM[bank].Data[i] == bank) && (!(RAM[bank].Data[i+3] & 3)) && (RAM[bank].Data[i+4]) ) {
			int offset = (int) ((RAM[bank].Data[i+1] << 16)|(RAM[bank].Data[i+2]<<8)|(RAM[bank].Data[i+3]));
			if (offset < RAM[bank].Size) {
			unsigned char NoPts = RAM[bank].Data[i+4];
			int offset_end = offset + (NoPts<<2);
			if (offset_end < RAM[bank].Size) {
				for (j = offset;  j < offset_end; j+=4) {
					if ( (RAM[bank].Data[j] != bank) || ((RAM[bank].Data[j+3] & 3)) || ((int) ((RAM[bank].Data[j+1] << 16)|(RAM[bank].Data[j+2]<<8)|(RAM[bank].Data[j+3])) > RAM[bank].Size))
						break;
				}
				if (j == i) {
					vCurrentActor.boneSetupTotal++;
					vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupTotal] = (bank << 24) | i;
//					dbgprintf(0,0,"BONES %i:%08x", vCurrentActor.boneSetupTotal,vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupTotal]);
				}
				}
			}
		}
	}

	return 0;
}
