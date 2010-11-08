typedef struct {
	int ID;
	unsigned int VStart, VEnd;
	unsigned int PStart, PEnd;
	char Filename[256];
} DMA;

extern bool zl_Init(char * Filename);
extern int zl_LoadROM();
extern void zl_ShowROMInformation();
extern void zl_InitCombiner();
extern int zl_GetGameVersion();
extern int zl_ReadData();
extern DMA zl_DMAGetFile(int DMAFileNo);
extern void zl_DMAGetFilename(char * Name, int DMAFileNo);
extern DMA zl_DMAVirtualToPhysical(unsigned int VStart, unsigned int VEnd);
extern int zl_GetFilenameTable();
extern int zl_GetDMATable();
extern int zl_GetCodeFile();
extern unsigned char * zl_DMAToBuffer(DMA File);
extern void zl_ClearAllSegments();
extern void zl_DeInit();
extern void zl_HexPrint(unsigned char * Buffer, int Offset, int Length);
