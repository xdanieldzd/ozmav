typedef struct {
    signed short X, Y, Z;
    signed short RX, RY, RZ;
    signed char Child1, Child2;
    unsigned long DList;
    bool isSet;
}z_bone;

struct zActorSections{
	unsigned char * text;
	unsigned char * data;
	unsigned char * rodata;
	unsigned char * bss;
	size_t data_s;
	size_t text_s;
	size_t rodata_s;
	size_t bss_s;
	unsigned long text_va;
	unsigned long data_va;
	unsigned long rodata_va;
	unsigned long bss_va;
};

#define Z_ACTOR_MAP	0
#define Z_ACTOR_DOOR	1

extern void zl_SetMipsWatchers();
extern void zl_GetMapObjects(int SceneNumber, int MapNumber);
extern void zl_LoadObject(unsigned short ObjNumber);
extern void zl_GetMapActors(int SceneNumber, int MapNumber);
extern struct zActorSections zl_GetActSections(unsigned char * Data, size_t Siz, unsigned long VStart);
extern void zl_ProcessActor(int MapNumber, int CurrActor, int Type);
extern void zl_DrawBone(z_bone * Bones, int CurrentBone);
extern void zl_DrawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ, GLuint DLBase);
