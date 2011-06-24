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

	fprintf(System.FileWavefrontObj, "# dumped via libbadRDP\n\n");
	fprintf(System.FileWavefrontObj, "mtllib %s\n\n", MtlFilename);

	fprintf(System.FileWavefrontMtl, "# dumped via libbadRDP\n\n");

	System.WavefrontObjVertCount = 1;
	System.WavefrontObjMaterialCnt = 1;

	System.ObjDumpingEnabled = true;
}

void RDP_Dump_StopModelDumping()
{
	if(System.FileWavefrontObj) fclose(System.FileWavefrontObj);
	if(System.FileWavefrontMtl) fclose(System.FileWavefrontMtl);

	System.ObjDumpingEnabled = false;
}

int RDP_Dump_CreateMaterial(unsigned char * TextureData, unsigned char TexFormat, unsigned int TexOffset, int Width, int Height, bool SMirror, bool TMirror)
{
	if(!System.FileWavefrontObj || !System.FileWavefrontMtl || !System.ObjDumpingEnabled) return EXIT_FAILURE;

	char TextureFilename[MAX_PATH];
	sprintf(TextureFilename, "texture_0x%08X_fmt0x%02X.png", TexOffset, TexFormat);

	char TexturePath[MAX_PATH];
	sprintf(TexturePath, "%s//%s", System.WavefrontObjPath, TextureFilename);

	RDP_Dump_SavePNG(TextureData, Width, Height, TexturePath, SMirror, TMirror);

	fprintf(System.FileWavefrontMtl, "newmtl material_%d\n", System.WavefrontObjMaterialCnt);
	fprintf(System.FileWavefrontMtl, "Ka 0.2000 0.2000 0.2000\n");
	fprintf(System.FileWavefrontMtl, "Kd 0.8000 0.8000 0.8000\n");
	fprintf(System.FileWavefrontMtl, "illum 1\n");
	fprintf(System.FileWavefrontMtl, "map_Kd %s\n", TextureFilename); /* Explanation here 
	http://blog.lexique-du-net.com/index.php?post/2009/07/24/AmbientDiffuseEmissive-and-specular-colorSome-examples */
	fprintf(System.FileWavefrontMtl, "%s%s%s\n", (SMirror ? "#horz. mirror" : ""), (TMirror ? "/vert. mirror" : ""), (SMirror || TMirror ? "\n" : ""));

	System.WavefrontObjMaterialCnt++;

	return System.WavefrontObjMaterialCnt - 1;
}

void RDP_Dump_SelectMaterial(int MatID)
{
	if(!System.FileWavefrontObj || !System.FileWavefrontMtl || !System.ObjDumpingEnabled) return;

	fprintf(System.FileWavefrontObj, "usemtl material_%d\n", MatID);
}

void RDP_Dump_DumpTriangle(__Vertex Vtx[], int VtxID[])
{
	if(!System.FileWavefrontObj || !System.FileWavefrontMtl || !System.ObjDumpingEnabled) return;

	float vtS, vtT;

	int i = 0;
	for(i = 0; i < 3; i++) {
		vtS = Vtx[VtxID[i]].RealS0; vtT = -Vtx[VtxID[i]].RealT0;
		if(isnan(vtS)) { vtS = 0.0f; } if(isnan(vtT)) { vtT = 0.0f; }
		if(Texture[0].CMS == G_TX_MIRROR) { vtS = vtS/2; }
		if(Texture[0].CMT == G_TX_MIRROR) { vtT = vtT/2; }

		fprintf(System.FileWavefrontObj, "v %4.8f %4.8f %4.8f\n", (int)Vtx[VtxID[i]].Vtx.X / 32.0f, (int)Vtx[VtxID[i]].Vtx.Y / 32.0f, (int)Vtx[VtxID[i]].Vtx.Z / 32.0f);
		fprintf(System.FileWavefrontObj, "vt %4.8f %4.8f\n", vtS, vtT);
		fprintf(System.FileWavefrontObj, "vn %4.8f %4.8f %4.8f\n", (float)Vtx[VtxID[i]].Vtx.R, (float)Vtx[VtxID[i]].Vtx.G, (float)Vtx[VtxID[i]].Vtx.B);
	}

	fprintf(System.FileWavefrontObj, "f %d/%d/%d %d/%d/%d %d/%d/%d\n\n",
		System.WavefrontObjVertCount, System.WavefrontObjVertCount, System.WavefrontObjVertCount,
		System.WavefrontObjVertCount + 1, System.WavefrontObjVertCount + 1, System.WavefrontObjVertCount + 1,
		System.WavefrontObjVertCount + 2, System.WavefrontObjVertCount + 2, System.WavefrontObjVertCount + 2);

	System.WavefrontObjVertCount += 3;
}

