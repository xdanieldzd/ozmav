----------------------------------------------------------------------------------------------------
OZMAV2 0.1 (build Mar 29 2010 00:41:20)
OpenGL Zelda Map Viewer

Written in 2010 by xdaniel - http://ozmav.googlecode.com/

Using PDCurses and libMISAKA Console & UI Library
libMISAKA - Written in 2010 by xdaniel
----------------------------------------------------------------------------------------------------

Prototype:
OZMAV2 <Z64 ROM> <Scene ID> [options]
Where:
 <Z64 ROM> is an Ocarina of Time or Majora's Mask ROM image, no matter the version, and
 <Scene ID> is the ID of the Scene to load initially (optional, defaults to 0).

Options:
 -d LEVEL       Level of debugging messages to output, listed below.

Possible debug levels:
 0 = Errors only
 1 = Errors, warnings, general information only
 2 = Errors, warnings, general information, important debugging messages only
 3 = All messages

For more information about the console commands that can be used, type "help" at the console, which will list all available commands. Better documentation for those to follow.
 
Example:
'OZMAV2 "Legend of Zelda, The - Ocarina of Time (U) (V1.0) [!].z64" 85 -d 1' - This loads the Ocarina of Time ROM named "Legend of Zelda, The - Ocarina of Time (U) (V1.0) [!].z64", loading Scene #85 initially, Kokiri Forest, and sets the debug level to 1, thus showing errors, warnings and general information about ex. the ROM.

Note that OZMAV2 still has bugs - in some regards even more than OZMAV had, like in the rendering code - and that the Console/UI library used in the program is still in very early stages of development. If there are problems that are not related to the Zelda- or N64-specific functionality of OZMAV2, they're probably related to libMISAKA.

Many, many thanks to everyone who contributed to the original OZMAV, as well as everyone still or previously involved into hacking the N64 Zelda games, no matter how insignificant there contribution was!
