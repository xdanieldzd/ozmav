// SM64/Fast3D to Zelda/F3DEX2 level converter
// Jan/Feb 2010 by xdaniel
// additions to collision and scene generation by spinout (Feb 2010)

#include "globals.h"

#define VERSION	"0.2.0"

char ROMFilename[1024];
char ZMapFilename[1024];
char ZSceneFilename[1024];

unsigned char * ROMBuffer;
unsigned char * ZMapBuffer;
unsigned char * ZSceneBuffer;
unsigned char * ColVtxBuffer;
unsigned char * ColTriBuffer;
unsigned char * ZWaterBuffer;
unsigned char * ZActorBuffer;
unsigned char * ZObjectBuffer;
unsigned char * ZLinkBuffer;

int ROMFilesize = 0;
int ZMapFilesize = 0;
int ZSceneFilesize = 0;
int TextureSize = 0;

/* LEVEL SCRIPT INTERPRETER VARIABLES */
unsigned int	LevelLayoutScript_Base = 0x00;
unsigned int	MacroPreset_Offset = 0x00;
unsigned int	MacroPreset_Length = 0x00;
unsigned int	TempOffset = 0x00;

unsigned int	LvlScript_Start = 0x00;
unsigned int	LvlScript_End = 0x00;
unsigned int	LvlScript_Entry = 0x00;
unsigned int	LvlScript_Length = 0x00;

unsigned int	TempScriptPos = 0x00;
unsigned int	TempScriptSegment = 0x00;
unsigned int	TempScriptPos_Backup = 0x00;
unsigned int	TempScriptSegment_Backup = 0x00;

unsigned int	TempGeoScriptPos = 0x00;
unsigned int	TempGeoScriptSegment = 0x00;
unsigned int	TempGeoScriptPos_Backup = 0x00;
unsigned int	TempGeoScriptSegment_Backup = 0x00;

char		CurrentScriptCmdText[256] = "";
unsigned int	CurrentCmd = 0x00;
unsigned int	CurrentCmdLength = 0x00;
bool		JumpInScript = false;
bool		JumpInGeoScript = false;

unsigned int	CurrentLevelArea = 0x01;
unsigned int	LevelArea = 0x01;
unsigned int	LevelID;

signed short linkx=0, linky=0, linkz =0;
unsigned short linkrot;

signed short Xrepos = 0, Yrepos = 0, Zrepos = 0;

/* DL vars */
bool DListHasEnded = false;

unsigned int DListOffsets[512];
int DLCount = 0;

/* Some of these are pretty hacky */
int ColTriCount		= 0;
int ColVtxCount		= 0;
int _ColVtxCount	= 0;
int ZTriOffset		= 0;
int ZWaterCount		= 0;
int ZWaterOffset	= 0;
bool collision_read	= false;
bool fix_collision	= false;

int ZActorCount	= 0;
int ZDoorCount	= 0; //unused
int ZObjectCount	= 0;


int verbose = 1;

struct __RAMSegment RAMSegment[SEG_COUNT];

int WriteActors()
{
	// force object count down
	if(ZObjectCount > 0xF) {
		msg(0, "- Overflow! Object count 0x%02X is over 0xF, ignoring surplus objects...\n", ZObjectCount);
		ZObjectCount = 0xF;
	}

	// objects
	memcpy(&ZMapBuffer[ZMAP_BASEOBJS], ZObjectBuffer, ZObjectCount << 1);

	// force actor count down
	if(ZActorCount > 0x3F) {
		msg(0, "- Overflow! Actor count 0x%02X is over 0x3F, ignoring surplus actors...\n", ZActorCount);
		ZActorCount = 0x3F;
	}

	// actors
	memcpy(&ZMapBuffer[ZMAP_BASEACTORS], ZActorBuffer, ZActorCount << 4);

	return 0;
}

