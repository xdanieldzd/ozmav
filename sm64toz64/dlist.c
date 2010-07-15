#include "globals.h"

int ConvertDList(unsigned int Seg, unsigned int Offset, bool CalledViaCmd)
{
	// tag OK: command converts correctly
	// tag NG: command doesn't yet convert correctly / conversion is untested / isn't yet covered
	// tag XX: command is deliberately ignored at the moment

	dmsg("[RAM] Display List at %02X%06X...\n", Seg, Offset);

	unsigned int w0 = 0, w1 = 0;
	unsigned int TargetOffset = Offset + ZMAP_HEADERGAP;

	while(!DListHasEnded) {
		w0 = Read32(RAMSegment[Seg].Data, Offset);
		w1 = Read32(RAMSegment[Seg].Data, Offset + 4);

		memset(&ZMapBuffer[TargetOffset], 0x00, 8);

		switch(RAMSegment[Seg].Data[Offset]) {
			case G_SETTIMG: {
				dmsg("- Found G_SETTIMG at 0x%06X...\t\tOK\n", Offset);

				Write32(ZMapBuffer, TargetOffset, w0);

				// write address
				unsigned int new_w1 = 0x00;

				if(RAMSegment[Seg].Data[Offset + 4] == 0x09) {			// if texture originally from RAM segment 0x09 (textures)
					new_w1 = (0x03000000 | ((w1 + ZMapFilesize - TextureSize) & 0x00FFFFFF));
				} else if(RAMSegment[Seg].Data[Offset + 4] == 0x07) {	// if texture originally from RAM segment 0x07 (level display lists)
					new_w1 = (0x03000000 | ((w1 + ZMAP_HEADERGAP) & 0x00FFFFFF));
				} else {												// just write original data, results in garbage
					new_w1 = w1;
					dmsg(" - Warning, texture in RAM segment 0x%02X, will result in garbage!\n", RAMSegment[Seg].Data[Offset + 4]);
				}
				Write32(ZMapBuffer, TargetOffset + 4, new_w1);

				break; }

			case F3D_VTX: {
				// write address
				Write32(ZMapBuffer, TargetOffset + 4, (0x03000000 | ((w1 + ZMAP_HEADERGAP) & 0x00FFFFFF)));

				// extract n & v0
				unsigned int n = _SHIFTR(w0, 20, 4) + 1;
				unsigned int v0 = _SHIFTR(w0, 16, 4);

				// write n & v0
				w0 = F3DEX2_VTX << 24;
				unsigned int new_v0 = (n + v0) << 12;
				w0 += new_v0;
				unsigned int new_n = n * 2;
				w0 += new_n;
				Write32(ZMapBuffer, TargetOffset, w0);

				// write vtx data to target buffer
				if(RAMSegment[Seg].Data[Offset + 4] == 0x07) {
					int vtxoffset = (w1 & 0x00FFFFFF);
					int VertexBase = vtxoffset + ZMAP_HEADERGAP;
					memcpy(&ZMapBuffer[VertexBase], &RAMSegment[Seg].Data[vtxoffset], n * 0x10);

					// scale verts
					int j;
					signed short x, y, z;
					for (j=0; j < (n<<4); j+=16)
					{
						x = Read16(ZMapBuffer, VertexBase+j+0);
						y = Read16(ZMapBuffer, VertexBase+j+2);
						z = Read16(ZMapBuffer, VertexBase+j+4);
						x += Xrepos;
						y += Yrepos;
						z += Zrepos;
						x /= SCALE;
						y /= SCALE;
						z /= SCALE;
						Write16(ZMapBuffer, VertexBase+j, x);
						Write16(ZMapBuffer, VertexBase+j+2, y);
						Write16(ZMapBuffer, VertexBase+j+4, z);
					}
				}

				dmsg("- Found F3D_VTX at 0x%06X...\t\t\tOK\n", Offset);
				break; }

			case F3D_TRI1: {
				Write32(ZMapBuffer, TargetOffset, ((F3DEX2_TRI1 << 24) | \
					((RAMSegment[Seg].Data[Offset + 5] / 0x0A) * 2) << 16 | \
					((RAMSegment[Seg].Data[Offset + 6] / 0x0A) * 2) << 8 | \
					((RAMSegment[Seg].Data[Offset + 7] / 0x0A) * 2) \
					));

				dmsg("- Found F3D_TRI1 at 0x%06X...\t\t\tOK\n", Offset);
				break; }

			case F3D_TRI4: {
				ZMapBuffer[TargetOffset] = F3DEX2_SPNOOP;

				dmsg("- Found F3D_TRI4 at 0x%06X...\t\t\tXX\n", Offset);
				break; }

			case F3D_TEXTURE: {
				unsigned int new_w0 = ((F3DEX2_TEXTURE << 24) | \
					RAMSegment[Seg].Data[Offset + 1] << 16 | \
					RAMSegment[Seg].Data[Offset + 2] << 8 | \
					_SHIFTL(RAMSegment[Seg].Data[Offset + 3], 1, 7));	// convert "on"

				Write64(ZMapBuffer, TargetOffset, new_w0, w1);

				dmsg("- Found F3D_TEXTURE at 0x%06X...\t\tOK\n", Offset);
				break; }

			case F3D_SETGEOMETRYMODE: {
				// convert parameters
				if(w1 & F3D_TEXTURE_ENABLE)	{ w1 &= ~F3D_TEXTURE_ENABLE; w1 |= F3DEX2_TEXTURE_ENABLE; }
				if(w1 & F3D_SHADING_SMOOTH)	{ w1 &= ~F3D_SHADING_SMOOTH; w1 |= F3DEX2_SHADING_SMOOTH; }
				if(w1 & F3D_CULL_FRONT)		{ w1 &= ~F3D_CULL_FRONT; w1 |= F3DEX2_CULL_FRONT; }
				if(w1 & F3D_CULL_BACK)		{ w1 &= ~F3D_CULL_BACK; w1 |= F3DEX2_CULL_BACK; }
				if(w1 & F3D_CULL_BOTH)		{ w1 &= ~F3D_CULL_BOTH; w1 |= F3DEX2_CULL_BOTH; }
				if(w1 & F3D_CLIPPING)		{ w1 &= ~F3D_CLIPPING; w1 |= F3DEX2_CLIPPING; }

				Write64(ZMapBuffer, TargetOffset, ((F3DEX2_GEOMETRYMODE << 24) | 0xFFFFFF), w1);

				dmsg("- Found F3D_SETGEOMETRYMODE at 0x%06X...\tOK\n", Offset);
				break; }

			case F3D_CLEARGEOMETRYMODE: {
				// convert parameters
				if(w1 & F3D_TEXTURE_ENABLE)	{ w1 &= ~F3D_TEXTURE_ENABLE; w1 |= F3DEX2_TEXTURE_ENABLE; }
				if(w1 & F3D_SHADING_SMOOTH)	{ w1 &= ~F3D_SHADING_SMOOTH; w1 |= F3DEX2_SHADING_SMOOTH; }
				if(w1 & F3D_CULL_FRONT)		{ w1 &= ~F3D_CULL_FRONT; w1 |= F3DEX2_CULL_FRONT; }
				if(w1 & F3D_CULL_BACK)		{ w1 &= ~F3D_CULL_BACK; w1 |= F3DEX2_CULL_BACK; }
				if(w1 & F3D_CULL_BOTH)		{ w1 &= ~F3D_CULL_BOTH; w1 |= F3DEX2_CULL_BOTH; }
				if(w1 & F3D_CLIPPING)		{ w1 &= ~F3D_CLIPPING; w1 |= F3DEX2_CLIPPING; }

				Write32(ZMapBuffer, TargetOffset, ((F3DEX2_GEOMETRYMODE << 24) | (~w1 & 0x00FFFFFF)));

				dmsg("- Found F3D_CLEARGEOMETRYMODE at 0x%06X...\tOK\n", Offset);
				break; }

			case F3D_CULLDL: {
				ZMapBuffer[TargetOffset] = F3DEX2_SPNOOP;

				dmsg("- Found F3D_CULLDL at 0x%06X...\t\t\tXX\n", Offset);
				break; }

			case F3D_SETOTHERMODE_H: {
				Write64(ZMapBuffer, TargetOffset, ((F3DEX2_SETOTHERMODE_H << 24) | 0x001001), w1);

				dmsg("- Found F3D_SETOTHERMODE_H at 0x%06X...\t\tNG\n", Offset);
				break; }

			case F3D_SETOTHERMODE_L: {
				if(RAMSegment[Seg].Data[Offset + 2] == 0x03) {
					unsigned int new_w0 = ((F3DEX2_SETOTHERMODE_L << 24) | ((_SHIFTR( w0, 8, 8 ) * 9) + 1));
					Write32(ZMapBuffer, TargetOffset, new_w0);
					Write32(ZMapBuffer, TargetOffset + 4, w1);
				} else {
					ZMapBuffer[TargetOffset] = F3DEX2_SPNOOP;
				}

				dmsg("- Found F3D_SETOTHERMODE_L at 0x%06X...\t\tNG\n", Offset);
				break; }

			case F3D_RDPHALF_1: {
				Write32(ZMapBuffer, TargetOffset, ((F3DEX2_RDPHALF_1 << 24) | (w0 & 0x00FFFFFF)));

				if(RAMSegment[Seg].Data[Offset + 4] == 0x03) {
					Write32(ZMapBuffer, TargetOffset + 4, (0x03000000 | ((w1 + ZMAP_HEADERGAP) & 0x00FFFFFF)));
				} else {
					Write32(ZMapBuffer, TargetOffset + 4, w1);
				}

				dmsg("- Found F3D_RDPHALF_1 at 0x%06X...\t\tOK\n", Offset);
				break; }

			case F3D_MOVEMEM: {
				if(((RAMSegment[Seg].Data[Offset + 1] >= 0x86) && (RAMSegment[Seg].Data[Offset + 1] <= 0x94)) && (RAMSegment[Seg].Data[Offset + 4] == 0x07)) {
					int lightno = (RAMSegment[Seg].Data[Offset + 1] / 2) - 0x42;
					lightno = 0x06 + (lightno * 0x03);

					Write64(ZMapBuffer, TargetOffset, ((F3DEX2_MOVEMEM << 24) | \
						0x08000A | \
						lightno << 8), \
						(0x03000000 | ((w1 + ZMAP_HEADERGAP) & 0x00FFFFFF)));
				} else {
					ZMapBuffer[TargetOffset] = F3DEX2_SPNOOP;
				}

				dmsg("- Found F3D_MOVEMEM at 0x%06X...\t\tNG\n", Offset);
				break; }

			case F3D_MOVEWORD: {
				ZMapBuffer[TargetOffset] = F3DEX2_SPNOOP;

				dmsg("- Found F3D_MOVEWORD at 0x%06X...\t\t\tXX\n", Offset);
				break; }

			case F3D_DL: {
				Write64(ZMapBuffer, TargetOffset, ((F3DEX2_DL << 24) | (w0 & 0x00FFFFFF)), (0x03000000 | ((w1 + ZMAP_HEADERGAP) & 0x00FFFFFF)));

				ConvertDList((w1 & 0xFF000000) >> 24, (w1 & 0x00FFFFFF), true);

				dmsg("- Found F3D_DL at 0x%06X...\t\t\tOK\n", Offset);
				break; }

			case F3D_ENDDL: {
				ZMapBuffer[TargetOffset] = F3DEX2_ENDDL;

				DListHasEnded = true;

				dmsg("- Found F3D_ENDDL at 0x%06X...\t\tOK\n", Offset);
				break; }

			default: {
				Write64(ZMapBuffer, TargetOffset, w0, w1);
				break; }
		}

		TargetOffset += 8;
		Offset += 8;
	}

	if(CalledViaCmd) DListHasEnded = false;

	return 0;
}
