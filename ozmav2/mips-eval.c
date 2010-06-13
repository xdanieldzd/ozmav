/*
 mips-eval.c
 spinout/2010-06-13
*/

#include "mips-eval.h"

int mips_funcs_found_count = 0;

/* registers to hold values */
unsigned int regs[32] = {
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0
};

/* boolean values to see if a register has been set */
unsigned char regset[32] = {
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int * funcs_watching;
int funcs_watching_count = 0;

/* Very hackish way to get arguments for function calls. It'll work for now */
void mips_EvalWord(unsigned int * words, int pos)
{
	unsigned int word = words[pos];
	switch(word >> 26)
	{
	case MIPS_OP_JAL:
	{
		/* delay slot */
		mips_EvalWord( words, pos+1);

		/* report function */
		mips_ReportFunc( getTARGET(word) );

		/* clear registers
		  I know this does not respect the mips calling convention */
		int __count;
		for(__count=0;__count<32;__count++)
		{
		    regset[__count] = false;
		    regs[__count] = 0;
		}
		break;
	}
	case MIPS_OP_ADDIU:
		regs[getRT(word)] = regs[getRS(word)] + getIMM(word);
		regset[getRT(word)] = true;
		break;
	case MIPS_OP_LUI:
		regs[getRT(word)] = getIMM(word)<<16;
		regset[getRT(word)] = true;
		break;
	case MIPS_OP_ORI:
		regs[getRT(word)] = regs[getRS(word)] | getIMM(word);
		regset[getRT(word)] = true;
		break;
	}
}

void mips_EvalWords(unsigned int * words, int count)
{
	int pos = 0;
	for(; pos++; pos < count)
		mips_EvalWord(words, pos);
}

void mips_SetFuncWatch(unsigned int target)
{
	funcs_watching_count++;
	funcs_watching = (unsigned int*) realloc(funcs_watching, funcs_watching_count * sizeof(unsigned int));
	funcs_watching[funcs_watching_count-1] = target;
}

int mips_ReportFunc(unsigned int target)
{
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
	if(func_no < 0)
		return -1;

	/* yep - put it in the list */
	mips_funcs_found_count++;
	mips_funcs_found = (struct mips_args*) realloc(mips_funcs_found, mips_funcs_found_count * sizeof(struct mips_args));
	mips_funcs_found[mips_funcs_found_count-1].target = target;

	/* copy args. arguments in stack is for another day (or night, amirite?) */
	for(i=0;i<4;i++)
	{
		if(regset[i+MIPS_REG_A0])
			mips_funcs_found[mips_funcs_found_count-1].args[i] = regs[i+MIPS_REG_A0];
		else
		{
			mips_funcs_found[mips_funcs_found_count-1].argc = i;
			break;
		}
	}

	return func_no;
}

void mips_ResetResults()
{
	if(mips_funcs_found != NULL)
		free(mips_funcs_found);
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
void * mips_GetFuncArg(unsigned int target, int argc)
{
	int i, func_no = -1;
	unsigned int * ret = NULL;
	target &= 0x0FFFFFFF;
	for(i=0;i<mips_funcs_found_count;i++)
	{
		if((funcs_watching[i]&0x0FFFFFFF) == target)
		{
			func_no = i;
			break;
		}
	}

	/* was it found ? */
	if(func_no < 0)
		return NULL;

	/* does the value exist? */
	if(mips_funcs_found[func_no].argc < argc)
		return NULL;

	*ret = mips_funcs_found[func_no].args[argc];
	return ret;
}

