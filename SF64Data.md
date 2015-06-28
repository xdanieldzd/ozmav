(Note: All offsets are for game version US 1.0!)

**(Note 2: Page BADLY needs update!)**

# Level Files #

To my knowledge, each level has one file inside the game's "file system" that contains all of its data, besides sound-related things - textures, models, level layout. Thus, one way to find a specific level's file is to check each file for textures that belong to the level you want to find.

## Level Setup ##

The _level setup data_ or _level header_ is a block of 0x44 (68) bytes that sets such things as the BGM, fog properties and more. It's always located directly before the actual level layout data in each file. Pointers to each setup data block can be found inside the second file in the file system, at offset 0xCE158, or at offset 0xCF1A8 in the regular ROM. This pointer table is ordered by level IDs, which is the same as the Level Select Gameshark code, so ex. Corneria's pointer is at 0xCE158/0xCF1A8, while the Training stage's is at 0xCE180/0xCF1D0.

Using the Training stage's data as an example here, which has its setup data at 0x6A60 inside its file, the following level properties are known: (absolute offsets used)

  * **0x6A6B: BGM** - 1 byte, defines which BGM is used on the stage, ex. 0x3F for Training
  * **0x6A6F: Fog color (R)** - 1 byte, red color component for fog
  * **0x6A73: Fog color (G)** - 1 byte, green color component for fog
  * **0x6A77: Fog color (B)** - 1 byte, blue color component for fog
  * **0x6A7A: Fog near value** - 2 bytes, ex. 0x03E4 (= 996)
  * **0x6A7E: Fog far value** - 2 bytes, ex. 0x03E8 (= 1000)
  * (Note: Valid ranges for fog near/far values are not known, I've seen graphical glitches and crashes while experimenting with those!)

The byte at offset 0x6A63 _seems_ to be related to the level type (ground, space, water), and the single bytes at offsets 0x6A8F to 0x6AA3 are lighting color RGB components.

## Level Layout ##

As mentioned before, the _level layout data_ always follows the 0x44 bytes of level setup data. Each level appears to be fully made of objects, besides the level's ground plane, which are placed using this data. Each entry in the level layout is 0x14 (20) bytes long.

The syntax seems to be as follows, again using the Training stage as an example, where it starts at offset 0x6AA4:

  * **0x6AA4: Ascending in-level position** - 4 bytes, ex. 0x42C80000, this must NOT descend, otherwise no further objects will be loaded(!?)
  * **0x6AA8: X coordinate** - 2 bytes, ex. 0xF448
  * **0x6AAA: Y coordinate** - 2 bytes, ex. 0xFB50
  * **0x6AAC: Z coordinate** - 2 bytes, ex. 0xFF38
  * **0x6AAE: X rotation** - 2 bytes, ex. 0x0000
  * **0x6AB0: Y rotation** - 2 bytes, ex. 0x0000
  * **0x6AB2: Z rotation** - 2 bytes, ex. 0x0000
  * **0x6AB4: Object type** - 2 bytes, ex. 0x0085
  * **0x6AB6: Unknown** - 2 bytes, appear to be always 0000

The initial in-level position appears to always be 0x42C80000 and it is unknown how exactly it works. Also, coordinates appear to be somehow relative to the current in-level position.

Here is a short and incomplete list of object types, as tested in the Training level. Not all objects are available in all levels, and it is currently unknown if additional object models can be loaded per level.

  * **0x0085**: Building/Tower
  * **0x0142**: Item: Laser power-up
  * **0x0143**: Checkpoint
  * **0x0144**: Item: Silver ring
  * **0x0145**: Item: Silver star
  * **0x0146**: Gate initiating rotation
  * **0x0147**: Item: Bomb
  * **0x0148**: Training fox-guy putting you into all-range mode
  * **0x014E**: Invisible training mode fly-through ring?
  * **0x014F**: Item: Arwing (1up?)
  * **0x0150**: Item: Golden ring
  * **0x0151**: Item: Wing repair
  * **0x0152**: Training mode fly-through ring
  * **0x03E8+**: ROB64's training mode speech
  * **0x0410**: Enemy: Ground-based cannon
  * **0x0411**: Enemy: Stationary floating enemy (needs high Z coordinate to appear in level, around 0x0650+)
  * **0x0412**: Enemy: Winged enemy, stationary (needs high Z coordinate)
  * **0x0413**: Enemy: Winged enemy, moving (need high Z coordinate)
  * **0x0414**: Enemy: Other stationary floating enemy
  * **0x0415**: Item: Wing repair (being dropped?)
  * **0x0442**: Moving wall
  * **0xFFFF**: Data end marker