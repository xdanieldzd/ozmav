typedef struct {
    signed short X, Y, Z;
    signed short RX, RY, RZ;
    signed char Child1, Child2;
    unsigned long DList;
    bool isSet;
}z_bone;

#define Z_ACTOR_MAP	0
#define Z_ACTOR_DOOR	1

extern void zl_SetMipsWatchers();
extern void zl_GetMapObjects(int SceneNumber, int MapNumber);
extern void zl_LoadObject(unsigned short ObjNumber);
extern void zl_GetMapActors(int SceneNumber, int MapNumber);
extern void zl_ProcessActor(int MapNumber, int CurrActor, int Type);
extern void zl_DrawBone(z_bone * Bones, int CurrentBone);
extern void zl_DrawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ, GLuint DLBase);
