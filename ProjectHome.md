The OZMAV series of programs, namesakes of this project page, are OpenGL-based map viewers for a variety of Nintendo 64 games, mainly _The Legend of Zelda: Ocarina of Time_ and _Majora's Mask_.

The original OZMAV was written in C, using the Windows API for its GUI. This version is still available from the SVN, but has since been deprecated.

Its replacement, called OZMAV2, is a "semi-cross-platform" rewrite of the program. It's still written in C, but is now buildable under both Windows and Linux, using the Windows API or Xlib (depending on the platform), and uses a console and UI library called libMISAKA, based on the curses library, for user interaction.

**Note:** OZMAV2 requires _libpng 1.2.x_ and _zlib_ binaries to work! Windows versions of those are available ex. at http://gnuwin32.sourceforge.net/packages/libpng.htm