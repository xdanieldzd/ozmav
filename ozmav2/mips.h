/***********************************
* MIPS General Purpose Header File *
***********************************/
#ifndef __MIPS_H__
#define __MIPS_H__

#include <stdint.h>
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

/**
|**
*\* MIPS64 assembly in macros 
**|
**/

/**
 * If you don't want the instruction names to have a prefix infront of them,
 * compile with the flag -D__MIPS_NO_PREFIX and there will be register names
 * and instruction names compiled without the MIPS_ prefix.
**/

/* Quick... */
#define MIPS_OP(x)   ((x)<<26)
#define MIPS_OF(x)   (((uint32_t)(x)>>2)&0xFFFF)
#define MIPS_SA(x)   (((x)&0x1F)<<6)
#define MIPS_RD(x)   (((x)&0x1F)<<11)
#define MIPS_RT(x)   (((x)&0x1F)<<16)
#define MIPS_RS(x)   (((x)&0x1F)<<21)
#define MIPS_IM(x)   ((uint32_t)(x)&0xFFFF)
#define MIPS_JT(x)   (((uint32_t)(x)>>2)&0x3FFFFFF)

#define mips_r0	0
#define mips_v0	2
#define mips_v1	3
#define mips_a0	4
#define mips_a1	5
#define mips_a2	6
#define mips_a3	7
#define mips_t0	8
#define mips_t1	9
#define mips_t2	10
#define mips_t3	11
#define mips_t4	12
#define mips_t5	13
#define mips_t6	14
#define mips_t7	15
#define mips_s0	16
#define mips_s1	17
#define mips_s2	18
#define mips_s3	19
#define mips_s4	20
#define mips_s5	21
#define mips_s6	22
#define mips_s7	23
#define mips_t8	24
#define mips_t9	25
#define mips_k0	26
#define mips_k1	27
#define mips_gp	28
#define mips_sp	29
#define mips_fp	30
#define mips_ra	31

