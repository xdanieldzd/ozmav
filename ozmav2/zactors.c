#include "globals.h"
#include "mips.h"

/*
	zactors.c - all code related to actor and object loading and processing
	 - TODO: cleanup, some documentation for zl_ProcessActor, zl_DrawBone, zl_DrawBones
*/

/*
	zl_SetMipsWatchers - setup mips-eval to watch for certain function calls
*/
void zl_SetMipsWatchers()
{
	mips_SetFuncWatch(0x80035260);// u32 a1 = display list
	mips_SetFuncWatch(0x800A457C);// u32 a2 = bones; u32 a3 = animation
	mips_SetFuncWatch(0x8002D62C);// f32 a1 = scale
	mips_SetFuncWatch(0x8009812C);// u16 a1 = object number to use
	mips_SetFuncWatch(0x80093D18);// u32 a3 = display list
	//mips_SetFuncWatch(0x800D0984);// u32 a0 = display list
	mips_SetFuncWatch(0x800A46F8);// u32 a2 = bones
	mips_SetFuncWatch(0x800A51A0);// u32 a1 = animation
	mips_SetFuncWatch(0x800A46F8);// u32 a2 = bones
	mips_SetFuncWatch(0x800A2000);// a0 = animation
	//mips_SetFuncWatch(0x80041880);// a0 = dlist

	mips_SetFuncWatch(0x800A663C);// a2 = bones a3 = ani Note: different bone structure, possibly different animation type!
	mips_SetFuncWatch(0x80031F50);// a2 = actor number to spawn

	return;
}

/*
	zl_GetMapObjects - load all objects necessary for rendering, as specified by the scene and/or map header
*/
void zl_GetMapObjects(int SceneNumber, int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i, %i);\n", __FUNCTION__, SceneNumber, MapNumber);

	dbgprintf(1, MSK_COLORTYPE_OKAY, "Loading Gameplay object(s)...\n");

	zl_LoadObject(1);		// gameplay_keep
	zl_LoadObject(0x12b);	// grass (object_kusa)
	if(zSHeader[SceneNumber].GameplayObj) zl_LoadObject(zSHeader[SceneNumber].GameplayObj);	// either gameplay_dangeon_keep or gameplay_field_keep

	// do we have objects specified in the map header?
	if(zMHeader[SceneNumber][MapNumber].ObjCount) {
		unsigned char Segment = zMHeader[SceneNumber][MapNumber].ObjOffset >> 24;
		unsigned int Offset = (zMHeader[SceneNumber][MapNumber].ObjOffset & 0x00FFFFFF);

		dbgprintf(1, MSK_COLORTYPE_OKAY, "Loading %i object(s) at 0x%08X...\n", zMHeader[SceneNumber][MapNumber].ObjCount, zMHeader[SceneNumber][MapNumber].ObjOffset);

		int CurrObj = 0;
		unsigned short ObjNumber = 0;

		while(CurrObj < zMHeader[SceneNumber][MapNumber].ObjCount) {
			// get the object number
			ObjNumber = Read16(RAM[Segment].Data, Offset + (CurrObj * 2));
			// load object
			zl_LoadObject(ObjNumber);
			// go to next
			CurrObj++;
		}
	}
}

/*
	zl_LoadObject - load the object with number ObjNumber
*/
void zl_LoadObject(unsigned short ObjNumber)
{
	// if the object has already been loaded, return
	if(zObject[ObjNumber].IsSet){
		return;
	}
	// mark object as not loaded
	zObject[ObjNumber].IsSet = false;

	// get the start/end offsets from the object table
	unsigned int TempStart = Read32(zGame.CodeBuffer, zGame.ObjectTableOffset + (0x8 * ObjNumber));
	unsigned int TempEnd = Read32(zGame.CodeBuffer, zGame.ObjectTableOffset + (0x8 * ObjNumber) + 4);

	// translate those offsets into physical offsets, if necessary
	DMA Object = zl_DMAVirtualToPhysical(TempStart);

	// if the offsets aren't all zero, assume we've got a valid object
	if((TempStart != 0) && (TempEnd != 0)) {
		zObject[ObjNumber].IsSet = true;
		zObject[ObjNumber].StartOffset = Object.PStart;
		zObject[ObjNumber].EndOffset = Object.PEnd;
		zObject[ObjNumber].Data = zl_DMAToBuffer( Object );
	}

	dbgprintf(1, (zObject[ObjNumber].IsSet ? MSK_COLORTYPE_INFO : MSK_COLORTYPE_ERROR), "- %04X -> %08X %08X -> %s (%s)",
		ObjNumber,
		zObject[ObjNumber].StartOffset, zObject[ObjNumber].EndOffset,
		Object.Filename,
		(zObject[ObjNumber].IsSet ? "okay" : "bad"));

	if(zObject[ObjNumber].IsSet && Object.Filename!=NULL && strlen(Object.Filename) < 20){
		zObject[ObjNumber].Name = malloc(20);
		memcpy(zObject[ObjNumber].Name, Object.Filename,20);
	} else {
		zObject[ObjNumber].Name = NULL;
	}
}

