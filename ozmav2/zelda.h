typedef struct {
	int ID;
	unsigned int VStart, VEnd;
	unsigned int PStart, PEnd;
	char Filename[256];
} DMA;

typedef struct {
    signed short X, Y, Z;
    signed short RX, RY, RZ;
    signed char Child1, Child2;
    unsigned long DList;
    bool isSet;
}z_bone;

#define Z_ACTOR_MAP	0
#define Z_ACTOR_DOOR	1

extern bool zl_Init(char * Filename);
extern int zl_LoadROM();
extern void zl_InitCombiner();
extern int zl_GetGameVersion();
extern int zl_LoadScene(int SceneNo);
extern DMA zl_DMAGetFile(int DMAFileNo);
extern void zl_DMAGetFilename(char * Name, int DMAFileNo);
extern DMA zl_DMAVirtualToPhysical(unsigned int VStart);
extern int zl_GetFilenameTable();
extern int zl_GetDMATable();
extern int zl_GetSceneTable();
extern void zl_SetMipsWatchers();
extern unsigned char * zl_DMAToBuffer(DMA File);
extern void zl_ClearAllSegments();
extern void zl_ClearViewerStructures();
extern int zl_ExecuteHeader(unsigned char Segment, unsigned int Offset, int SceneNumber, int MapNumber);
extern void zl_GetDisplayLists(int MapNumber);
extern void zl_ExecuteDisplayLists(int MapNumber);
extern void zl_GetMapObjects(int SceneNumber, int MapNumber);
extern void zl_LoadObject(unsigned short ObjNumber);
extern void zl_GetMapActors(int SceneNumber, int MapNumber);
extern void zl_ProcessActor(int MapNumber, int CurrActor, int Type);
extern void zl_DrawBone(z_bone * Bones, int CurrentBone);
extern void zl_DrawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ, GLuint DLBase);
extern void zl_DeInit();
extern void zl_HexPrint(unsigned char * Buffer, int Offset, int Length);

