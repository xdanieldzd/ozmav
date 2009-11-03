-------------------------------------------------------------
OZMAV V0.8 (Build 'stapled dlists') - OpenGL Zelda Map Viewer
Written 2008/2009 by xdaniel & contributors

http://ozmav.googlecode.com/
-------------------------------------------------------------

- Usage:
   Open up a Zelda: Ocarina of Time or Majora's Mask ROM via the menu, or by dragging it into the OZMAV window. Then you can select which level to
   load via the F1/F2 keys or the treeview on the right side of the window. Supported ROM versions are:
     - Ocarina of Time, Japanese/American (V1.0, V1.1, V1.2)
     - Ocarina of Time, European (V1.0, V1.1)
     - Ocarina of Time - Master Quest (Debug ROM + certain modifications)
     - Majora's Mask, Japanese (V1.0, V1.1)
     - Majora's Mask, American (V1.0)
     - Majora's Mask, European (V1.0)
     - Majora's Mask, American (Kiosk Demo)
   All non-Debug ROMs must be decompressed variants, because OZMAV is not capable of decompressing the Yaz0 data found in most regular ROMs by itself.

- Controls:
   - Left mouse click + movement: Rotate camera
   - Right mouse click on actor: Show actor properties
   - W/A/S/D: move camera
   - Cursor keys: Rotate camera
   - F1/F2: Load previous/next scene from ROM
   - F3/F4: If scene has multiple maps, select which map to render
   - F5/F6: Select which environment setting to use (defaults to 0x01, generally daytime)
   - Numpad / and *: Select scene header to use

- Notes:
   Please note that OZMAV needs certain OpenGL extensions to be supported by your video card to emulate some aspects of the Nintendo 64 hardware.
   If, for example, the GL_ARB_fragment_program extension isn't supported, combiner emulation - responsible for certain types of coloring, multi-
   texturing, etc. - will be unavailable. If at least one such extension isn't supported on your system, OZMAV will show an error on startup, listing
   the unsupported extension or extensions.

   Also, OZMAV should not be viewed as an accurate representation of either the Nintendo 64 hardware, or what the hardware is capable of. While in
   certain cases OZMAV's output is pretty much identical of what proper emulators display, many inaccurate shortcuts have been taken in its source
   code, which is simply tailored to display levels from the Zelda games, nothing else.
