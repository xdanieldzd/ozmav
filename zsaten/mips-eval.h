
#ifndef _MIPS_EVAL_H_
#define _MIPS_EVAL_H_
#include "r3400i.h"

#define MIPS_OP_JAL	3
#define MIPS_OP_ADDIU	9
#define MIPS_OP_ORI	13
#define MIPS_OP_LUI	15
#define MIPS_OP_SW	43
#define MIPS_OP_LW	35
#define MIPS_OP_TYPE_R	0

#define MIPS_ROP_SLL	0x0
#define MIPS_ROP_SRL	0x2
#define MIPS_ROP_SRA	0x3
#define MIPS_ROP_SLLV	0x4
#define MIPS_ROP_SRLV	0x6
#define MIPS_ROP_SRAV	0x7
#define MIPS_ROP_JR	0x8
#define MIPS_ROP_JALR	0x9
#define MIPS_ROP_SYSCALL	0xc
#define MIPS_ROP_BREAK	0xd
#define MIPS_ROP_SYNC	0xf
#define MIPS_ROP_MFHI	0x10
#define MIPS_ROP_MTHI	0x11
#define MIPS_ROP_MFLO	0x12
#define MIPS_ROP_MTLO	0x13
#define MIPS_ROP_DSLLV	0x14
#define MIPS_ROP_DSRLV	0x16
#define MIPS_ROP_DSRAV	0x17
#define MIPS_ROP_MULT	0x18
#define MIPS_ROP_MULTU	0x19
#define MIPS_ROP_DIV	0x1A
#define MIPS_ROP_DIVU	0x1B
#define MIPS_ROP_DMULT	0x1C
#define MIPS_ROP_DMULTU	0x1D
#define MIPS_ROP_DDIV	0x1E
#define MIPS_ROP_DDIVU	0x1F
#define MIPS_ROP_ADD	0x20
#define MIPS_ROP_ADDU	0x21
#define MIPS_ROP_SUB	0x22
#define MIPS_ROP_SUBU	0x23
#define MIPS_ROP_AND	0x24
#define MIPS_ROP_OR	0x25
#define MIPS_ROP_XOR	0x26
#define MIPS_ROP_NOR	0x27
#define MIPS_ROP_SLT	0x2A
#define MIPS_ROP_SLTU	0x2B
#define MIPS_ROP_DADD	0x2C
#define MIPS_ROP_DADDU	0x2D
#define MIPS_ROP_DSUB	0x2E
#define MIPS_ROP_DSUBU	0x2F
#define MIPS_ROP_TGE	0x30
#define MIPS_ROP_TGEU	0x31
#define MIPS_ROP_TLT	0x32
#define MIPS_ROP_TLTU	0x33
#define MIPS_ROP_TEQ	0x34
#define MIPS_ROP_TNE	0x36
#define MIPS_ROP_DSLL	0x38
#define MIPS_ROP_DSRL	0x3A
#define MIPS_ROP_DSRA	0x3B
#define MIPS_ROP_DSLL32	0x3C
#define MIPS_ROP_DSRL32	0x3E
#define MIPS_ROP_DSRA32	0x3F

#define MIPS_REG_SP	29
#define MIPS_REG_A0	4
#define MIPS_REG_RA	31



struct mips_args {
	unsigned int target;
	int argc;
	unsigned int args[4];
};

extern struct mips_args * mips_funcs_found;
extern int mips_funcs_found_count;

/* function declarations */
extern void mips_EvalWord(unsigned int * words, int pos);
extern void mips_EvalWords(unsigned int * words, int count);
extern void mips_SetFuncWatch(unsigned int target);
extern int mips_ReportFunc(unsigned int target);
extern void mips_ResetResults();
extern void mips_ResetWatch();
extern void * mips_GetFuncArg(unsigned int target, int argc, int nonzero);
extern void mips_SetSpecialOp(unsigned int Op, unsigned int mask, int ValueToSet);
extern void mips_ResetSpecialOps();
extern void mips_SetMap(unsigned char * Data, int Size, unsigned int Address);
extern void mips_ResetMap();


#endif /* _MIPS_EVAL_H_ */
