----------------------------------------------------------------------------------------------------
Project 'OZMAV2' - OpenGL Zelda Map Viewer

Written in 2010 by xdaniel & contributors
Using PDCurses and libMISAKA Console & UI Library
http://ozmav.googlecode.com/

libMISAKA - Written in 2010 by xdaniel & contributors
----------------------------------------------------------------------------------------------------

Prototype:
 OZMAV2.exe [options]

Options:
 -r PATH        Path to Ocarina of Time or Majora's Mask ROM to load
 -s SCENE       Initial scene number to load, in decimal (optional, defaults to 0).
 -d LEVEL       Level of debugging messages shown, between 0 and 3 (optional, defaults to 0).

Possible debug levels:
 0 = Errors only
 1 = Errors, warnings, general information only
 2 = Errors, warnings, general information, important debugging messages only
 3 = All messages

Example: 'OZMAV2 -r "Legend of Zelda, The - Ocarina of Time (U) (V1.0) [!].z64" -s 85 -d 1'
 The above loads the Ocarina of Time ROM named "Legend of Zelda, The - Ocarina of Time (U) (V1.0)
 [!].z64", loading Scene #85 initially, Kokiri Forest, and sets the debug level to 1, thus showing
 errors, warnings and general information about ex. the ROM.

----------------------------------------------------------------------------------------------------

For more information about the console commands that can be used, type "help" at the console, which
will list all available commands. Better documentation for those to follow.

----------------------------------------------------------------------------------------------------
 
Note that OZMAV2 still has bugs - in some regards even more than OZMAV had, like in the rendering
code - and that the Console/UI library used in the program is still in very early stages of
development. If there are problems that are not related to the Zelda- or N64-specific functionality
of OZMAV2, they're probably related to libMISAKA.

Many, many thanks to everyone who contributed to the original OZMAV, as well as everyone still or
previously involved into hacking the N64 Zelda games, no matter how insignificant there contribution
was!

----------------------------------------------------------------------------------------------------
