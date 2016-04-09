@echo off

set x=%VISUALSTUDIOVERSION%
if "%x%" == "" (
	call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
)

set CLOPTS=-nologo -MTd -Zi -GL -O2 -Wall /MP /EHsc /fp:fast^
	-wd4062 -wd4100 -wd4201 -wd4265 -wd4355 -wd4571 -wd4625^
	-wd4626 -wd4350 -wd4365 -wd4514 -wd4668 -wd4710 -wd4711^
	-wd4820^
	/I"..\libraries\SDL2-2.0.3\include"^
	/I"..\libraries\SDL2_ttf-2.0.12\include"

set LINKOPTS=/link "kernel32.lib" "user32.lib" "gdi32.lib" "shell32.lib"^
	"SDL2.lib" "SDL2main.lib" "SDL2_ttf.lib" ^
	/libpath:"..\Libraries\SDL2-2.0.3\VisualC\SDL\Win32\Release"^
	/libpath:"..\Libraries\SDL2-2.0.3\VisualC\SDLmain\Win32\Release"^
	/libpath:"..\Libraries\SDL2_ttf-2.0.12\lib\x86"

set SRCFILES=src/main.cpp^
	src/camera.cpp^
	src/draw.cpp^
	src/jobmanager.cpp^
	src/light.cpp^
	src/matrix.cpp^
	src/ray.cpp^
	src/scene.cpp^
	src/shape.cpp^
	src/timer.cpp^
	src/vector.cpp

cl %CLOPTS% %SRCFILES% %LINKOPTS% /out:raytracer.exe
