@echo off

CALL "%VS100COMNTOOLS%\vsvars32.bat" 2>NUL
IF ERRORLEVEL 1 GOTO VS_NEEDED

nmake -f makefile.msvc CFG=dbg
goto END

:VS_NEEDED
echo Visual Studio 2010 (vs10) doesn't seem to be installed
goto END

:END