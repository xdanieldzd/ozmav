typedef struct {
	char Title[21];
	char GameID[5];
	int Version;
	int GameType;
	int SceneTableFileNo;			// starting at 0
	unsigned int SceneTableOffset;
	int SceneCount;
	char TitleText[256];
} __zGameVer;

static __zGameVer GameVer[] =
{
	// retail releases
	{ "THE LEGEND OF ZELDA ", "CZLJ", 0, 0, 27, 0x0EA440, 0x64, "Ocarina of Time (J) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 1, 0, 27, 0x0EA600, 0x64, "Ocarina of Time (J) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 2, 0, 27, 0x0EA450, 0x64, "Ocarina of Time (J) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 0, 0, 27, 0x0EA440, 0x64, "Ocarina of Time (U) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 1, 0, 27, 0x0EA600, 0x64, "Ocarina of Time (U) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 2, 0, 27, 0x0EA450, 0x64, "Ocarina of Time (U) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 0, 0, 28, 0x0E7D60, 0x64, "Ocarina of Time (E) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 1, 0, 28, 0x0E7DA0, 0x64, "Ocarina of Time (E) [v1.1]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 0, 1, 28, 0x117510, 0x70, "Majora's Mask (J) [v1.0]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 1, 1, 28, 0x1177E0, 0x70, "Majora's Mask (J) [v1.1]" },
	{ "ZELDA MAJORA'S MASK ", "NZSE", 0, 1, 31, 0x11E1E0, 0x70, "Majora's Mask (U) [v1.0]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 0, 1, 49, 0x11E860, 0x70, "Majora's Mask (E) [v1.0]" },

	// demos, developer builds
	{ "THE LEGEND OF ZELDA ", "NZLE", 15, 0, 28, 0x10CBB0, 0x6D, "Ocarina of Time Master Quest (E) [Debug Version]" },
	{ "MAJORA'S MASK       ", "NDLE", 0, 1, 31, 0x11D3E0, 0x70, "Majora's Mask (U) [Kiosk Demo]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 1, 1, 49, 0x1562B0, 0x70, "Majora's Mask (E) [Debug Version]" },

	// hacks
	{ "THE LEGEND OF DEBUG ", "CZLE", 15, 0, 28, 0x10CBB0, 0x6D, "Ocarina of Time: Master Quest [Debug Version]" },
	{ "ZELDA OOT NIGHTMARE ", "NZLE", 15, 0, 28, 0x10CBB0, 0x6D, "Ocarina of Time: Master Quest [Debug Version]" }
};
