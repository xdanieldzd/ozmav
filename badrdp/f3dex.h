extern void RDP_F3DEX_Init();

// ----------------------------------------

extern void RDP_F3DEX_LOAD_UCODE();
extern void RDP_F3DEX_BRANCH_Z();
extern void RDP_F3DEX_TRI2();
extern void RDP_F3DEX_MODIFYVTX();

// ----------------------------------------

#define F3DEX_LOAD_UCODE		0xAF
#define F3DEX_BRANCH_Z			0xB0
#define F3DEX_TRI2				0xB1
#define F3DEX_MODIFYVTX			0xB2

// ----------------------------------------

#define F3DEX_TEXTURE_ENABLE	0x00000002
#define F3DEX_SHADING_SMOOTH	0x00000200
#define F3DEX_CULL_FRONT		0x00001000
#define F3DEX_CULL_BACK			0x00002000
#define F3DEX_CULL_BOTH			0x00003000
#define F3DEX_CLIPPING			0x00800000

#define F3DEX_MTX_STACKSIZE		18

#define F3DEX_MTX_MODELVIEW		0x00
#define F3DEX_MTX_PROJECTION	0x01
#define F3DEX_MTX_MUL			0x00
#define F3DEX_MTX_LOAD			0x02
#define F3DEX_MTX_NOPUSH		0x00
#define F3DEX_MTX_PUSH			0x04
