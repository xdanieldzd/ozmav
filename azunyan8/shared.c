/*
	shared.c - shared functions
*/

#include "globals.h"

char * parseFormat(char * fmt, ...)
{
	if(tempstr == NULL) tempstr = malloc(BUFSIZ);

	va_list argp;

	if(fmt == NULL) return NULL;

	va_start(argp, fmt);
	vsprintf((char*)tempstr, fmt, argp);
	va_end(argp);

	return tempstr;
}

void getRoot(char * path, char * target)
{
	char temp[MAX_PATH];
	strcpy(temp, path);
	char * p;
	if((p = strchr(temp, FILESEP))) {
		p++;
		*p = '\0';
		strcpy(target, temp);
	} else {
		sprintf(target, ".%c", FILESEP);
	}
}

char * getFilename(char * path)
{
	if(tempstr == NULL) tempstr = malloc(BUFSIZ);

	char * p;
	if((p = strrchr(path, FILESEP))) {
		strcpy(tempstr, p+1);
	} else {
		strcpy(tempstr, path);
	}

	return tempstr;
}
