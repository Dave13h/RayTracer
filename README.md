# RayTracer

A Basic RayTracer using C++11 Threads and SDL

# The build file is pretty janky..

Yeah, its a Windows batch file that uses Visual Studio's cl.exe,
the dependancies location is hardwired and all that. Just look in the
build.bat :poop:

You'll need some DLLs to get this running once you've survived the build process too,
pretty much the SDL DLLs.

- libfreetype-6.dll
- SDL2.dll
- SDL2_ttf.dll
- zlib.dll

# Runtime options

-b<Number of bounces>       - Max number of bounces
-h<height>                  - Window Height
-i                          - Interlace Mode
-r                          - Realtime Mode
-t<Number of threads>       - Number of threads to spawn
-w<width>                   - Window Width

# Font

http://www.1001fonts.com/carrois-gothic-font.html

Copyright (c) 2011 by Ralph du Carrois, with Reserved Font Name 'Carrois'
This Font Software is licensed under the SIL Open Font License, Version 1.1.
This license is copied below, and is also available with a FAQ at:
http://scripts.sil.org/OFL