extern void md_InitModelDumping(int SceneNo);
extern void md_StopModelDumping();
extern void md_CreateMaterial(int TextureID, unsigned char * TextureData);
extern void md_DumpTriangle(int Vtxs[]);
extern int md_SavePNG(unsigned char * Buffer, int Width, int Height, char * Filename);
