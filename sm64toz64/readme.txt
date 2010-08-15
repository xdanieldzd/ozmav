SM64 Fast3D to Zelda F3DEX2 Converter
By xdaniel with additions by spinout
Version 0.2.0
Built Aug 15 2010 20:17:36

Prototype:
sm64toz64 <SM64 ROM> <map name> <level ID> <level area> [options]
Where:
 <SM64 ROM> is a extended NTSC Mario 64 ROM,
 <map name> is the output scene/map name,
 <level ID> is the level in SM64 to convert (hex value), and
 <level area> is the area of the level you want to convert (usually 0x1)

Options:
 -l             Log messages to file 'log.txt'
 -i ROM SCENE   Insert scene/map to OoT Debug ROM. SCENE is decimal
 -o ADDRESS     Address to insert map/scene at if -i flag is enabled (Hex)
 -a, --about    Display program information
 -h, --help     This message

Example:
"SM64toZ64 SM64.z64 test 0x05 0x01 -l -i ZMQ_Debug.z64 108" - This inserts SM64's level 0x05, Bob-Omb's Battlefield, into a Debug ROM, using the default offset 0x35D0000, replacing scene 108 (Sasatest) in the game.

Possible level IDs are:
0x00 Haunted House
0x01 Cool Cool Mountain
0x02 Inside Castle
0x03 Hazy Maze Cave
0x04 Shifting Sand Land
0x05 Bob-Omb's Battlefield
0x06 Snow Man's land
0x07 Wet Dry World
0x08 Jolly Roger Bay
0x09 Tiny Huge Island
0x0A Tick Tock Clock
0x0B Rainbow Road
0x0C Castle Grounds
0x0D Bowser 1 Course
0x0E Vanish Cap
0x0F Bowser's Fire Sea
0x10 Secret Aquarium
0x11 Bowser 3 Course
0x12 Lethal Lava Land
0x13 Dire Dire Docks
0x14 Whomp's Fortress
0x15 Thank You cake picture at the end
0x16 Castle Courtyard
0x17 Peach's Secret Slide
0x18 Metal Cap
0x19 Wing Cap
0x1A Bowser 1 Battle Platform
0x1B Rainbow Clouds Bonus
0x1C Bowser 2 Battle Platform
0x1D Bowser 3 Battle Platform
0x1E Tall Tall Mountain

Please note that not all of them are converted properly at this point. Some may crash the converter, others might only be converted partially since they're mostly made of objects, yet others are pointless to convert anyway (mainly 0x15).

Many thanks to spinout for probably half of the whole code that's now in there, as well as to messiaen/frauber for documentation and advice regarding SM64's data!