/* MIPS assembly */
#define MIPS_ADD(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x20)
#define MIPS_ADDU(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x21)
#define MIPS_ADDI(rt, rs, immd)     (MIPS_OP(0x08) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_ADDIU(rt, rs, immd)    (MIPS_OP(0x09) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_ANDI(rt, rs, immd)     (MIPS_OP(0x0C) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_BC1F(off)              (MIPS_OP(0x11) | MIPS_RS(0x08) | MIPS_OF(off))
#define MIPS_BC1FL(off)             (MIPS_OP(0x11) | MIPS_RS(0x08) | MIPS_RT(0x02) | MIPS_OF(off))
#define MIPS_BC1T(off)              (MIPS_OP(0x11) | MIPS_RS(0x08) | MIPS_RT(0x01) | MIPS_OF(off))
#define MIPS_BC1TL(off)             (MIPS_OP(0x11) | MIPS_RS(0x08) | MIPS_RT(0x03) | MIPS_OF(off))
#define MIPS_BEQ(rs, rt, off)       (MIPS_OP(0x04) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_OF(off))
#define MIPS_BEQL(rs, rt, off)      (MIPS_OP(0x14) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_OF(off))
#define MIPS_BGEZ(rs, off)          (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x01) | MIPS_OF(off))
#define MIPS_BGEZAL(rs, off)        (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x11) | MIPS_OF(off))
#define MIPS_BGEZALL(rs, off)       (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x13) | MIPS_OF(off))
#define MIPS_BGEZL(rs, off)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x03) | MIPS_OF(off))
#define MIPS_BGTZ(rs, off)          (MIPS_OP(0x07) | MIPS_RS(rs) | MIPS_OF(off))
#define MIPS_BGTZL(rs, off)         (MIPS_OP(0x17) | MIPS_RS(rs) | MIPS_OF(off))
#define MIPS_BLEZ(rs, off)          (MIPS_OP(0x06) | MIPS_RS(rs) | MIPS_OF(off))
#define MIPS_BLEZL(rs, off)         (MIPS_OP(0x16) | MIPS_RS(rs) | MIPS_OF(off))
#define MIPS_BLTZ(rs, off)          (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_OF(off))
#define MIPS_BLTZAL(rs, off)        (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x10) | MIPS_OF(off))
#define MIPS_BLTZALL(rs, off)       (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x12) | MIPS_OF(off))
#define MIPS_BLTZL(rs, off)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x02) | MIPS_OF(off))
#define MIPS_BNE(rs, rt, off)       (MIPS_OP(0x05) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_OF(off))
#define MIPS_BNEL(rs, rt, off)      (MIPS_OP(0x15) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_OF(off))
#define MIPS_BREAK(code)            ((code) << 6 | 0x0D)
#define MIPS_CACHE(base, op, off)   (MIPS_OP(0x2F) | MIPS_RS(base) | MIPS_RT(op) | MIPS_OF(off))
#define MIPS_CFC1(rt, rd)           (MIPS_OP(0x11) | MIPS_RS(0x02) | MIPS_RT(base) | MIPS_RD(rd))
#define MIPS_COP1(cofun)            (MIPS_OP(0x11) | (1 << 25) | ((cofun) & 0x1FFFFFF))
#define MIPS_CTC1(rt, rd)           (MIPS_OP(0x11) | MIPS_RS(0x06) | MIPS_RT(base) | MIPS_RD(rd))
#define MIPS_DADD(rd, rs, rt)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2C)
#define MIPS_DADDI(rt, rs, immd)    (MIPS_OP(0x18) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_DADDIU(rt, rs, immd)   (MIPS_OP(0x19) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_DADDU(rd, rs, rt)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2D)
#define MIPS_DDIV(rs, rt)           (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1E)
#define MIPS_DDIVU(rs, rt)          (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1F)
#define MIPS_DIV(rs, rt)            (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1A)
#define MIPS_DIVU(rs, rt)           (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1B)
#define MIPS_DMFC0(rt, rd)          (MIPS_OP(0x10) | MIPS_RS(0x01) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_DMTC0(rt, rd)          (MIPS_OP(0x10) | MIPS_RS(0x05) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_DMULT(rs, rt)          (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1C)
#define MIPS_DMULTU(rs, rt)         (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x1D)
#define MIPS_DSLL(rd, rt, sa)       (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x38)
#define MIPS_DSLLV(rd, rt, rs)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x14)
#define MIPS_DSLL32(rd, rt, sa)     (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x3C)
#define MIPS_DSRA(rd, rt, sa)       (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x3B)
#define MIPS_DSRAV(rd, rt, rs)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x17)
#define MIPS_DSRA32(rd, rt, sa)     (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x3F)
#define MIPS_DSRL(rd, rt, sa)       (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x3A)
#define MIPS_DSRLV(rd, rt, rs)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x16)
#define MIPS_DSRL32(rd, rt, sa)     (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x3E)
#define MIPS_DSUB(rd, rs, rt)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2E)
#define MIPS_DSUBU(rd, rs, rt)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2F)
#define MIPS_ERET()                 (MIPS_OP(0x10) | 1 << 25 | 0x18)
#define MIPS_J(target)              (MIPS_OP(0x02) | MIPS_JT(target))
#define MIPS_JAL(target)            (MIPS_OP(0x03) | MIPS_JT(target))
#define MIPS_JALR(rd, rs)           (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RD(rd) | 0x09)
#define MIPS_JR(rs)                 (MIPS_OP(0x00) | MIPS_RS(rs) | 0x08)
#define MIPS_LB(rt, off, base)      (MIPS_OP(0x20) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LBU(rt, off, base)     (MIPS_OP(0x24) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LD(rt, off, base)      (MIPS_OP(0x37) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LDC1(rt, off, base)    (MIPS_OP(0x35) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LDL(rt, off, base)     (MIPS_OP(0x1A) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LDR(rt, off, base)     (MIPS_OP(0x1B) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LH(rt, off, base)      (MIPS_OP(0x21) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LHU(rt, off, base)     (MIPS_OP(0x25) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LL(rt, off, base)      (MIPS_OP(0x30) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LLD(rt, off, base)     (MIPS_OP(0x34) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LUI(rt, immd)          (MIPS_OP(0x0F) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_LW(rt, off, base)      (MIPS_OP(0x23) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LWC1(rt, off, base)    (MIPS_OP(0x31) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LWL(rt, off, base)     (MIPS_OP(0x22) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LWR(rt, off, base)     (MIPS_OP(0x26) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_LWU(rt, off, base)     (MIPS_OP(0x27) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_MFC0(rt, rd)           (MIPS_OP(0x10) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_MFC1(rt, rd)           (MIPS_OP(0x11) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_MFHI(rd)               (MIPS_OP(0x00) | MIPS_RD(rd) | 0x10)
#define MIPS_MFLO(rd)               (MIPS_OP(0x00) | MIPS_RD(rd) | 0x12)
#define MIPS_MTC0(rt, rd)           (MIPS_OP(0x10) | MIPS_RS(0x04) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_MTC1(rt, rd)           (MIPS_OP(0x11) | MIPS_RS(0x04) | MIPS_RT(rt) | MIPS_RD(rd))
#define MIPS_MTHI(rd)               (MIPS_OP(0x00) | MIPS_RD(rd) | 0x11)
#define MIPS_MTLO(rd)               (MIPS_OP(0x00) | MIPS_RD(rd) | 0x13)
#define MIPS_MULT(rs, rt)           (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x18)
#define MIPS_MULTU(rs, rt)          (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | 0x19)
#define MIPS_NOR(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x27)
#define MIPS_OR(rd, rs, rt)         (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x25)
#define MIPS_ORI(rt, rs, immd)      (MIPS_OP(0x0D) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_SB(rt, off, base)      (MIPS_OP(0x28) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SC(rt, off, base)      (MIPS_OP(0x38) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SCD(rt, off, base)     (MIPS_OP(0x3C) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SD(rt, off, base)      (MIPS_OP(0x3F) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SDC1(rt, off, base)    (MIPS_OP(0x3D) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SDL(rt, off, base)     (MIPS_OP(0x2C) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SDR(rt, off, base)     (MIPS_OP(0x2D) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SH(rt, off, base)      (MIPS_OP(0x29) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SLL(rd, rt, sa)        (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa))
#define MIPS_SLLV(rd, rt, rs)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x04)
#define MIPS_SLT(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2A)
#define MIPS_SLTI(rt, rs, immd)     (MIPS_OP(0x0A) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_SLTIU(rt, rs, immd)    (MIPS_OP(0x0B) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))
#define MIPS_SLTU(rd, rs, rt)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x2B)
#define MIPS_SRA(rd, rt, sa)        (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x03)
#define MIPS_SRAV(rd, rt, rs)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x07)
#define MIPS_SRL(rd, rt, sa)        (MIPS_OP(0x00) | MIPS_RT(rt) | MIPS_RD(rd) | MIPS_SA(sa) | 0x02)
#define MIPS_SRLV(rd, rt, rs)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x06)
#define MIPS_SUB(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x22)
#define MIPS_SUBU(rd, rs, rt)       (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x23)
#define MIPS_SW(rt, off, base)      (MIPS_OP(0x2B) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SWC1(rt, off, base)    (MIPS_OP(0x39) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SWL(rt, off, base)     (MIPS_OP(0x2A) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SWR(rt, off, base)     (MIPS_OP(0x2E) | MIPS_RS(base) | MIPS_RT(rt) | MIPS_IM(off))
#define MIPS_SYNC()                 (0x0F)
#define MIPS_SYSCALL(code)          ((code) << 6 | 0x0C)
#define MIPS_TEQ(rs, rt, code)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define MIPS_TEQI(rs, immd)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x0C) | MIPS_IM(immd))
#define MIPS_TGE(rs, rt, code)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define MIPS_TGEI(rs, immd)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x08) | MIPS_IM(immd))
#define MIPS_TGEIU(rs, immd)        (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x09) | MIPS_IM(immd))
#define MIPS_TGEU(rs, rt, code)     (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x31)
#define MIPS_TLBP()                 (MIPS_OP(0x10) | 1 << 25 | 0x08)
#define MIPS_TLBR()                 (MIPS_OP(0x10) | 1 << 25 | 0x01)
#define MIPS_TLBWI()                (MIPS_OP(0x10) | 1 << 25 | 0x02)
#define MIPS_TLBWR()                (MIPS_OP(0x10) | 1 << 25 | 0x06)
#define MIPS_TLT(rs, rt, code)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x32)
#define MIPS_TLTI(rs, immd)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x0A) | MIPS_IM(immd))
#define MIPS_TLTIU(rs, immd)        (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x0B) | MIPS_IM(immd))
#define MIPS_TLTU(rs, rt, code)     (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x33)
#define MIPS_TNE(rs, rt, code)      (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | ((code)&0x3FF) << 6 | 0x36)
#define MIPS_TNEI(rs, immd)         (MIPS_OP(0x01) | MIPS_RS(rs) | MIPS_RT(0x0E) | MIPS_IM(immd))
#define MIPS_XOR(rd, rs, rt)        (MIPS_OP(0x00) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_RD(rd) | 0x26)
#define MIPS_XORI(rt, rs, immd)     (MIPS_OP(0x0E) | MIPS_RS(rs) | MIPS_RT(rt) | MIPS_IM(immd))

#endif /* __MIPS_H__ */