int RDP_Dump_SavePNG(unsigned char * Buffer, int Width, int Height, char * Filename, bool SMirror, bool TMirror)
{
	int x, y, x2, y2;
	unsigned char * mirrored = NULL;

	png_structp png_ptr;
	png_infop info_ptr;

	// horz/vert mirror
	if(SMirror && TMirror) {
		Width *= 2;
		Height *= 2;
		mirrored = (unsigned char*)malloc(Height*Width*4);
		memset(mirrored, 0xFF, Height*Width*4);
		for(y = 0; y < Height/2; y++) {
			memcpy(&mirrored[y*Width*4], &Buffer[y*Width/2*4], Width*4);
			x2 = (Width/2)-1;
			for(x = 0; x < Width/2; x++) {
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)]   = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+1] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+1];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+2] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+2];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+3] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+3];
				x2--;
			}
		}
		y2 = Height/2;
		for(y = Height/2; y < Height; y++) {
			memcpy(&mirrored[y*Width*4], &Buffer[(--y2)*Width/2*4], Width*4);
			x2 = (Width/2)-1;
			for(x = 0; x < Width/2; x++) {
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)]   = Buffer[((y2-1)*Width/2*4)+(Width/2*4)+(x2*4)];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+1] = Buffer[((y2-1)*Width/2*4)+(Width/2*4)+(x2*4)+1];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+2] = Buffer[((y2-1)*Width/2*4)+(Width/2*4)+(x2*4)+2];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+3] = Buffer[((y2-1)*Width/2*4)+(Width/2*4)+(x2*4)+3];
				x2--;
			}
		}

	// horz mirror
	} else if(SMirror) {
		Width *= 2;
		mirrored = (unsigned char*)malloc(Height*Width*4);
		memset(mirrored, 0xFF, Height*Width*4);
		for(y = 0; y < Height; y++) {
			memcpy(&mirrored[y*Width*4], &Buffer[y*Width/2*4], Width/2*4);
			x2 = (Width/2)-1;
			for(x = 0; x < Width/2; x++) {
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)]   = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+1] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+1];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+2] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+2];
				mirrored[(y*Width*4)+(Width/2*4)+(x*4)+3] = Buffer[((y-1)*Width/2*4)+(Width/2*4)+(x2*4)+3];
				x2--;
			}
		}

	// vert mirror
	} else if(TMirror) {
		Height *= 2;
		mirrored = (unsigned char*)malloc(Height*Width*4);
		memset(mirrored, 0xFF, Height*Width*4);
		for(y = 0; y < Height/2; y++) {
			memcpy(&mirrored[y*Width*4], &Buffer[y*Width*4], Width*4);
		}
		y2 = Height/2;
		for(y = Height/2; y < Height; y++) {
			memcpy(&mirrored[y*Width*4], &Buffer[(--y2)*Width*4], Width*4);
		}
	}

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

	// mirrored texture
	if(SMirror || TMirror) {
		for(y = 0; y < Height; y++) row_pointers[y] = mirrored+(y*Width*4);

	// normal texture
	} else {
		for(y = 0; y < Height; y++) row_pointers[y] = Buffer+(y*Width*4);
	}

	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);

	free(row_pointers);
	row_pointers = NULL;

	free(mirrored);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(file);

	return EXIT_SUCCESS;
}