int CreateZMapHeader()
{
	// write base Display List
	int ZMapOffset = ZMAP_BASEDLIST;
	Write64(ZMapBuffer, ZMapOffset, 0xDF000000, 0x00000000); ZMapOffset += 8;

	int DL = 0;
	for(DL = 0; DL < DLCount; DL++) {
		Write64(ZMapBuffer, ZMapOffset, 0xDE000000, (0x03000000 + DListOffsets[DL]));
		ZMapOffset += 8;
	}

	Write64(ZMapBuffer, ZMapOffset, 0xDF000000, 0x00000000);

	// write actor data
	WriteActors();

	// write map header
	ZMapOffset = 0x00;

	Write64(ZMapBuffer, ZMapOffset, 0x16000000, 0x00000000); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x08000000, 0x00000000); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x12000000, 0x00000000); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x10000000, 0x0C000100); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x0A000000, 0x03000060); ZMapOffset += 8;	// mesh header
	Write64(ZMapBuffer, ZMapOffset, (0x0B000000|((ZObjectCount&0xF)<<16)), (0x03000000|ZMAP_BASEOBJS));	//object data
	ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, (0x01000000|((ZActorCount&0x3F)<<16)), (0x03000000|ZMAP_BASEACTORS));	//actor data
	ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x14000000, 0x00000000);

	// write mesh header
	ZMapOffset = 0x60;

	Write64(ZMapBuffer, ZMapOffset, 0x00010000, 0x0300006C); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, 0x03000074, (0x03000000 + ZMAP_BASEDLIST + 8)); ZMapOffset += 8;
	Write64(ZMapBuffer, ZMapOffset, (0x03000000 + ZMAP_BASEDLIST), 0x01000000);

	return 0;
}

int CreateCollision(int Offset) // 
{
	
	msg(3,"- Building collision...\n");
        if (!ColVtxCount)
        {
        	printf("WARNING: No collision to build! Unknown results ahead...\n");
        	fix_collision = false;
        	return 0;
        }
        fix_collision = true;
        // empty collision type
	Write64(ZSceneBuffer, Offset, 0x00000000, 0x00000000);
	// copy triangles
	int TriOff = Offset+8;
	int TriSize = ColTriCount << 4;
	dmsg("- TriOff: 0x%X TriSize 0x%X\n", TriOff,TriSize);
	memcpy(&ZSceneBuffer[TriOff], ColTriBuffer, TriSize);

	// copy verts
	int VtxOff = TriOff + TriSize;
	int VtxSize = ColVtxCount * 6;
	dmsg("- ColVtxCount: %i VtxOff: 0x%X VtxSize 0x%X\n",ColVtxCount, VtxOff, VtxSize);
	memcpy(&ZSceneBuffer[VtxOff], ColVtxBuffer, VtxSize);

	// water
	int WaterOff = VtxOff+VtxSize;
	if (WaterOff & 3) // 4-byte alignment
		WaterOff = (WaterOff & 0xFFFFFFF4) + 4;
	int WaterSize = ZWaterCount << 4;
	dmsg("- WaterOff: 0x%X WaterSize 0x%X\n", WaterOff, WaterSize);
	memcpy(&ZSceneBuffer[WaterOff], ZWaterBuffer, WaterSize);

	// collision header
	int CollisionHeaderOffset = WaterOff + WaterSize;
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x00, 0xC000C000, 0xC0004000);
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x08, 0x40004000, ColVtxCount<<16);
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x10, (0x02000000|VtxOff), ColTriCount<<16);
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x18, (0x02000000|TriOff), (0x02000000|Offset));
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x20, 0x00000000, ZWaterCount << 16);
	Write64(ZSceneBuffer, CollisionHeaderOffset+0x28, (0x02000000|WaterOff), 0x00000000);

	//Collision fixer stuff
	FILE * cini=fopen("collision_fixer.ini","w");
	fprintf(cini, "'6-byte\n%X\n%X\n'16-byte\n%X\n%X", VtxOff, VtxOff+VtxSize, TriOff, TriOff+TriSize);
	fclose(cini);

	free(ColTriBuffer);
	free(ColVtxBuffer);

	return CollisionHeaderOffset | 0x02000000;
}

