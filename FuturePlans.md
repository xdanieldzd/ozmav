# Cleanup & Standardization #
_Priority: #1_

I propose that we should make a library for loading N64 games - or use one already avalible, such as [libn64rom](http://vg64tools.googlecode.com/svn/pc-lib/libn64rom/tags/v1.0/). Maybe we should standardize types as well (stdint.h) - but that shouldn't be an issue. Also, we should make more use of malloc(), realloc() and free() instead of declaring giant arrays - though this may come at a slight performance cost. General code cleanup is needed as well, and maybe some documentation for the more complex functions.

badRDP obviously needs work - I might take a stab at it, but xdaniel knows it much better than I do.

# Editing? #
_Once code cleanup has been done, of course_

SF64Toolkit looks as if it could easily become an editor, as it does have saving functions built in already. Level viewing seems to have progressed rather well - maybe a level setup editor could be implemented. I don't know anything about text, but I believe that [xdaniel has done some work](http://z64.spinout182.com/index.php?topic=43.msg276#msg276) on that.

OZMAV2 (maybe we should get a new name soon?) would make an awesome (setup) editor. I was talking with cooliscool the other day, and he thought that ozmav2 has a better future than UoT, in terms of viewing, editing, and being written in C.

## Model conversion ##
Exporting level data as Wavefront .obj is implemented (though currently broken, as I'm sure you know) in ozmav2, and should be implemented for SF64Tookit as well. I believe importing obj data (libobjn64 ?) should become a priority. [obj2c](http://spinout182.com/obj2c) could be used as a base - it already has triangle optimization, png and bmp support.