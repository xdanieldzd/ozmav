typedef struct {
	int SceneNo;
	char Name[256];
} __zGameScene;

static __zGameScene GameScene[] =
{
	{ 0, "Inside the Deku Tree" },
	{ 1, "Dondongo's Cavern" },
	{ 2, "Inside Jabu Jabu's Belly" },
	{ 3, "Forest Temple" },
	{ 4, "Fire Temple" },
	{ 5, "Water Temple" },
	{ 6, "Spirit Temple" },
	{ 7, "Shadow Temple" },
	{ 8, "Bottom of the Well" },
	{ 9, "Ice Cavern" },
	{ 10, "Ganon's Tower" },
	{ 11, "Gerudo Training Grounds" },
	{ 12, "Thieves' Hideout" },
	{ 13, "Inside Ganon's Castle" },
	{ 14, "Ganon's Tower (collapsing)" },
	{ 15, "Inside Ganon's Castle (collapsing)" },
	{ 16, "Treasure Chest Shop" },
	{ 17, "Deku Tree (Boss Room)" },
	{ 18, "Dondogo Cavern (Boss Room)" },
	{ 19, "Inside Jabu Jabu's Belly (Boss Room)" },
	{ 20, "Forest Temple (Boss Room)" },
	{ 21, "Fire Temple (Boss Room)" },
	{ 22, "Water Temple (Boss Room)" },
	{ 23, "Spirit Temple (Boss Room)" },
	{ 24, "Shadow Temple (Boss Room)" },
	{ 25, "Ganon's Tower (Boss Room)" },
	{ 26, "Outside Ganon's Tower (escape)" },
	{ 27, "Market Entrance (Day)" },
	{ 28, "Market Entrance (Night)" },
	{ 29, "Market Entrance (Adult)" },
	{ 30, "Back Alley (Day)" },
	{ 31, "Back Alley (Night)" },
	{ 32, "Market (Day)" },
	{ 33, "Market (Night)" },
	{ 34, "Market (Adult)" },
	{ 35, "Outside Temple of Time (day)" },
	{ 36, "Outside Temple of Time (night)" },
	{ 37, "Outside Temple of Time (adult)" },
	{ 38, "Know-It-All Brothers" },
	{ 39, "House of Twins" },
	{ 40, "Mido's House" },
	{ 41, "Saria's House" },
	{ 42, "Kakariko Village House" },
	{ 43, "Back Alley Village House" },
	{ 44, "Kakariko Bazaar" },
	{ 45, "Kokiri Shop" },
	{ 46, "Goron Shop" },
	{ 47, "Zora Shop" },
	{ 48, "Kakariko Potion Shop" },
	{ 49, "Market Potion Shop" },
	{ 50, "Bombchu Shop" },
	{ 51, "Happy Mask Shop" },
	{ 52, "Link's House" },
	{ 53, "Puppy Woman's House" },
	{ 54, "Stable" },
	{ 55, "Impa's House" },
	{ 56, "Lakeside Laboratory" },
	{ 57, "Carpenter's Tent" },
	{ 58, "Dampe's Hut" },
	{ 59, "Great Fairy Fountain" },
	{ 60, "Small Fairy Fountain" },
	{ 61, "Magic Fairy Fountain" },
	{ 62, "Grottos" },
	{ 63, "Grave (1)" },
	{ 64, "Grave (2)" },
	{ 65, "Royal Family's Tomb" },
	{ 66, "Shooting Gallery" },
	{ 67, "Temple of Time" },
	{ 68, "Chamber of Sages" },
	{ 69, "Castle Courtyard (day)" },
	{ 70, "Castle Courtyard (night)" },
	{ 71, "Cutscene Map" },
	{ 72, "Dampe's Grave" },
	{ 73, "Fishing Pond" },
	{ 74, "Zelda's Courtyard" },
	{ 75, "Bombchu Bowling Alley" },
	{ 76, "Talon's House" },
	{ 77, "Lots'o Pots" },
	{ 78, "Granny's Potion Shop" },
	{ 79, "Ganon's Castle (final battle)" },
	{ 80, "Skulltula House" },
	{ 81, "Hyrule Field" },
	{ 82, "Kakariko Village" },
	{ 83, "Graveyard" },
	{ 84, "Zora River" },
	{ 85, "Kokiri Forest" },
	{ 86, "Sacred Forest Meadow" },
	{ 87, "Lake Hylia" },
	{ 88, "Zora's Domain" },
	{ 89, "Zora's Fountain" },
	{ 90, "Gerudo Valley" },
	{ 91, "Lost Woods" },
	{ 92, "Desert Colossus" },
	{ 93, "Gerudo's Fortress" },
	{ 94, "Haunted Wasteland" },
	{ 95, "Hyrule Castle" },
	{ 96, "Death Mountain" },
	{ 97, "Death Mountain Crater" },
	{ 98, "Goron City" },
	{ 99, "Lon Lon Ranch" },
	{ 100, "Ganon's Castle" },
	{ 101, "Test Map" },
	{ 102, "Besitu" },
	{ 103, "Depth Test" },
	{ 104, "Middle Stalfos Room" },
	{ 105, "Boss Stalfos Room" },
	{ 106, "Dark Link Testing Area" },
	{ 107, "Castle Courtyard (prototype)" },
	{ 108, "Sasatest" },
	{ 109, "Test Room" }
};

