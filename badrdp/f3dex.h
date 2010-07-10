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
