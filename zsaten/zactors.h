struct actorSections {
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

typedef struct {
    signed short X, Y, Z;
    signed short RX, RY, RZ;
    signed char Child, Sibling;
    unsigned long DList;
    bool isSet;
    GLint Matrix[16];
} actorBone;

void initActorParsing(int objFileNo);
int loadExternalFiles(char * ovl_filename, char * obj_filename);
void setMipsWatchers();
struct actorSections getActorSections(unsigned char * Data, size_t Size, unsigned long VStart);
void processActor();
void drawBone(actorBone Bones[], int CurrentBone, int ParentBone);
void drawLink(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ, int detail, int frames);
void drawBones(unsigned int BoneOffset, unsigned int AnimationOffset, float Scale, short X, short Y, short Z, short RX, short RY, short RZ);
int scanAnimations(unsigned char bank);
int scanBones(unsigned char bank);
