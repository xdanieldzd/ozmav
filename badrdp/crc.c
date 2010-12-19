#include "globals.h"

unsigned int CRCTable[256];

void crc_GenerateTable()
{
	unsigned int CRC, Poly;
	int	i, j;

	Poly = 0xEDB88320;
	for(i = 0; i < 256; i++) {
		CRC = i;
		for(j = 8; j > 0; j--) {
			if(CRC & 1) CRC = (CRC >> 1) ^ Poly;
			else CRC >>= 1;
		}
		CRCTable[i] = CRC;
	}
}

unsigned int crc_GenerateCRC(unsigned char * Data, int Len)
{
	unsigned int CRC = ~0;
	int i;
	for(i = 0; i < Len; i++) CRC = (CRC >> 8) ^ CRCTable[(CRC ^ Data[i]) & 0xFF];

	return ~CRC;
}