/*
	zl_GetMapActors - load actors as specified in the scene and/or map file
*/
void zl_GetMapActors(int SceneNumber, int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i, %i);\n", __FUNCTION__, SceneNumber, MapNumber);

	unsigned char Segment = 0;
	unsigned int Offset = 0;

	int CurrActor = 0;

	// see if we're trying to load actors from the scene or map file
	if(MapNumber == -1) {
		// try to load Links and doors from the scene file

		if(zSHeader[SceneNumber].LinkCount) {
			// Links
			Segment = zSHeader[SceneNumber].LinkOffset >> 24;
			Offset = (zSHeader[SceneNumber].LinkOffset & 0x00FFFFFF);

			dbgprintf(1, MSK_COLORTYPE_OKAY, "Loading %i Link(s) at 0x%08X...", zSHeader[SceneNumber].LinkCount, zSHeader[SceneNumber].LinkOffset);

			while(CurrActor < zSHeader[SceneNumber].LinkCount) {
				zLink[CurrActor].Number = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10));
				zLink[CurrActor].X = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 2);
				zLink[CurrActor].Y = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 4);
				zLink[CurrActor].Z = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 6);
				zLink[CurrActor].RX = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 8);
				zLink[CurrActor].RY = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 10);
				zLink[CurrActor].RZ = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 12);
				zLink[CurrActor].Var = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 14);

				CurrActor++;
			}

			CurrActor = 0;
		}

		if(zSHeader[SceneNumber].DoorCount) {
			// Doors
			Segment = zSHeader[SceneNumber].DoorOffset >> 24;
			Offset = (zSHeader[SceneNumber].DoorOffset & 0x00FFFFFF);

			dbgprintf(1, MSK_COLORTYPE_OKAY, "Loading %i Door(s) at 0x%08X...", zSHeader[SceneNumber].DoorCount, zSHeader[SceneNumber].DoorOffset);

			while(CurrActor < zSHeader[SceneNumber].DoorCount) {
				zDoor[CurrActor].RoomFront = RAM[Segment].Data[Offset + (CurrActor * 0x10)];
				zDoor[CurrActor].FadeFront = RAM[Segment].Data[Offset + (CurrActor * 0x10) + 1];
				zDoor[CurrActor].RoomRear = RAM[Segment].Data[Offset + (CurrActor * 0x10) + 2];
				zDoor[CurrActor].FadeRear = RAM[Segment].Data[Offset + (CurrActor * 0x10) + 3];
				zDoor[CurrActor].Number = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 4);
				zDoor[CurrActor].X = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 6);
				zDoor[CurrActor].Y = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 8);
				zDoor[CurrActor].Z = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 10);
				zDoor[CurrActor].RY = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 12);
				zDoor[CurrActor].Var = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 14);

				if((!zGame.IsCompressed) && (!zGame.GameType)) zl_ProcessActor(MapNumber, CurrActor, 1);

				CurrActor++;
			}
		}

	} else {
		// try to load regular actors from the map file

		if(zMHeader[SceneNumber][MapNumber].ActorCount) {
			// Actors
			Segment = zMHeader[SceneNumber][MapNumber].ActorOffset >> 24;
			Offset = (zMHeader[SceneNumber][MapNumber].ActorOffset & 0x00FFFFFF);

			dbgprintf(1, MSK_COLORTYPE_OKAY, "Loading %i Actor(s) at 0x%08X...", zMHeader[SceneNumber][MapNumber].ActorCount, zMHeader[SceneNumber][MapNumber].ActorOffset);

			while(CurrActor < zMHeader[SceneNumber][MapNumber].ActorCount) {
				zMapActor[MapNumber][CurrActor].Number = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10));
				zMapActor[MapNumber][CurrActor].X = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 2);
				zMapActor[MapNumber][CurrActor].Y = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 4);
				zMapActor[MapNumber][CurrActor].Z = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 6);
				zMapActor[MapNumber][CurrActor].RX = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 8);
				zMapActor[MapNumber][CurrActor].RY = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 10);
				zMapActor[MapNumber][CurrActor].RZ = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 12);
				zMapActor[MapNumber][CurrActor].Var = Read16(RAM[Segment].Data, Offset + (CurrActor * 0x10) + 14);

				// go to actor processing (actor table, etc)
				if((!zGame.IsCompressed) && (!zGame.GameType)) zl_ProcessActor(MapNumber, CurrActor, 0);

	/*			dbgprintf(0, MSK_COLORTYPE_OKAY, "%2i: No%04X;Obj%04X;Var%04X;DL%08X;Ani%08X/%08X;Siz%.3f;%s",
					CurrActor,
					(unsigned short)zMapActor[MapNumber][CurrActor].Number,
					zActor[zMapActor[MapNumber][CurrActor].Number].Object,
					(unsigned short)zMapActor[MapNumber][CurrActor].Var,
					zActor[zMapActor[MapNumber][CurrActor].Number].DisplayList,
					zActor[zMapActor[MapNumber][CurrActor].Number].BoneSetup,
					zActor[zMapActor[MapNumber][CurrActor].Number].Animation,
					zActor[zMapActor[MapNumber][CurrActor].Number].Scale,
					zActor[zMapActor[MapNumber][CurrActor].Number].Name);
	*/
				CurrActor++;
			}
		}
	}

	dbgprintf(1, MSK_COLORTYPE_INFO, "\n");
}

/*
	zl_GetActSections - get actor overlay sections
*/
struct zActorSections zl_GetActSections(unsigned char * Data, size_t Size, unsigned long VStart)
{
	struct zActorSections ret;
	int indent, sections_addr;

	/* get section header */
	indent = Read32(Data, Size-4);
	sections_addr = Size - indent;

