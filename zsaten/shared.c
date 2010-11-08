#include "globals.h"

char * parseFormat(char * fmt, ...)
{
	if(vProgram.tempString == NULL) vProgram.tempString = malloc(BUFSIZ);

	va_list argp;

	if(fmt == NULL) return NULL;

	va_start(argp, fmt);
	vsprintf((char*)vProgram.tempString, fmt, argp);
	va_end(argp);

	return vProgram.tempString;
}

char * getFilename(char * path)
{
	if(vProgram.tempString == NULL) vProgram.tempString = malloc(BUFSIZ);

	char * p;
	if((p = strrchr(path, FILESEP))) {
		strcpy(vProgram.tempString, p+1);
	} else {
		strcpy(vProgram.tempString, path);
	}

	return vProgram.tempString;
}
