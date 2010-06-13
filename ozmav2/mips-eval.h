


#ifndef _MIPS_EVAL_H_
#define _MIPS_EVAL_H_
#include "r3400i.h"
#include <stdio.h>
#include <stdlib.h>

#define true	1
#define false	0

#define MIPS_OP_JAL	3
#define MIPS_OP_ADDIU	9
#define MIPS_OP_ORI	13
#define MIPS_OP_LUI	15
#define MIPS_OP_SW	43
#define MIPS_REG_SP	29
#define MIPS_REG_A0	4

struct mips_args {
	unsigned int target;
	int argc;
	unsigned int args[4];
};

struct mips_args * mips_funcs_found;
int mips_funcs_found_count;

/* function declarations */
extern void mips_EvalWord(unsigned int * words, int pos);
extern void mips_EvalWords(unsigned int * words, int count);
extern void mips_SetFuncWatch(unsigned int target);
extern int mips_ReportFunc(unsigned int target);
extern void mips_ResetResults();
extern void mips_ResetWatch();
extern void * mips_GetFuncArg(unsigned int target, int argc);


#endif /* _MIPS_EVAL_H_ */