typedef struct {
	char Title[21];
	char GameID[5];
	int Version;
	int GameType;
	int CodeFileNo;			// starting at 0
	int CodeRAMOffset;
	unsigned int SceneTableOffset;
	unsigned int ObjectTableOffset;
	unsigned int ActorTableOffset;
	int SceneCount;
	int ObjectCount;
	int ActorCount;
	char TitleText[64];
} __zGameVer;

static __zGameVer GameVer[] =
{
	// retail releases
	{ "THE LEGEND OF ZELDA ", "CZLJ", 0,  0, 27, 0x8001CE60, 0x0EA440, 0x0E7F58, 0x0D7490, 0x64, 402, 469, "Ocarina of Time (J) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 1,  0, 27, 0x8001CE60, 0x0EA600, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (J) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLJ", 2,  0, 27, 0x8001CE60, 0x0EA450, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (J) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 0,  0, 27, 0x8001CE60, 0x0EA440, 0x0E7F58, 0x0D7490, 0x64, 402, 469, "Ocarina of Time (U) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 1,  0, 27, 0x8001CE60, 0x0EA600, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (U) [v1.1]" },
	{ "THE LEGEND OF ZELDA ", "CZLE", 2,  0, 27, 0x8001CE60, 0x0EA450, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (U) [v1.2]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 0,  0, 28, 0x8001CE60, 0x0E7D60, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (E) [v1.0]" },
	{ "THE LEGEND OF ZELDA ", "NZLP", 1,  0, 28, 0x8001CE60, 0x0E7DA0, 0x0,      0x0,      0x64, 402, 469, "Ocarina of Time (E) [v1.1]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 0,  1, 28, 0x8001CE60, 0x117510, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (J) [v1.0]" },
	{ "THE MASK OF MUJURA  ", "NZSJ", 1,  1, 28, 0x8001CE60, 0x1177E0, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (J) [v1.1]" },
	{ "ZELDA MAJORA'S MASK ", "NZSE", 0,  1, 31, 0x8001CE60, 0x11E1E0, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (U) [v1.0]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 0,  1, 49, 0x8001CE60, 0x11E860, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (E) [v1.0]" },

	// demos, developer builds
	{ "THE LEGEND OF ZELDA ", "NZLE", 15, 0, 28, 0x8001CE60, 0x10CBB0, 0x10A6C8, 0x0F9440, 0x6D, 402, 469, "Ocarina of Time Master Quest (E) [Debug Version]" },
	{ "MAJORA'S MASK       ", "NDLE", 0,  1, 31, 0x8001CE60, 0x11D3E0, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (U) [Kiosk Demo]" },
	{ "ZELDA MAJORA'S MASK ", "NZSP", 1,  1, 49, 0x8001CE60, 0x1562B0, 0x0,      0x0,      0x70, 642, 689, "Majora's Mask (E) [Debug Version]" },

	// hacks
	{ "THE LEGEND OF DEBUG ", "CZLE", 15, 0, 28, 0x8001CE60, 0x10CBB0, 0x10A6C8, 0x0F9440, 0x6D, 402, 469, "Ocarina of Time: Master Quest [Debug Version]" },
	{ "ZELDA OOT NIGHTMARE ", "NZLE", 15, 0, 28, 0x8001CE60, 0x10CBB0, 0x10A6C8, 0x0F9440, 0x6D, 402, 469, "Ocarina of Time: Master Quest [Debug Version]" }
};
