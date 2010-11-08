typedef struct {
	char title[21];
	char gameID[5];
	int version;
	int gameType;
	int codeFileNo;			// starting at 0
	int codeRAMOffset;
	unsigned int objectTableOffset;
	unsigned int actorTableOffset;
	int objectCount;
	int actorCount;
	char titleText[64];
} vZeldaVersionStruct;

static vZeldaVersionStruct GameVer[] =
{
	// retail releases
/*	{ "THE LEGEND OF ZELDA ", "CZLJ", 0,  0, 27, 0x8001CE60, 0x0E7F58, 0x0D7490, 402, 471, "Ocarina of Time (J) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 1,  0, 27, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (J) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 2,  0, 27, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (J) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 0,  0, 27, 0x8001CE60, 0x0E7F58, 0x0D7490, 402, 471, "Ocarina of Time (U) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 1,  0, 27, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (U) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 2,  0, 27, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (U) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 0,  0, 28, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (E) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 1,  0, 28, 0x8001CE60, 0x0,      0x0,      402, 471, "Ocarina of Time (E) [v1.1]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 0,  1, 28, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (J) [v1.0]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 1,  1, 28, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (J) [v1.1]" },
	{ "ZELDA MAJORA'S MASK ", "NZSE", 0,  1, 31, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (U) [v1.0]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 0,  1, 49, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (E) [v1.0]" },
*/
	// demos, developer builds
	{ "THE LEGEND OF ZELDA ", "NZLE", 15, 0, 28, 0x8001CE60, 0x10A6C8, 0x0F9440, 402, 471, "Ocarina of Time Master Quest (E) [Debug Version]" },
/*	{ "MAJORA'S MASK       ", "NDLE", 0,  1, 31, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (U) [Kiosk Demo]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 1,  1, 49, 0x8001CE60, 0x0,      0x0,      642, 0,   "Majora's Mask (E) [Debug Version]" },

	// hacks
	{ "THE LEGEND OF DEBUG ", "CZLE", 15, 0, 28, 0x8001CE60, 0x10A6C8, 0x0F9440, 402, 471, "Ocarina of Time: Master Quest [Debug Version]" },
	{ "ZELDA OOT NIGHTMARE ", "NZLE", 15, 0, 28, 0x8001CE60, 0x10A6C8, 0x0F9440, 402, 471, "Ocarina of Time: Master Quest [Debug Version]" }*/
};