	/* set values of structure */
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

unsigned collectables[0x20] = {
	 0x04042440, 0x04042440, 0x04042440, /* rupees 0-2 */
	 0x0403BCD8, /* recovery heart (3) */
	 0x04007860, /* bomb (4) */
	 0x04005AA0, /* arrow (5) */
	 0x0403B030, /* heart peice (6) */
	 0x0403B030, /* alpha heart container FIXME wrong dlist (7) */
	 0x04005AA0, 0x04005AA0, 0x04005AA0, /* arrow (8-A) */
	 0x04007860, /* bomb (B) */
	 0x04015740, 0x04015740,0x04015740,0x04015740,0x04015740, /* C-11 */
	 0x0403BCD8, /* heart (12) */
	 0x04042440, 0x04042440, /* rupees (13,14) */
	 0x04015740, 0x04015740, 0x04015740, 0x04015740, /* 15-18 */
	 0x04007860, /* bomb (19) */
	 0x04015740, 0x04015740, 0x04015740, 0x04015740, 0x04015740, 0x04015740 /* 1A - 1F */
};

/*
	zl_ScanForBones - use pattern recognition to scan for possible bone structures
*/
unsigned int zl_ScanForBones(unsigned char RAMSeg, int BonesNo)
{
	if(!RAM[RAMSeg].IsSet) return 0;

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Scanning for bones...\n");

	int i = 0, j = 0, foundcnt = 0;
	for(i = 0; i < RAM[RAMSeg].Size; i += 4) {
		if((RAM[RAMSeg].Data[i] == RAMSeg) && (!(RAM[RAMSeg].Data[i+3] & 3)) && (RAM[RAMSeg].Data[i+4])) {
			int offset = (int)((RAM[RAMSeg].Data[i+1] << 16)|(RAM[RAMSeg].Data[i+2]<<8)|(RAM[RAMSeg].Data[i+3]));
			if(offset < RAM[RAMSeg].Size) {
				unsigned char NoPts = RAM[RAMSeg].Data[i+4];
				int offset_end = offset + (NoPts<<2);
				if (offset_end < RAM[RAMSeg].Size) {
					for(j = offset; j < offset_end; j+=4) {
						if((RAM[RAMSeg].Data[j] != RAMSeg) || ((RAM[RAMSeg].Data[j+3] & 3)) || ((int) ((RAM[RAMSeg].Data[j+1] << 16)|(RAM[RAMSeg].Data[j+2]<<8)|(RAM[RAMSeg].Data[j+3])) > RAM[RAMSeg].Size))
							break;
					}
					if (j == i) {
						i = RAMSeg << 24 | i;
						dbgprintf(0, MSK_COLORTYPE_OKAY, "Found bones #%i at offset %08X\n", foundcnt, i);
						if(foundcnt == BonesNo) return i;

						foundcnt++;
					}
				}
			}
		}
	}

	if(foundcnt == 0) dbgprintf(0, MSK_COLORTYPE_OKAY, "...no bones found!\n");

	return 0;
}

/*
	zl_ScanForAnims - use pattern recognition to scan for possible animation structures
*/
unsigned int zl_ScanForAnims(unsigned char RAMSeg, int AnimNo)
{
	if(!RAM[RAMSeg].IsSet) return 0;

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Scanning for animations...\n");

	int i = 0, foundcnt = 0, offset = 0;
	for(i = 0; i < RAM[RAMSeg].Size; i += 4) {
		if(
			(!RAM[RAMSeg].Data[i])	&&
			(RAM[RAMSeg].Data[i+1] > 1)	&&
			(!RAM[RAMSeg].Data[i+2])	&&
			(!RAM[RAMSeg].Data[i+3])	&&
			(RAM[RAMSeg].Data[i+4] == RAMSeg)	&&
			((int) ((RAM[RAMSeg].Data[i+5] << 16)|(RAM[RAMSeg].Data[i+6]<<8)|(RAM[RAMSeg].Data[i+7])) < RAM[RAMSeg].Size)	&&
			(RAM[RAMSeg].Data[i+8] == RAMSeg)	&&
			((int) ((RAM[RAMSeg].Data[i+9] << 16)|(RAM[RAMSeg].Data[i+10]<<8)|(RAM[RAMSeg].Data[i+11])) < RAM[RAMSeg].Size)	&&
			(!RAM[RAMSeg].Data[i+14])	&&
			(!RAM[RAMSeg].Data[i+15])
			)
			{
				offset = (RAMSeg << 24) | i;
				dbgprintf(0, MSK_COLORTYPE_OKAY, "Found animation #%i at offset %08X\n", foundcnt, offset);
				if(foundcnt == AnimNo) return offset;

				foundcnt++;
			}
	}

	if(foundcnt == 0) dbgprintf(0, MSK_COLORTYPE_OKAY, "...no animations found!\n");

	return 0;
}

/*
	zl_ProcessActor - do all actions necessary for loading, processing and rendering each actor
*/
void zl_ProcessActor(int MapNumber, int CurrActor, int Type)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i, %i, %i);\n", __FUNCTION__, MapNumber, CurrActor, Type);

	unsigned short ActorNumber = 0, Var = 0;
	short X = 0, Y = 0, Z = 0, RX = 0, RY = 0, RZ = 0;
	GLuint DLBase = 0;

	switch(Type) {
		case Z_ACTOR_MAP:	{
			//map actors
			ActorNumber = zMapActor[MapNumber][CurrActor].Number;
			X = zMapActor[MapNumber][CurrActor].X;
			Y = zMapActor[MapNumber][CurrActor].Y;
			Z = zMapActor[MapNumber][CurrActor].Z;
			RX = zMapActor[MapNumber][CurrActor].RX;
			RY = zMapActor[MapNumber][CurrActor].RY;
			RZ = zMapActor[MapNumber][CurrActor].RZ;
			Var = zMapActor[MapNumber][CurrActor].Var;

			//drawing stuff
			zMapActor[MapNumber][CurrActor].GLDList = glGenLists(1);
			glListBase(zMapActor[MapNumber][CurrActor].GLDList);
			DLBase = zMapActor[MapNumber][CurrActor].GLDList;
			break; }
		case Z_ACTOR_DOOR: {
			//doors
			ActorNumber = zDoor[CurrActor].Number;
			X = zDoor[CurrActor].X;
			Y = zDoor[CurrActor].Y;
			Z = zDoor[CurrActor].Z;
			RX = 0;
			RY = zDoor[CurrActor].RY;
			RZ = 0;
			Var = zDoor[CurrActor].Var;

			//drawing stuff
			zDoor[CurrActor].GLDList = glGenLists(1);
			glListBase(zDoor[CurrActor].GLDList);
			DLBase = zDoor[CurrActor].GLDList;
			break; }
	}

	// if the actor hasn't been processed yet, do so
	if(zActor[ActorNumber].IsSet == false) {
		dbgprintf(1, MSK_COLORTYPE_INFO, "- Evaluating actor 0x%04X...", ActorNumber);

		// get the base offset for reading from the actor table
		unsigned int BaseOffset = zGame.ActorTableOffset + (ActorNumber * 0x20);

		// get the data itself
		zActor[ActorNumber].PStart = Read32(zGame.CodeBuffer, BaseOffset);
		zActor[ActorNumber].PEnd = Read32(zGame.CodeBuffer, BaseOffset + 4);
		zActor[ActorNumber].VStart = Read32(zGame.CodeBuffer, BaseOffset + 8);
		zActor[ActorNumber].VEnd = Read32(zGame.CodeBuffer, BaseOffset + 12);
		zActor[ActorNumber].ProfileVStart = Read32(zGame.CodeBuffer, BaseOffset + 20);
		zActor[ActorNumber].NameRStart = Read32(zGame.CodeBuffer, BaseOffset + 24);

		// if game is not compressed...
		if(!zGame.IsCompressed) {
			// calculate where the actor name starts inside the code file
			zActor[ActorNumber].NameCStart = (zActor[ActorNumber].NameRStart - zGame.CodeRAMOffset);

			// and read the name out
			unsigned char * Current = &zGame.CodeBuffer[zActor[ActorNumber].NameCStart];
			Current += sprintf(zActor[ActorNumber].Name, "%s", Current);
			while(!*Current) Current++;

			dbgprintf(1, MSK_COLORTYPE_INFO, "- Actor is called '%s'", zActor[ActorNumber].Name);
		}

		// get the actor's physical offsets via the DMA table
		DMA Actor = zl_DMAVirtualToPhysical(zActor[ActorNumber].PStart);
		// if the physical offset are not zero, continue with the processing
		if((Actor.PStart != 0) && (Actor.PEnd != 0))
		{
			/*
			- load it to a buffer
			- get object number
			? object loaded
				? oot debug
					- get .text size
					- mips_EvalWords() on .text section
					- evaluate those results
				? fallback
					- Hacks
					- Scan for 0xE7
			*/
			zActor[ActorNumber].Data = zl_DMAToBuffer( Actor );

			zActor[ActorNumber].Size = Actor.VEnd - Actor.VStart;

			zActor[ActorNumber].Object = Read16(zActor[ActorNumber].Data, (zActor[ActorNumber].ProfileVStart - zActor[ActorNumber].VStart) + 8);

			dbgprintf(1, MSK_COLORTYPE_INFO, "- Actor uses object 0x%04X (%s loaded)", zActor[ActorNumber].Object, (zObject[zActor[ActorNumber].Object].IsSet) ? "is" : "not");

			// check if the actor's requested object has been loaded before
			if(zObject[zActor[ActorNumber].Object].IsSet == true) {
				unsigned char TargetSeg = 0x06;
				float * scale = NULL;
				int *dlist = NULL, *bones = NULL, *anim = NULL, *alt_objn = NULL, *spawnact = NULL;
				struct zActorSections Sections;

				if(zActor[ActorNumber].Object == 0x0001) {
					TargetSeg = 0x04;
				} else if(zActor[ActorNumber].Object == 0x0002) {
					TargetSeg = 0x05;
				} else {
					TargetSeg = 0x06;
				}

				RAM[TargetSeg].Data = zObject[zActor[ActorNumber].Object].Data;
				RAM[TargetSeg].Size = zObject[zActor[ActorNumber].Object].EndOffset - zObject[zActor[ActorNumber].Object].StartOffset;
				RAM[TargetSeg].IsSet = true;
				// Set Watch for variables
				mips_ResetSpecialOps();
				mips_SetSpecialOp(MIPS_LH(mips_r0, 0x1C, mips_a0), Var);
				mips_SetSpecialOp(MIPS_LH(mips_r0, 0x1C, mips_s0), Var);

				/* get actor sections */
				Sections = zl_GetActSections(zActor[ActorNumber].Data, zActor[ActorNumber].Size, zActor[ActorNumber].VStart);

				/* clear previous ram map */
				mips_ResetMap();

				/* set sections */
				if(Sections.data_s)
					mips_SetMap(Sections.data,	Sections.data_s,	Sections.data_va);
				if(Sections.rodata_s)
					mips_SetMap(Sections.rodata,	Sections.rodata_s,	Sections.rodata_va);
				if(Sections.bss_s)
					mips_SetMap(Sections.bss,	Sections.bss_s,		Sections.bss_va);

				/* clear pevious evaluation */
				mips_ResetResults();

				dbgprintf(1, MSK_COLORTYPE_INFO, " - Sections:\n  -.text=0x%X; .data=0x%X; .rodata=0x%X; .bss=0x%X;\n - Evaluating ASM...\n",
					Sections.text_va, Sections.data_va, Sections.rodata_va, Sections.bss_va);

				/* interpret words */
				mips_EvalWords((unsigned int *)Sections.text, Sections.text_s / 4);

				if(zGame.ActorTableOffset == 0x0F9440)
				{
					scale	= mips_GetFuncArg(0x8002D62C,1,true);
					dlist	= mips_GetFuncArg(0x80035260,1,true);
					bones	= mips_GetFuncArg(0x800A457C,2,true);
					if(bones == NULL || !*bones){
						bones = mips_GetFuncArg(0x800A46F8,2,true);
						if(bones == NULL || !*bones){
							bones = mips_GetFuncArg(0x800A46F8,2,true);
						}
					}
					anim	= mips_GetFuncArg(0x800A457C,3,true);
					if(anim == NULL || !*anim){
						anim = mips_GetFuncArg(0x800A51A0,1,true);
						if(anim == NULL){
							anim = mips_GetFuncArg(0x800A2000,0,true);
						}
					}
					alt_objn = mips_GetFuncArg(0x8009812C,1,true);
					if(dlist == NULL || *dlist > 0x80000000){
						dlist = mips_GetFuncArg(0x80093D18, 3,true);
						/*if(dlist == NULL || *dlist > 0x80000000){
							dlist = mips_GetFuncArg(0x80041880, 0, true);
						}*/
					}
					spawnact = mips_GetFuncArg(0x80031F50,3,false); // maybe...
				}

				zActor[ActorNumber].Animation = (anim != NULL) ? *anim : 0;
				zActor[ActorNumber].BoneSetup = (bones != NULL) ? *bones : 0;
				zActor[ActorNumber].Scale = (scale != NULL) ? *scale : 0.01f;
				zActor[ActorNumber].DisplayList = (dlist != NULL) ? *dlist : 0;

				if(spawnact!=NULL)
					dbgprintf(2, MSK_COLORTYPE_INFO, "  - Note: Spawns actor 0x%04X (not spawning)", *spawnact);

				if(alt_objn!=NULL && *alt_objn <= zGame.ObjectCount && zActor[ActorNumber].Object < 3 && *alt_objn > 3 && zObject[*alt_objn].IsSet){
					zActor[ActorNumber].Object = *alt_objn;
					dbgprintf(0, MSK_COLORTYPE_INFO, "  - Alternate object found for actor %s: 0x%04X", zActor[ActorNumber].Name, *alt_objn);
				}
				if(!(zActor[ActorNumber].DisplayList >> 24) && zActor[ActorNumber].DisplayList){
					zActor[ActorNumber].DisplayList = 0;
				}

				// -------- CODE BELOW IS UGLY HACK --------
				if(zActor[ActorNumber].Object == 0x61){	//warp pad scale
					zActor[ActorNumber].Scale = 1.0;
				}
				if(zActor[ActorNumber].Object == 0x170){ //crate scale
					zActor[ActorNumber].Scale = 0.1f;
				}
				if(zActor[ActorNumber].Scale < 0.001 && !zActor[ActorNumber].BoneSetup){
					zActor[ActorNumber].Scale = 0.1f;
				}
				// bombable boulder
				if(zActor[ActorNumber].Object == 0x163) {
					zActor[ActorNumber].Scale = 0.1;
				}
				// greenery
				if((zActor[ActorNumber].Object == 0x7C) && (ActorNumber == 0x77)) {
					zActor[ActorNumber].Scale = 1.0;
				}
				// greenery
				if(ActorNumber == 0x14E) {
					zActor[ActorNumber].Scale = 0.1;
				}
				// deku tree stuff
				else if(zActor[ActorNumber].Object == 0x36) {
					zActor[ActorNumber].Scale = 0.1;
				}
				// windmill spinning thingy
				if((zActor[ActorNumber].Object == 0x6C) && (ActorNumber == 0x123)) {
					zActor[ActorNumber].Scale = 0.1;
				}

				// pot
				if(ActorNumber == 0x111) {
					zActor[ActorNumber].Object = 0x12C;
					zActor[ActorNumber].DisplayList = 0x060017C0;
					zActor[ActorNumber].Scale = 0.2;
				// where does 0x060002E0 come from for actors using object 0x0001?? ... Grass!
				} else if(ActorNumber == 0x125){
					zActor[ActorNumber].Object = 0x12B;
					zActor[ActorNumber].DisplayList = 0x06000140;
				} else if(ActorNumber == 0xA){	//treasure chest
					zActor[ActorNumber].BoneSetup = 0;
					zActor[ActorNumber].DisplayList = 0x060006F0;
				}

				if(zActor[ActorNumber].DisplayList > 0x80000000 || ((!zActor[ActorNumber].DisplayList) && (!zActor[ActorNumber].BoneSetup))){//(zActor[ActorNumber].Scale <= 0.01f)) {
					dbgprintf(1, MSK_COLORTYPE_WARNING, "  - Not all required information found, trying to find via hacks...");

					// flames
					if(ActorNumber == 0x8) {
						zActor[ActorNumber].DisplayList = 0x0404D4E0;	//mips eval can't pick this up yet but it's there.
						zActor[ActorNumber].Scale = 0.005;

					// sliding doors
					} else if((zActor[ActorNumber].Object == 0x1) && (ActorNumber == 0x2E)) {
						zActor[ActorNumber].DisplayList = 0x04049FE0;	// door
//						zActor[ActorNumber].DisplayList = 0x0404B0D0;	// metal bars
						zActor[ActorNumber].Scale = 1.0;

					// gossip stones
					} else if(zActor[ActorNumber].Object == 0x188) {
						zActor[ActorNumber].DisplayList = 0x06000950;
						zActor[ActorNumber].Scale = 0.1;
						//really ugly hack - NOP an ENDDL
						RAM[0x06].Data[0x9C8] = 0;

					// gravestones
					} else if(zActor[ActorNumber].Object == 0xA2) {
						zActor[ActorNumber].DisplayList = 0x060001B0;
						zActor[ActorNumber].Scale = 0.15;

					// torch
					} else if(zActor[ActorNumber].Object == 0xA4) {
						zActor[ActorNumber].Scale = 1.0;
						zActor[ActorNumber].DisplayList = 0x06000870;

					// sign
					} else if(ActorNumber == 0x141) {
						zActor[ActorNumber].DisplayList = 0x0403C050;
						zActor[ActorNumber].Object = 1;

					// ? forgot
					} else if(zActor[ActorNumber].Object == 0x11F) {
						zActor[ActorNumber].DisplayList = 0x060006B0;
						zActor[ActorNumber].Scale = 0.15;

					// ? forgot
					} else if(zActor[ActorNumber].Object == 0x5F) {
						zActor[ActorNumber].DisplayList = 0x06000970;
						zActor[ActorNumber].Scale = 1.0;

					// deku tree mouth
					} else if(ActorNumber == 0x3E) {
						zActor[ActorNumber].DisplayList = 0x060009D0;
						zActor[ActorNumber].Scale = 1.0;

					//spider webs
					} else if(ActorNumber == 0xF) {
						zActor[ActorNumber].DisplayList = 0x060061B0;

					// everything else, atm disabled
					} else if(zActor[ActorNumber].Object > 0x3 && 0){
						int i = 0;
						for(i = 0; i < RAM[0x06].Size; i+=8) {
							unsigned int w0 = Read32(RAM[0x06].Data, i);
							unsigned int w1 = Read32(RAM[0x06].Data, i + 1);
							//assume 1st 0xe7 is entry point
							if((w0 == 0xe7000000) && (w1 == 0x00000000)) {
								zActor[ActorNumber].DisplayList = 0x06000000 | i;
								break;
							}
						}
						zActor[ActorNumber].Scale = 0.25;
					} else {
						zActor[ActorNumber].DisplayList = 0;
					}
				}

				if(!RDP_CheckAddressValidity(zActor[ActorNumber].Animation)) {
//					zActor[ActorNumber].Animation = zl_ScanForAnims(TargetSeg, 0);
				}
				if(zActor[ActorNumber].BoneSetup != 0 && !RDP_CheckAddressValidity(zActor[ActorNumber].BoneSetup)) {
//					zActor[ActorNumber].BoneSetup = zl_ScanForBones(TargetSeg, 0);
				}
				// end hack
			}
		}
		else
			zActor[ActorNumber].DisplayList = 0;

		// mark actor as processed
		zActor[ActorNumber].IsSet = true;

		dbgprintf(0, MSK_COLORTYPE_INFO, "  - Scale %.3f, DL %08X, Bones %08X, Ani %08X\n     %s, Obj %04X\n",
			zActor[ActorNumber].Scale, zActor[ActorNumber].DisplayList,
			zActor[ActorNumber].BoneSetup, zActor[ActorNumber].Animation,
			zActor[ActorNumber].Name, zActor[ActorNumber].Object);

		dbgprintf(1, MSK_COLORTYPE_INFO, "- Actor 0x%04X has been processed.", ActorNumber);

	} else {
		dbgprintf(1, MSK_COLORTYPE_INFO, "- Actor 0x%04X already known...", ActorNumber);
	}

	//collectables
	if(ActorNumber == 0x15 && zGame.ActorTableOffset == 0x0F9440){
		zActor[ActorNumber].DisplayList = collectables[Var&0x1F];
		zActor[ActorNumber].Scale = 0.01f;
		zActor[ActorNumber].Object = 1;
	}

	if(zActor[ActorNumber].BoneSetup || zActor[ActorNumber].DisplayList){
		/* set up object */
		unsigned char TargetSeg = 0x06;

		if(zActor[ActorNumber].Object == 0x0001) {
			TargetSeg = 0x04;
		} else if(zActor[ActorNumber].Object == 0x0002) {
			TargetSeg = 0x05;
		} else {
			TargetSeg = 0x06;
		}

		zActor[ActorNumber].DisplayList = (TargetSeg << 24) | (zActor[ActorNumber].DisplayList & 0x00FFFFFF);

		RAM[TargetSeg].Data = zObject[zActor[ActorNumber].Object].Data;
		RAM[TargetSeg].Size = zObject[zActor[ActorNumber].Object].EndOffset - zObject[zActor[ActorNumber].Object].StartOffset;
		RAM[TargetSeg].IsSet = true;

		//Bone structure
		if(zActor[ActorNumber].BoneSetup) {
			dbgprintf(0, MSK_COLORTYPE_OKAY, " - Drawing bone structure for actor %04X", ActorNumber);
			zl_DrawBones(zActor[ActorNumber].BoneSetup, zActor[ActorNumber].Animation, zActor[ActorNumber].Scale, X, Y, Z, RX, RY, RZ, DLBase);
		}
		//Display list
		else if(zActor[ActorNumber].DisplayList) {
			dbgprintf(0, MSK_COLORTYPE_OKAY, " - Trying to execute DList (%08X) for object 0x%04X...",zActor[ActorNumber].DisplayList, zActor[ActorNumber].Object);

			if(RDP_CheckAddressValidity(zActor[ActorNumber].DisplayList)) {
				dbgprintf(0, MSK_COLORTYPE_OKAY, " - DList Address 0x%08X is valid!", zActor[ActorNumber].DisplayList);
				glNewList(DLBase, GL_COMPILE);
					glPushMatrix();

					glTranslated(X, Y, Z);
					glRotated(RX / 182.0444444, 1, 0, 0);
					glRotated(RY / 182.0444444, 0, 1, 0);
					glRotated(RZ / 182.0444444, 0, 0, 1);
					glScalef(zActor[ActorNumber].Scale, zActor[ActorNumber].Scale, zActor[ActorNumber].Scale);

					RDP_ClearStructures(false);

					if(!zOptions.DumpModel) RDP_ParseDisplayList(zActor[ActorNumber].DisplayList, true);

					glPopMatrix();
				glEndList();
			}
		}

		RAM[TargetSeg].Data = NULL;
		RAM[TargetSeg].Size = 0;
		RAM[TargetSeg].IsSet = false;

	} else { // draw a cube
		dbgprintf(0, MSK_COLORTYPE_INFO, " - Drawing a cube :(");

		if(!zOptions.DumpModel) {
			glNewList(DLBase, GL_COMPILE);
				glPushMatrix();
				glTranslated(X, Y, Z);
				glRotated(RX / 182.0444444, 1, 0, 0);
				glRotated(RY / 182.0444444, 0, 1, 0);
				glRotated(RZ / 182.0444444, 0, 0, 1);
				glScalef(10.0, 10.0, 10.0);
				glBegin(GL_QUADS);
					// Front Face
					glVertex3f(-1.0f, -1.0f,  1.0f);
					glVertex3f( 1.0f, -1.0f,  1.0f);
					glVertex3f( 1.0f,  1.0f,  1.0f);
					glVertex3f(-1.0f,  1.0f,  1.0f);
					// Back Face
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f,  1.0f, -1.0f);
					glVertex3f( 1.0f,  1.0f, -1.0f);
					glVertex3f( 1.0f, -1.0f, -1.0f);
					// Top Face
					glVertex3f(-1.0f,  1.0f, -1.0f);
					glVertex3f(-1.0f,  1.0f,  1.0f);
					glVertex3f( 1.0f,  1.0f,  1.0f);
					glVertex3f( 1.0f,  1.0f, -1.0f);
					// Bottom Face
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f( 1.0f, -1.0f, -1.0f);
					glVertex3f( 1.0f, -1.0f,  1.0f);
					glVertex3f(-1.0f, -1.0f,  1.0f);
					// Right Face
					glVertex3f( 1.0f, -1.0f, -1.0f);
					glVertex3f( 1.0f,  1.0f, -1.0f);
					glVertex3f( 1.0f,  1.0f,  1.0f);
					glVertex3f( 1.0f, -1.0f,  1.0f);
					// Left Face
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f,  1.0f);
					glVertex3f(-1.0f,  1.0f,  1.0f);
					glVertex3f(-1.0f,  1.0f, -1.0f);
				glEnd();
				glPopMatrix();
			glEndList();
		}
	}
}

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