int CreateZSceneHeader()
{
	int ZSceneOffset = 0x00;
	// collision header offset
	int CollisionOffset = CreateCollision(0x100);
	/* Eww yuck eww */
	Write64(ZSceneBuffer, ZSceneOffset, 0x15000000, 0x00000000); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x04010000, 0x02000068); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x19000000, 0x00000000); ZSceneOffset += 8;
	if (CollisionOffset) {
		Write64(ZSceneBuffer, ZSceneOffset, 0x03000000, CollisionOffset);
		ZSceneOffset += 8;
	}
	Write64(ZSceneBuffer, ZSceneOffset, 0x06000000, 0x02000060); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x07000000, 0x00000003); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x00010000, 0x02000050); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x11000000, 0x01000000); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x0F060000, 0x02000070); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x14000000, 0x00000000); ZSceneOffset += 8;
	
	if (!CollisionOffset) {
		Write64(ZSceneBuffer, ZSceneOffset, 0x00000000, 0x00000000);
		ZSceneOffset += 8;
	}

	/* Link */
	memcpy(&ZSceneBuffer[ZSceneOffset], ZLinkBuffer, 0x16);
	ZSceneOffset += 16;

	/* Link Index, Map offsets (0x6D6170206F666673 = "map offs") */
	Write64(ZSceneBuffer, ZSceneOffset, 0x00000000, 0x00000000); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x6D617020, 0x6F666673); ZSceneOffset += 8;

	/* environment settings (FIXME this can and should be cut down, or maybe copied from M64?) */
	//0x70
	Write64(ZSceneBuffer, ZSceneOffset, 0x3C3C3C4C, 0x4945CDCA); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0xCABDB5B5, 0x534F5514); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x131907E4, 0x32003C3C); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x3C4C4945, 0xCDCACABD); ZSceneOffset += 8;
	//0x90
	Write64(ZSceneBuffer, ZSceneOffset, 0xB5B5534F, 0x55141319); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x07E43200, 0x3C3C3C4C); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x4945CDCA, 0xCABDB5B5); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x534F5514, 0x131907E4); ZSceneOffset += 8;
	//0xB0
	Write64(ZSceneBuffer, ZSceneOffset, 0x32003C3C, 0x3C4C4945); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0xCDCACABD, 0xB5B5534F); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x55141319, 0x07E43200); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x3C3C3C4C, 0x4945CDCA); ZSceneOffset += 8;
	//0xD0
	Write64(ZSceneBuffer, ZSceneOffset, 0xCABDB5B5, 0x534F5514); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x131907E4, 0x32003C3C); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x3C4C4945, 0xCDCACABD); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0xB5B5534F, 0x55141319); ZSceneOffset += 8;
	//0xF0
	Write64(ZSceneBuffer, ZSceneOffset, 0x07E43200, 0x00000000); ZSceneOffset += 8;
	Write64(ZSceneBuffer, ZSceneOffset, 0x00000000, 0x00000000);
	//0x100

	return 0x30+(CollisionOffset&0xFFFFFF);
}

unsigned char * CreatePadding(unsigned int Filesize)
{
	int PadSize = GetPaddingSize(Filesize, Z_PADDING);
	unsigned char * Padding = (unsigned char*)malloc(sizeof(char) * PadSize);
	memset(Padding, 0x00, PadSize);

	return Padding;
}

