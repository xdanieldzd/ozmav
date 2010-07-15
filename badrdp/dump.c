#include "globals.h"

void RDP_Dump_InitModelDumping(char Path[MAX_PATH], char ObjFilename[MAX_PATH], char MtlFilename[MAX_PATH])
{
	strcpy(System.WavefrontObjPath, Path);

	char TempPath[MAX_PATH];

	strcpy(TempPath, Path);
	strcat(TempPath, "/");
	strcat(TempPath, ObjFilename);
	System.FileWavefrontObj = NULL;
	System.FileWavefrontObj = fopen(TempPath, "w");

	strcpy(TempPath, Path);
	strcat(TempPath, "/");
	strcat(TempPath, MtlFilename);
	System.FileWavefrontMtl = NULL;
	System.FileWavefrontMtl = fopen(TempPath, "w");

	if(!System.FileWavefrontObj || !System.FileWavefrontMtl) return;

	fprintf(System.FileWavefrontObj, "# dumped via libbadRDP\n");
	fprintf(System.FileWavefrontObj, "mtllib %s\n", MtlFilename);

	fprintf(System.FileWavefrontMtl, "# dumped via libbadRDP\n");

	System.WavefrontObjVertCount = 1;
	System.WavefrontObjMaterialCnt = 1;
}

void RDP_Dump_StopModelDumping()
{
	if(System.FileWavefrontObj) fclose(System.FileWavefrontObj);
	if(System.FileWavefrontMtl) fclose(System.FileWavefrontMtl);
}

void RDP_Dump_CreateMaterial(unsigned char * TextureData, unsigned char TexFormat, unsigned int TexOffset, int Width, int Height)
{
	if(!System.FileWavefrontObj || !System.FileWavefrontMtl) return;

	char TextureFilename[MAX_PATH];
	sprintf(TextureFilename, "texture_fmt0x%02X_0x%08X.png", TexFormat, TexOffset);

	char TexturePath[MAX_PATH];
	sprintf(TexturePath, "%s//%s", System.WavefrontObjPath, TextureFilename);

	RDP_Dump_SavePNG(TextureData, Width, Height, TexturePath);

	fprintf(System.FileWavefrontMtl, "newmtl material_%d\n", System.WavefrontObjMaterialCnt);
	fprintf(System.FileWavefrontMtl, "Kd 0.0000 0.0000 0.0000\n");
	fprintf(System.FileWavefrontMtl, "illum 1\n");
	fprintf(System.FileWavefrontMtl, "map_Ka %s\n", TextureFilename);
	fprintf(System.FileWavefrontMtl, "map_Kd %s\n\n", TextureFilename);

	fprintf(System.FileWavefrontObj, "usemtl material_%d\n", System.WavefrontObjMaterialCnt);

	System.WavefrontObjMaterialCnt++;
}

void RDP_Dump_DumpTriangle(__Vertex Vtx[])
{
	if(!System.FileWavefrontObj || !System.FileWavefrontMtl) return;

	int i = 0;
	for(i = 0; i < 3; i++) {
		fprintf(System.FileWavefrontObj, "v %4.8f %4.8f %4.8f\n", (float)Vtx[i].X / 32.0f, (float)Vtx[i].Y / 32.0f, (float)Vtx[i].Z / 32.0f);
//		fprintf(System.FileWavefrontObj, "vt %4.8f %4.8f\n", Vtx[i].RealS0, -Vtx[i].RealT0);
		fprintf(System.FileWavefrontObj, "vn %4.8f %4.8f %4.8f\n", (float)Vtx[i].R, (float)Vtx[i].G, (float)Vtx[i].B);
	}

	fprintf(System.FileWavefrontObj, "f %d/%d/%d %d/%d/%d %d/%d/%d\n\n",
		System.WavefrontObjVertCount, System.WavefrontObjVertCount, System.WavefrontObjVertCount,
		System.WavefrontObjVertCount + 1, System.WavefrontObjVertCount + 1, System.WavefrontObjVertCount + 1,
		System.WavefrontObjVertCount + 2, System.WavefrontObjVertCount + 2, System.WavefrontObjVertCount + 2);

	System.WavefrontObjVertCount += 3;
}

int RDP_Dump_SavePNG(unsigned char * Buffer, int Width, int Height, char * Filename)
{
	png_structp png_ptr;
	png_infop info_ptr;

	FILE * file;
	if((file = fopen(Filename, "wb")) == NULL) return EXIT_FAILURE;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, file);

	png_set_IHDR(png_ptr, info_ptr, Width, Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_packing(png_ptr);

	png_write_info(png_ptr, info_ptr);

	png_byte ** row_pointers = NULL;
	row_pointers = (png_byte**) malloc(Height * sizeof(png_byte*));

	int y;
	for(y = 0; y < Height; y++)
		row_pointers[y] = Buffer+(y*Width*4);

	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);

	free(row_pointers);
	row_pointers = NULL;

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(file);

	return EXIT_SUCCESS;
}