void zl_DrawBone(z_bone Bones[], int CurrentBone, int ParentBone)
{
	#if 0
	//borrowed from UoT
	glDisable(GL_TEXTURE);
	glDisable(GL_LIGHTING);
	glDepthRange(0, 0);
	glLineWidth(9);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3d(0, 0, 0);
	glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
	glEnd();
	glPointSize(11);
	glBegin(GL_POINTS);
	glColor3f(0, 0, 0);
	glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
	glEnd();
	glPointSize(8);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3d(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
	glEnd();
	glPointSize(1);
	glLineWidth(1);
	glDepthRange(0, 1);
	glEnable(GL_TEXTURE);
	glEnable(GL_LIGHTING);
	#endif

	glPushMatrix();

	glTranslated(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
	glRotated(Bones[CurrentBone].RZ / 182.0444444, 0, 0, 1);
	glRotated(Bones[CurrentBone].RY / 182.0444444, 0, 1, 0);
	glRotated(Bones[CurrentBone].RX / 182.0444444, 1, 0, 0);
/*
	if(CurrentBone > 0) {
		glPushMatrix();
		glLoadIdentity();
			glTranslated(Bones[CurrentBone].X, Bones[CurrentBone].Y, Bones[CurrentBone].Z);
			glRotated(Bones[CurrentBone].RZ / 182.0444444, 0, 0, 1);
			glRotated(Bones[CurrentBone].RY / 182.0444444, 0, 1, 0);
			glRotated(Bones[CurrentBone].RX / 182.0444444, 1, 0, 0);

			// get current matrix
			glGetIntegerv(GL_MODELVIEW_MATRIX, Bones[CurrentBone].Matrix);
			int i;
//			zl_MulMatrices(Bones[CurrentBone].Matrix, Bones[ParentBone].Matrix, Bones[CurrentBone].Matrix);
//			dbgprintf(0, MSK_COLORTYPE_WARNING, "Bone %i matrix, target %08X:", CurrentBone, 0x0D << 24 | CurrentBone*0x40);
//			for(i = 0; i < 16; i+=4) {
//				dbgprintf(0, MSK_COLORTYPE_INFO, "[%6i] [%6i] [%6i] [%6i]",
//					Bones[CurrentBone].Matrix[i], Bones[CurrentBone].Matrix[i + 1], Bones[CurrentBone].Matrix[i + 2], Bones[CurrentBone].Matrix[i + 3]);
//			}

			// write to RAM
			int Offset = (CurrentBone-1)* 0x40;
			for(i = 0; i < 16; i++) {
				Write16(RAM[0x0D].Data, Offset + 0x20, (Bones[CurrentBone].Matrix[i] & 0xFFFF0000) >> 16);
				Write16(RAM[0x0D].Data, Offset, (Bones[CurrentBone].Matrix[i] & 0xFFFF));
				Offset+=2;
			}

//			dbgprintf(0, MSK_COLORTYPE_WARNING, "CHECK ->");
//			float TempMatrix2[4][4];
//			int x,y, MtxTemp1, MtxTemp2;
//			Offset = CurrentBone * 0x40;
//			for(x = 0; x < 4; x++) {
//				for(y = 0; y < 4; y++) {
//					MtxTemp1 = Read16(RAM[0x0D].Data, Offset);
//					MtxTemp2 = Read16(RAM[0x0D].Data, Offset + 32);
//					TempMatrix2[x][y] = ((MtxTemp1 << 16) | MtxTemp2) * (1.0f / 65536.0f);
//					Offset += 2;
//				}
//				dbgprintf(0, 0, "[% 6.0f] [% 6.0f] [% 6.0f] [% 6.0f]", TempMatrix2[x][0], TempMatrix2[x][1], TempMatrix2[x][2], TempMatrix2[x][3]);
//			}
		glPopMatrix();
	}
*/
	//Draw display list
	if(Bones[CurrentBone].DList && RDP_CheckAddressValidity(Bones[CurrentBone].DList)){
		RDP_ClearStructures(false);
		if(!zOptions.DumpModel) RDP_ParseDisplayList(Bones[CurrentBone].DList, true);
	}

	//Draw child
	if(Bones[CurrentBone].Child > -1){
		zl_DrawBone(Bones, Bones[CurrentBone].Child, CurrentBone);
	}

	glPopMatrix();

	//Draw sibling/next bone
	if(Bones[CurrentBone].Sibling > -1){
		zl_DrawBone(Bones, Bones[CurrentBone].Sibling, ParentBone);
	}
}

void zl_DrawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ, GLuint DLBase)
{
	dbgprintf(2, MSK_COLORTYPE_INFO, "%s(0x%x, 0x%x, %.3f,\n  %i, %i, %i, %i, %i, %i, %i);\n",__FUNCTION__, BoneOffset, AnimationOffset, Scale, X, Y, Z, RX, RY, RZ, DLBase);
	int BoneCount, BoneListListOffset, Seg, _Seg, i, AniSeg=0, RotIndexOffset=0, RotValOffset=0;

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

	z_bone * Bones = malloc(sizeof(z_bone) * BoneCount);
	memset(Bones, 0, sizeof(z_bone) * BoneCount);

	if(RDP_CheckAddressValidity(AnimationOffset)){
		AniSeg = AnimationOffset >> 24;
		AnimationOffset &= 0xFFFFFF;
		RotIndexOffset = Read32((RAM[AniSeg].Data), (AnimationOffset+8));
		RotIndexOffset &= 0xFFFFFF;
		RotValOffset = Read32((RAM[AniSeg].Data), (AnimationOffset+4));
		RotValOffset &= 0xFFFFFF;
		/*Bones[0].X = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset) * 2) );
		Bones[0].Y = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset+2) * 2) );
		Bones[0].Z = Read16(RAM[AniSeg].Data, RotValOffset + (Read16(RAM[AniSeg].Data, RotIndexOffset+4) * 2) );*/
		RotIndexOffset += 6;
	}

	Seg = (BoneListListOffset >> 24) & 0xFF;
	BoneListListOffset &= 0xFFFFFF;
	dbgprintf(2, MSK_COLORTYPE_INFO, " - Seg=0x%x; BoneListListOffset=0x%x; BoneCount=%i", Seg, BoneListListOffset, BoneCount);

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
		if(AniSeg && RDP_CheckAddressValidity((AniSeg<<24)|(RotIndexOffset + (i * 6) + 4) ) ){
			unsigned short RXIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6));
			unsigned short RYIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6) + 2);
			unsigned short RZIndex = Read16(RAM[AniSeg].Data, RotIndexOffset + (i * 6) + 4);

			Bones[i].RX = Read16(RAM[AniSeg].Data, RotValOffset + (RXIndex * 2));
			Bones[i].RY = Read16(RAM[AniSeg].Data, RotValOffset + (RYIndex * 2));
			Bones[i].RZ = Read16(RAM[AniSeg].Data, RotValOffset + (RZIndex * 2));
		}
		dbgprintf(2, MSK_COLORTYPE_INFO, " Bone %2i (%08X): (%6i %6i %6i) (%2i %2i) %08X", i, BoneOffset, Bones[i].X, Bones[i].Y, Bones[i].Z, Bones[i].Child, Bones[i].Sibling, Bones[i].DList);
	}
