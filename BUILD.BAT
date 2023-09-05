@echo off
if '%1'=='' goto noparam
if '%1'=='s' goto ok
if '%1'=='m' goto ok
if '%1'=='c' goto ok
if '%1'=='l' goto ok
if '%1'=='h' goto ok
goto noparam
:ok
echo Working, please wait...
copy build\tui%1.mak . > nul
copy build\tuilib.rsp . > nul

if exist *.obj del *.obj

command /c build\mk.bat %1 con

if exist *.obj del *.obj
if exist *.mak del *.mak
if exist *.rsp del *.rsp
if exist *.sym del *.sym
if exist *.cfg del *.cfg

echo.
echo Done.
goto end
:noparam
echo Usage: BUILD memory_model
echo   memory_model = s, m, c, l, h
:end