int main(int argc, char * argv[])
{
	int SceneNumber = -1;
	int zROMOffset = 0x35D0000;
	FILE * zROM = NULL;

	if (argc > 1){
		if (!strcmp(argv[1], "--help")||!strcmp(argv[1], "-h")){
			printf("Prototype:\n%s <SM64 ROM> <map name> <level ID> <level area> [options]\nWhere:\n <SM64 ROM> is a extended NTSC Mario 64 ROM,\n <map name> is the output scene/map name,\n <level ID> is the level in SM64 to convert (hex value), and\n <level area> is the area of the level you want to convert (usually 0x1)\n\nOptions:\n -l\t\tLog messages to file 'log.txt'\n -i ROM SCENE\tInsert scene/map to OoT Debug ROM. SCENE is decimal\n -o ADDRESS\tAddress to insert map/scene at if -i flag is enabled (Hex)\n -a, --about\tDisplay program information\n -h, --help\tThis message\n -v LEVEL\tSet verbosity level, 0-3, default 1\n", argv[0]);
			return 0;
		}
		if (!strcmp(argv[1], "--about")||!strcmp(argv[1], "-a")){
			printf("SM64 Fast3D to Zelda F3DEX2 Converter\nBy xdaniel with additions by spinout\nVersion %s", VERSION);
			#ifdef DEBUG
			printf(" (Debug build)");
			#endif
			printf("\nBuilt %s %s\n", __DATE__, __TIME__);
			return 0;
		}
	}
	
	// check extra args
	if(argc > 5) {
		int curr_arg=5;
		while(curr_arg<argc)
		{
			if(!strcmp(argv[curr_arg], "-l")) {
				FILE * fp;
				if((fp = freopen("log.txt", "w", stdout)) == NULL) printf("Error opening log file.\n");
			}
			else if(!strcmp(argv[curr_arg], "-i")) {
				if(curr_arg+2 >=argc)
					error("Not enough arguments for -i flag\n");
				zROM = fopen(argv[++curr_arg],"r+b");
				if(!zROM)
					error("Cannot open Zelda ROM\n");
				sscanf(argv[++curr_arg], "%i", &SceneNumber);
			}
			else if(!strcmp(argv[curr_arg], "-o")) {
				if(curr_arg+1 >= argc)
					error("Not enough arguments for -o flag\n");
				sscanf(argv[++curr_arg], "%x", &zROMOffset);
			}
			else if(!strcmp(argv[curr_arg], "-v")) {
				if(curr_arg+1 >= argc)
					error("Not enough arguments for -v flag\n");
				sscanf(argv[++curr_arg], "%x", &verbose);
				/* bounds check */
				if(verbose < 0)
					verbose = 0;
				if(verbose > 3)
					verbose = 3;
			}
			curr_arg++;

		}
	}

	if(argc < 5) {
		error("Invalid arguments\nType `%s --help' for help\n",argv[0]);
	}

	// get arguments from command line
	strcpy(ROMFilename, argv[1]);

	sscanf(argv[3], "%x", &LevelID);
	sscanf(argv[4], "%x", &LevelArea);

	if((LevelID < 0x00) || (LevelID > 0x1E)) {
		error("Invalid level ID\n");
	}

	if((LevelArea < 0x01) || (LevelArea > 0x04)) {
		error("Invalid level area\n");
	}

	// load SM64 ROM image into buffer
	msg(1, "Loading ROM image '%s'...\n", ROMFilename);
	FILE * file;
	if((file = fopen(ROMFilename, "rb")) == NULL) {
		error("Cannot open SM64 ROM file.\n");
	}
	fseek(file, 0, SEEK_END);
	ROMFilesize = ftell(file);
	rewind(file);
	ROMBuffer = (unsigned char*) malloc (sizeof(char) * ROMFilesize);
	fread(ROMBuffer, 1, ROMFilesize, file);
	fclose(file);

	//allocate space for actor and object buffers
	ZActorBuffer=(unsigned char*)malloc(0x1000);
	ZObjectBuffer=(unsigned char*)malloc(0x80);
	ZLinkBuffer = (unsigned char*)malloc(0x100);
	ZLink(ZLinkBuffer, 0, 0, 0, 0, 0, 0, 0, 0xFFF);
	/* start executing the level scripts
	 the next 15 or so lines are from mariozmav */

	InitLevelScriptInterpreter();
	dmsg("%i\n", __LINE__);

	TempScriptPos = 0x00;
	TempScriptSegment = 0x15;
	TempScriptPos_Backup = 0x00;
	TempScriptSegment_Backup = 0x00;

	ExecuteLevelScript();
	dmsg("%i\n", __LINE__);

	/* setup / zero some variables we're gonna need soon */
	TempScriptPos = LvlScript_Entry;
	TempScriptSegment = 0x0E;
	TempScriptPos_Backup = 0x00;
	TempScriptSegment_Backup = 0x00;

	msg(2, "Executing SM64 level script for level ID 0x%02X...\n", LevelID);
	ExecuteLevelScript();

	// create Zelda map header
	msg(2, "\nCreating Zelda map header...\n");
	CreateZMapHeader();

	// create buffer for scene file
	ZSceneFilesize = (ColTriCount << 4) + (ColVtxCount * 6) + (ZWaterCount << 4) + 0x200; // Rough estimate of scene size
	ZSceneBuffer = (unsigned char*) realloc (ZSceneBuffer, sizeof(char) * ZSceneFilesize);
	memset(ZSceneBuffer, 0x00, ZSceneFilesize);

	// create Zelda scene header
	msg(2, "Creating Zelda scene header...\n");
	ZSceneFilesize = CreateZSceneHeader();	// Actual scene size

	// generate padding for map file
	unsigned char * MapPadding = CreatePadding(ZMapFilesize);

	// save map file
	sprintf(ZMapFilename, "%s.zmap", argv[2]);
	remove(ZMapFilename);
	msg(1, "Saving Zelda map file to '%s'...\n", ZMapFilename);
	file = fopen(ZMapFilename, "wb");
	fwrite(ZMapBuffer, 1, ZMapFilesize, file);
	fwrite(MapPadding, 1, GetPaddingSize(ZMapFilesize, Z_PADDING), file);
	int zMapSize = ftell(file);
	fclose(file);

	// generate padding for scene file
	unsigned char * ScenePadding = CreatePadding(ZSceneFilesize);

	// save scene file
	sprintf(ZSceneFilename, "%s.zscene", argv[2]);
	remove(ZSceneFilename);
	msg(1, "Saving Zelda scene file to '%s'...\n", ZSceneFilename);
	file = fopen(ZSceneFilename, "wb");
	fwrite(ZSceneBuffer, 1, ZSceneFilesize, file);
	fwrite(ScenePadding, 1, GetPaddingSize(ZSceneFilesize, Z_PADDING), file);
	int zSceneSize = ftell(file);
	fclose(file);

	//Fix collision
	if (fix_collision)
	{
		msg(3, "Fixing collision...\n");

		FILE * rini=fopen("romname.ini","w");
		fprintf(rini, "'Rom you want to edit\n%s\n'Unmodified rom needed for collision fixer\n%s", ZSceneFilename, ZSceneFilename);
		fclose(rini);

		#ifdef WIN32
		system("collision_fixer.exe");
		#else
		system("wine collision_fixer.exe");
		#endif
		
		remove("romname.ini");
		remove("collision_fixer.ini");
	}
	
	/* Write to ROM? */
	if (SceneNumber >= 0){
		msg(1, "Inserting to ROM at offset %08X using scene %i...\n", zROMOffset, SceneNumber);

		char * SceneBuffer = (char*)malloc(zSceneSize);
		char * tmpbuff = (char*)malloc(0x14);

		int zSceneStart = zROMOffset;
		int zSceneEnd = zSceneStart + zSceneSize;
		int zMapStart = zSceneEnd;
		int zMapEnd = zMapStart + zMapSize;

		/* read scene file, so we don't have to fix collision again */
		file = fopen(ZSceneFilename, "rb");
		fread(SceneBuffer, 1,zSceneSize, file);
		fclose(file);

		/* Map pointers */
		Write64(SceneBuffer, 0x68, zMapStart, zMapEnd);

		/* Write scene */
		fseek(zROM, zROMOffset, SEEK_SET);
		fwrite(SceneBuffer, 1, zSceneSize, zROM);

		/* Write map */
		fwrite(ZMapBuffer, 1, ZMapFilesize, zROM);
		fwrite(MapPadding, 1, GetPaddingSize(ZMapFilesize, Z_PADDING), zROM);

		/* Write scene pointers */
		memset(tmpbuff, 0x14, 0x0);
		Write64(tmpbuff, 0, zSceneStart, zSceneEnd);
		fseek(zROM, 0x14 * SceneNumber + 0xBA0BB0, SEEK_SET);
		fwrite(tmpbuff, 1, 0x14, zROM);

		/* Close ROM */
		fflush(zROM);
		fclose(zROM);
		//free(SceneBuffer);
	}


	// we're done!
	msg(1, "\nDone!\n");

	return 0;
}
