@echo off
make -ftui%1.mak > %2
if exist tui%1.lib del tui%1.lib
tlib tui%1 /C @tuilib.rsp > %2
:del *.obj
