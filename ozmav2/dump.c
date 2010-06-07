#include "globals.h"

void md_InitModelDumping(int SceneNo)
{
	if(!zOptions.DumpModel) return;

	char MtlFilename[256];
	char FilePath[256];

	sprintf(FilePath, "dump//%s", zGame.TitleText);
	oz_CreateFolder(FilePath);
	sprintf(FilePath, "dump//%s//scene_%i", zGame.TitleText, (unsigned int)SceneNo);
	oz_CreateFolder(FilePath);

	sprintf(FilePath, "dump//%s//scene_%i//model.obj", zGame.TitleText, (unsigned int)SceneNo);
	zProgram.FileWavefrontObj = fopen(FilePath, "w");

	sprintf(MtlFilename, "material.mtl");
	sprintf(FilePath, "dump//%s//scene_%i//%s", zGame.TitleText, (unsigned int)SceneNo, MtlFilename);
	zProgram.FileWavefrontMtl = fopen(FilePath, "w");

	if(!zProgram.FileWavefrontObj || !zProgram.FileWavefrontMtl) {
		dbgprintf(1, MSK_COLORTYPE_WARNING, "- Warning: Could not create/open files for model dumping!\n");
		return;
	}

	fprintf(zProgram.FileWavefrontObj, "mtllib %s\n", MtlFilename);

	zProgram.WavefrontObjVertCount = 1;
	zProgram.WavefrontObjMaterialCnt = 1;
}

void md_StopModelDumping()
{
	if(zProgram.FileWavefrontObj) fclose(zProgram.FileWavefrontObj);
	if(zProgram.FileWavefrontMtl) fclose(zProgram.FileWavefrontMtl);
}

void md_CreateMaterial(int TextureID, unsigned char * TextureData)
{
	if(!zProgram.FileWavefrontObj || !zProgram.FileWavefrontMtl) return;

	char TextureFilename[256];
	sprintf(TextureFilename, "texture_fmt0x%02X_0x%08X.png", zTexture[TextureID].Format, zTexture[TextureID].Offset);

	char TexturePath[256];
	sprintf(TexturePath, "dump//%s//scene_%i//%s", zGame.TitleText, (unsigned int)zOptions.SceneNo, TextureFilename);

	md_SavePNG(TextureData, zTexture[TextureID].RealWidth, zTexture[TextureID].RealHeight, TexturePath);

	if(TextureID == 0) {
		fprintf(zProgram.FileWavefrontMtl, "newmtl material_%d\n", zProgram.WavefrontObjMaterialCnt);
		fprintf(zProgram.FileWavefrontMtl, "Kd 0.0000 0.0000 0.0000\n");
		fprintf(zProgram.FileWavefrontMtl, "illum 1\n");
		fprintf(zProgram.FileWavefrontMtl, "map_Kd %s\n\n", TextureFilename);

		fprintf(zProgram.FileWavefrontObj, "usemtl material_%d\n", zProgram.WavefrontObjMaterialCnt);

		zProgram.WavefrontObjMaterialCnt++;
	}
}

void md_DumpTriangle(int Vtxs[])
{
	if(!zProgram.FileWavefrontObj || !zProgram.FileWavefrontMtl) return;

	if(zTexture[0].Width == 0 || zTexture[0].Height == 0) return;

	int i = 0;

	for(i = 0; i < 3; i++) {
		float TempS0 = _FIXED2FLOAT(zVertex[Vtxs[i]].S, 16) * (zTexture[0].ScaleS * zTexture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(zTexture[0].Width, 16);
		float TempT0 = _FIXED2FLOAT(zVertex[Vtxs[i]].T, 16) * (zTexture[0].ScaleT * zTexture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(zTexture[0].Height, 16);

		fprintf(zProgram.FileWavefrontObj, "v %4.8f %4.8f %4.8f\n", (float)zVertex[Vtxs[i]].X / 32.0f, (float)zVertex[Vtxs[i]].Y / 32.0f, (float)zVertex[Vtxs[i]].Z / 32.0f);
		fprintf(zProgram.FileWavefrontObj, "vt %4.8f %4.8f\n", TempS0, -TempT0);
		fprintf(zProgram.FileWavefrontObj, "vn %4.8f %4.8f %4.8f\n", (float)zVertex[Vtxs[i]].R, (float)zVertex[Vtxs[i]].G, (float)zVertex[Vtxs[i]].B);
		if(!(zGfx.GeometryMode & G_LIGHTING)) fprintf(zProgram.FileWavefrontObj, "vc %1.0f %1.0f %1.0f\n", (float)zVertex[Vtxs[i]].R, (float)zVertex[Vtxs[i]].G, (float)zVertex[Vtxs[i]].B);
	}

	fprintf(zProgram.FileWavefrontObj, "f %d/%d/%d %d/%d/%d %d/%d/%d\n\n",
		zProgram.WavefrontObjVertCount, zProgram.WavefrontObjVertCount, zProgram.WavefrontObjVertCount,
		zProgram.WavefrontObjVertCount + 1, zProgram.WavefrontObjVertCount + 1, zProgram.WavefrontObjVertCount + 1,
		zProgram.WavefrontObjVertCount + 2, zProgram.WavefrontObjVertCount + 2, zProgram.WavefrontObjVertCount + 2);

	zProgram.WavefrontObjVertCount += 3;
}

int md_SavePNG(unsigned char * Buffer, int Width, int Height, char * Filename)
{
	png_structp png_ptr;
	png_infop info_ptr;

	FILE * file;
	if((file = fopen(Filename, "wb")) == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not create PNG file '%s'!\n", Filename);
		return EXIT_FAILURE;
	}

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
