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

void getFilePath(char * FullPath, char * Target)
{
	char Temp[MAX_PATH];
	strcpy(Temp, FullPath);
	char * Ptr;
	if((Ptr = strrchr(Temp, FILESEP))) {
		Ptr++;
		*Ptr = '\0';
		strcpy(Target, Temp);
	} else {
		sprintf(Target, ".%c", FILESEP);
	}
}

void strtolower(char str[])
{
	while(*str != 0) {
		if(*str >= 'A' && *str <= 'Z') {
			*str = *str + 'a' - 'A';
		}
		str++;
	}
}
