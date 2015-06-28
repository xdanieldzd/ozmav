# Star Fox 64 Toolkit Usage #

When starting the program's executable, you'll be greeted by its console interface and a "No ROM loaded!" message. At this point, you'll first of all want to open a ROM. The command **loadrom sf64.z64** will open up the file with the specified filename, in this case sf64.z64, in the same folder that the executable resides in.

Once the ROM has been loaded, the programs tries to identify it and load its DMA table. The resulting output might look something like this:

```
 - Filename:      sf64.z64
 - Size:          12MB (96 Mbit)
 - Title:         STARFOX64
 - Game ID:       NFXE
 - Version:       1.0
 - CRC1:          0xA7D015F8
 - CRC2:          0x2289AA43

- ROM has been recognized as 'Star Fox 64 (U) [v1.0]'.

- Reading DMA table...
- 64 files found in DMA table.

- ROM has been loaded!
```

Once this has been done, and the ROM has been recognized as a supported SF64/Lylat Wars ROM, you can, for example, create a "decompressed ROM". This kind of ROM will not contain compressed data anymore (the well-known MIO0 blocks) and can thus be edited much easier. The command **expandrom sf64\_dec.z64** would create such a decompressed ROM from the currently loaded one, and save it with the filename sf64\_dec.z64.

An example video, using the still experimental **extractfiles/createrom** method, is provided below:

<a href='http://www.youtube.com/watch?feature=player_embedded&v=YTqQueGfEh4' target='_blank'><img src='http://img.youtube.com/vi/YTqQueGfEh4/0.jpg' width='425' height=344 /></a>

(More throughout documentation to follow)