#define MAX_SEGMENTS		16

struct __RAM {
	bool IsSet;
	unsigned int Size;
	unsigned char * Data;
};

extern struct __RAM RAM[MAX_SEGMENTS];

extern enum { F3D, F3DEX, F3DEX2 } UcodeIDs;

extern void RDP_SetupOpenGL();
extern void RDP_InitParser(int UcodeID);
extern void RDP_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size);
extern void RDP_Yaz0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize);
extern void RDP_MIO0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize);
extern bool RDP_CheckAddressValidity(unsigned int Address);
extern void RDP_ClearSegment(unsigned char Segment);
extern void RDP_ClearTextures();
extern void RDP_ClearStructures(bool Full);
extern void RDP_ParseDisplayList(unsigned int Address, bool ResetStack);
extern void RDP_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1);
