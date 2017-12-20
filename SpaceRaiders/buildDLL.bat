@echo off

if not defined DEV_ENV_DIR (
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
)
set DEV_ENV_DIR= ???

set DEFINES= /D "NDEBUG" /D "_CONSOLE" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /D "_CRT_SECURE_NO_WARNINGS"
set CFLAGS= -Zi -nologo /GS /GL /W4 /Gy /Zc:wchar_t /fp:fast /fp:except- %DEFINES%
set LFLAGS= -incremental:no user32.lib kernel32.lib

if not exist .\bin mkdir .\bin
pushd .\bin

del *.pdb > NUL 2> NUL

REM dlls
echo "WAITING FOR PDB ..." > lock.tmp
REM cl %CFLAGS% ../Win32Console.cpp -LD /link -PDB:Win32Console_%random%.pdb %LFLAGS%
REM cl %CFLAGS% ../src/Scripts.cpp -LD /link -PDB:Script_%random%.pdb %LFLAGS% Engine.lib
msbuild ../Scripts.vcxproj /p:Configuration=Debug
del lock.tmp

REM platform exe
REM cl %CFLAGS% ..\src\main.cpp /link %LFLAGS%

popd

echo Done!
