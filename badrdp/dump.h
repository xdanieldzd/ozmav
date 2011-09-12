extern int RDP_Dump_CreateMaterial(unsigned char * TextureData, unsigned char TexFormat, unsigned int TexOffset, int Width, int Height, bool SMirror, bool TMirror);
extern void RDP_Dump_SelectMaterial(int MatID);
extern void RDP_Dump_BeginGroup(unsigned int Address);
extern void RDP_Dump_DumpTriangle(__Vertex Vtx[], int VtxID[]);
extern int RDP_Dump_SavePNG(unsigned char * Buffer, int Width, int Height, char * Filename, bool SMirror, bool TMirror);
