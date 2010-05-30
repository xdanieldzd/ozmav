extern unsigned int crc_table[256];

void gen_table();
unsigned int crc32(unsigned char *data, int len);
int N64GetCIC(unsigned char *data);
int N64CalcCRC(unsigned int *crc, unsigned char *data);
int FixChecksum(unsigned char * ROMBuffer);
