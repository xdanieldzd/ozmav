extern void dl_F3DEX2_Init();

// ----------------------------------------

extern void dl_F3DEX2_VTX();
extern void dl_F3DEX2_MODIFYVTX();
extern void dl_F3DEX2_CULLDL();
extern void dl_F3DEX2_BRANCH_Z();
extern void dl_F3DEX2_TRI1();
extern void dl_F3DEX2_TRI2();
extern void dl_F3DEX2_QUAD();
extern void dl_F3DEX2_SPECIAL_3();
extern void dl_F3DEX2_SPECIAL_2();
extern void dl_F3DEX2_SPECIAL_1();
extern void dl_F3DEX2_DMA_IO();
extern void dl_F3DEX2_TEXTURE();
extern void dl_F3DEX2_POPMTX();
extern void dl_F3DEX2_GEOMETRYMODE();
extern void dl_F3DEX2_MTX();
extern void dl_F3DEX2_MOVEWORD();
extern void dl_F3DEX2_MOVEMEM();
extern void dl_F3DEX2_LOAD_UCODE();
extern void dl_F3DEX2_DL();
extern void dl_F3DEX2_ENDDL();
extern void dl_F3DEX2_SPNOOP();
extern void dl_F3DEX2_RDPHALF_1();
extern void dl_F3DEX2_SETOTHERMODE_L();
extern void dl_F3DEX2_SETOTHERMODE_H();
extern void dl_F3DEX2_RDPHALF_2();

// ----------------------------------------

#define	F3DEX2_VTX				0x01
#define	F3DEX2_MODIFYVTX		0x02
#define	F3DEX2_CULLDL			0x03
#define	F3DEX2_BRANCH_Z			0x04
#define	F3DEX2_TRI1				0x05
#define F3DEX2_TRI2				0x06
#define F3DEX2_QUAD				0x07

#define	F3DEX2_SPECIAL_3		0xD3
#define	F3DEX2_SPECIAL_2		0xD4
#define	F3DEX2_SPECIAL_1		0xD5
#define	F3DEX2_DMA_IO			0xD6
#define	F3DEX2_TEXTURE			0xD7
#define	F3DEX2_POPMTX			0xD8
#define F3DEX2_GEOMETRYMODE		0xD9
#define	F3DEX2_MTX				0xDA
#define	F3DEX2_MOVEWORD			0xDB
#define	F3DEX2_MOVEMEM			0xDC
#define	F3DEX2_LOAD_UCODE		0xDD
#define	F3DEX2_DL				0xDE
#define	F3DEX2_ENDDL			0xDF
#define	F3DEX2_SPNOOP			0xE0
#define	F3DEX2_RDPHALF_1		0xE1
#define	F3DEX2_SETOTHERMODE_L	0xE2
#define	F3DEX2_SETOTHERMODE_H	0xE3
#define	F3DEX2_RDPHALF_2		0xF1

// ----------------------------------------

#define F3DEX2_TEXTURE_ENABLE		0x00000000
#define F3DEX2_CULL_FRONT			0x00000200
#define F3DEX2_CULL_BACK			0x00000400
#define F3DEX2_CULL_BOTH			0x00000600
#define F3DEX2_SHADING_SMOOTH		0x00200000
#define F3DEX2_CLIPPING				0x00800000
