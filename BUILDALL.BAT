@echo off

echo.
echo Compiling memory models:

copy build\*.mak . > nul
copy build\*.rsp . > nul

echo   Small...
command /c build\mk.bat s nul
if exist *.obj del *.obj > nul
echo   Medium...
command /c build\mk.bat m nul
if exist *.obj del *.obj > nul
echo   Compact...
command /c build\mk.bat c nul
if exist *.obj del *.obj > nul
echo   Large...
command /c build\mk.bat l nul
if exist *.obj del *.obj > nul
echo   Huge...
command /c build\mk.bat h nul

if exist *.obj del *.obj
if exist *.mak del *.mak
if exist *.rsp del *.rsp
if exist *.sym del *.sym
if exist *.cfg del *.cfg
