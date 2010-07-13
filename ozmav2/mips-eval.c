/*
 mips-eval.c
 spinout/2010-06-13
*/

#include "mips-eval.h"

#include "globals.h"

#define SAFETY_VAL	4

int mips_funcs_found_count = 0;

/* registers to hold values */
unsigned int regs[32] = {
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int * funcs_watching;
int funcs_watching_count = 0;

unsigned int stack[256*SAFETY_VAL];
signed int stack_pos = 128*SAFETY_VAL;
signed int imm;

/*
#define flip32(w)\
	(((w)&0xFF000000)>>24)|(((w)&0x00FF0000)>>8)|(((w)&0x0000FF00)<<8)|(((w)&0x000000FF)<<24)
*/
/* Very hackish way to get arguments for function calls. It'll work for now */
void mips_EvalWord(unsigned int * words, int pos)
{
	unsigned int word = flip32(words[pos]);
	switch(word >> 26)
	{
	case MIPS_OP_JAL:
	{
		/* delay slot */
		mips_EvalWord( words, pos+1);

		/* report function */
		mips_ReportFunc( getTARGET(word) );

		/* clear registers */
		regs[1] = 0;
		regs[2] = 0;
		regs[3] = 0;
		regs[4] = 0;
		regs[5] = 0;
		regs[6] = 0;
		regs[7] = 0;
		regs[8] = 0;
		regs[9] = 0;
		regs[10] = 0;
		regs[11] = 0;
		regs[12] = 0;
		regs[13] = 0;
		regs[14] = 0;
		regs[15] = 0;
		regs[24] = 0;
		regs[25] = 0;
		regs[26] = 0;
		regs[27] = 0;
		break;
	}
	case MIPS_OP_ADDIU:
		imm = getIMM(word);
		if(imm & 0x8000){
			imm |= 0xFFFF0000;
		}
		regs[getRT(word)] = regs[getRS(word)] + imm;
		if(getRT(word) == getRS(word) && getRT(word) == MIPS_REG_SP){
			stack_pos += (signed short)imm;
		}
		break;
	case MIPS_OP_LUI:
		regs[getRT(word)] = getIMM(word)<<16;
		break;
	case MIPS_OP_ORI:
		regs[getRT(word)] = regs[getRS(word)] | getIMM(word);
		break;
	case MIPS_OP_SW:
		if(getRS(word) == MIPS_REG_SP){
			stack[stack_pos+getIMM(word)] = regs[getRT(word)];

		}
		break;
	case MIPS_OP_LW:
		if(getRS(word) == MIPS_REG_SP){
			regs[getRT(word)] = stack[stack_pos+getIMM(word)];
		}
		break;
	case MIPS_OP_TYPE_R:
	{
		switch (word & 0x3F)
		{
		case MIPS_ROP_SLL:
			regs[getRD(word)] = regs[getRT(word)] << getSA(word);
			break;
		case MIPS_ROP_SRA:
			regs[getRD(word)] = regs[getRT(word)] >> getSA(word);
			break;
		}
	}
		break;
	}
}

void mips_EvalWords(unsigned int * words, int count)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "%s(%p, %i)", __FUNCTION__, words, count);
	int pos;
	stack_pos = 128 * SAFETY_VAL;
	for(pos=0; pos<count; pos++)
	{
		mips_EvalWord(words, pos);
		regs[0] = 0;
	}
}

void mips_SetFuncWatch(unsigned int target)
{
	funcs_watching_count++;
	funcs_watching = (unsigned int*) realloc(funcs_watching, funcs_watching_count * sizeof(unsigned int));
	funcs_watching[funcs_watching_count-1] = target;
}

int mips_ReportFunc(unsigned int target)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "%s(%08X);", __FUNCTION__, target);
	int i;
	int func_no = -1;
	target &= 0x0FFFFFFF;

	/* is the function being watched for? */
	for(i=0;i<funcs_watching_count;i++)
	{
		if((funcs_watching[i]&0x0FFFFFFF) == target)
		{
			func_no = i;
			break;
		}
	}

	/* was it found ? */
	if(func_no < 0){
		/* Wait a second. Maybe there's something useful.. */
		int seg;
		for(i=0;i<4;i++){
			seg = regs[i+MIPS_REG_A0] >> 24;
			if((seg == 6 || seg == 4) && !(regs[i+MIPS_REG_A0] & 0x00F00003) ){
				dbgprintf(0, MSK_COLORTYPE_WARNING, "Unwatched function %08X has suspicious argument #%i: %08X", target, i, regs[i+MIPS_REG_A0]);
			}
		}
		return -1;
	}
	/* yep - put it in the list */
	mips_funcs_found_count++;
	mips_funcs_found = (struct mips_args*) realloc(mips_funcs_found, mips_funcs_found_count * sizeof(struct mips_args));
	mips_funcs_found[mips_funcs_found_count-1].target = target;

	/* copy args. arguments in stack is for another day (or night, amirite?) */
	for(i=0;i<4;i++)
	{
		mips_funcs_found[mips_funcs_found_count-1].args[i] = regs[i+MIPS_REG_A0];
	}

	mips_funcs_found[mips_funcs_found_count-1].argc = 4;

	dbgprintf(2, MSK_COLORTYPE_OKAY, " - Recording %08X with args 0x%08X, 0x%08X, 0x%08X, 0x%08X", target,
		mips_funcs_found[mips_funcs_found_count-1].args[0],
		mips_funcs_found[mips_funcs_found_count-1].args[1],
		mips_funcs_found[mips_funcs_found_count-1].args[2],
		mips_funcs_found[mips_funcs_found_count-1].args[3]
		);

	return func_no;
}

void mips_ResetResults()
{
	//if(mips_funcs_found != NULL)
	//	free(mips_funcs_found);
	mips_funcs_found_count = 0;
}

void mips_ResetWatch()
{
	if(funcs_watching != NULL)
		free(funcs_watching);
	funcs_watching_count = 0;
}

/*
 returns a pointer because the potential value is anything
*/
void * mips_GetFuncArg(unsigned int target, int argc, int nonzero)
{
	if(argc > 3)
		return NULL;

	int i, func_no = -1;
	unsigned int * ret = NULL;
	target &= 0x0FFFFFFF;
	dbgprintf(2, MSK_COLORTYPE_INFO, "  - Requesting %08X #%i", target, argc);

	for(i=0;i<mips_funcs_found_count;i++)
	{
		if((mips_funcs_found[i].target&0x0FFFFFFF) == target)
		{
			// if we're specifically looking for a non-zero value, check for that
			if(nonzero && mips_funcs_found[i].args[argc] != 0) {
				func_no = i;
				break;
			}
		}
	}

	/* was it found ? */
	if(func_no < 0){
		dbgprintf(1, MSK_COLORTYPE_WARNING, "  - %08X not caught", target);
		return NULL;
	}

	ret = &mips_funcs_found[func_no].args[argc];
	dbgprintf(2, MSK_COLORTYPE_INFO, "  - Reporting %08X %i : 0x%08X", target, argc, *ret);
	return ret;
}
