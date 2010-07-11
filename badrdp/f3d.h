extern void RDP_F3D_Init();

// ----------------------------------------

extern void RDP_F3D_SPNOOP();
extern void RDP_F3D_MTX();
extern void RDP_F3D_RESERVED0();
extern void RDP_F3D_MOVEMEM();
extern void RDP_F3D_VTX();
extern void RDP_F3D_RESERVED1();
extern void RDP_F3D_DL();
extern void RDP_F3D_RESERVED2();
extern void RDP_F3D_RESERVED3();
extern void RDP_F3D_SPRITE2D_BASE();
extern void RDP_F3D_TRI4();
extern void RDP_F3D_RDPHALF_CONT();
extern void RDP_F3D_RDPHALF_2();
extern void RDP_F3D_RDPHALF_1();
extern void RDP_F3D_QUAD();
extern void RDP_F3D_CLEARGEOMETRYMODE();
extern void RDP_F3D_SETGEOMETRYMODE();
extern void RDP_F3D_ENDDL();
extern void RDP_F3D_SETOTHERMODE_L();
extern void RDP_F3D_SETOTHERMODE_H();
extern void RDP_F3D_TEXTURE();
extern void RDP_F3D_MOVEWORD();
extern void RDP_F3D_POPMTX();
extern void RDP_F3D_CULLDL();
extern void RDP_F3D_TRI1();

// ----------------------------------------

#define F3D_SPNOOP				0x00
#define F3D_MTX					0x01
#define F3D_RESERVED0			0x02
#define F3D_MOVEMEM				0x03
#define F3D_VTX					0x04
#define F3D_RESERVED1			0x05
#define F3D_DL					0x06
#define F3D_RESERVED2			0x07
#define F3D_RESERVED3			0x08
#define F3D_SPRITE2D_BASE		0x09

#define F3D_TRI4				0xB1
#define F3D_RDPHALF_CONT		0xB2
#define F3D_RDPHALF_2			0xB3
#define F3D_RDPHALF_1			0xB4
#define F3D_QUAD				0xB5
#define F3D_CLEARGEOMETRYMODE	0xB6
#define F3D_SETGEOMETRYMODE		0xB7
#define F3D_ENDDL				0xB8
#define F3D_SETOTHERMODE_L		0xB9
#define F3D_SETOTHERMODE_H		0xBA
#define F3D_TEXTURE				0xBB
#define F3D_MOVEWORD			0xBC
#define F3D_POPMTX				0xBD
#define F3D_CULLDL				0xBE
#define F3D_TRI1				0xBF

// ----------------------------------------

#define F3D_TEXTURE_ENABLE		0x00000002
#define F3D_SHADING_SMOOTH		0x00000200
#define F3D_CULL_FRONT			0x00001000
#define F3D_CULL_BACK			0x00002000
#define F3D_CULL_BOTH			0x00003000
#define F3D_CLIPPING			0x00000000

#define F3D_MTX_STACKSIZE		10

#define F3D_MTX_MODELVIEW		0x00
#define F3D_MTX_PROJECTION		0x01
#define F3D_MTX_MUL				0x00
#define F3D_MTX_LOAD			0x02
#define F3D_MTX_NOPUSH			0x00
#define F3D_MTX_PUSH			0x04