/*
	RAM[0x0D].Size = (BoneCount+1)*0x40;
	RAM[0x0D].Data = (unsigned char*)malloc(sizeof(char)*RAM[0x0D].Size);
	RAM[0x0D].IsSet = true;
	memset(RAM[0x0D].Data, 0x00, RAM[0x0D].Size);
*/
	//render
	glNewList(DLBase, GL_COMPILE_AND_EXECUTE);
		glPushMatrix();

		glTranslated(X, Y, Z);
		glRotated(RX / 182.0444444, 1, 0, 0);
		glRotated(RY / 182.0444444, 0, 1, 0);
		glRotated(RZ / 182.0444444, 0, 0, 1);
		glScalef(Scale, Scale, Scale);

		zl_DrawBone(Bones, 0, -1);

		glPopMatrix();
	glEndList();
/*
	memset(RAM[0x0D].Data, 0x00, RAM[0x0D].Size);
	RDP_ClearSegment(0x0D);
*/
	free(Bones);
}

void zl_SaveMapActors(int SceneNumber, int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i, %i);\n", __FUNCTION__, SceneNumber, MapNumber);

	int CurrActor = 0;

	if(zMHeader[SceneNumber][MapNumber].ActorCount) {
		unsigned char Segment = zMHeader[SceneNumber][MapNumber].ActorOffset >> 24;
		unsigned int Offset = (zMHeader[SceneNumber][MapNumber].ActorOffset & 0x00FFFFFF);

		while(CurrActor < zMHeader[SceneNumber][MapNumber].ActorCount) {
//			dbgprintf(0, MSK_COLORTYPE_WARNING, "-writing actor %i...\n", CurrActor);
//			dbgprintf(0, MSK_COLORTYPE_WARNING, " -old X is %i...\n", Read16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 2)));
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10)), zMapActor[MapNumber][CurrActor].Number);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 2), zMapActor[MapNumber][CurrActor].X);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 4), zMapActor[MapNumber][CurrActor].Y);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 6), zMapActor[MapNumber][CurrActor].Z);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 8), zMapActor[MapNumber][CurrActor].RX);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 10), zMapActor[MapNumber][CurrActor].RY);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 12), zMapActor[MapNumber][CurrActor].RZ);
			Write16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 14), zMapActor[MapNumber][CurrActor].Var);
//			dbgprintf(0, MSK_COLORTYPE_WARNING, " -NEW X is %i...\n", Read16(RAM[Segment].Data, (Offset + (CurrActor * 0x10) + 2)));

			CurrActor++;
		}

		RDP_SaveSegment(Segment, zROM.Data);
	}

	return;
}
