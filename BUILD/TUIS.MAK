.AUTODEPEND

#               *Translator Definitions*
CC = bcc +TUIS.CFG
TASM = H:\BC\BIN\TASM
TLIB = tlib
TLINK = tlink
LIBPATH = H:\BC\LIB
INCLUDEPATH = H:\BC\INCLUDE


#               *Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#               *List Macros*


EXE_dependencies =  \
 textio.obj \
 tuifiles.obj \
 tuikeys.obj \
 tuimouse.obj \
 tuidate.obj \
 tuistr.obj \
 tuistd.obj \
 tuiext.obj \
 tuigraph.obj \
 tuidlgs.obj \
 tuidemo.obj \
 gmouse.obj

#               *Explicit Rules*
tuidemo.exe: tuis.cfg $(EXE_dependencies)
  $(TLINK) /x/c/d/P-/L$(LIBPATH) @&&|
c0l.obj+
textio.obj+
tuifiles.obj+
tuikeys.obj+
tuimouse.obj+
tuidate.obj+
tuistr.obj+
tuistd.obj+
tuiext.obj+
tuigraph.obj+
tuidlgs.obj+
tuidemo.obj+
gmouse.obj
tuidemo
                # no map file
cl.lib
|


#               *Individual File Dependencies*
textio.obj: tuis.cfg textio.cpp

tuifiles.obj: tuis.cfg tuifiles.cpp

tuikeys.obj: tuis.cfg tuikeys.cpp

tuimouse.obj: tuis.cfg tuimouse.cpp

tuidate.obj: tuis.cfg tuidate.cpp

tuistr.obj: tuis.cfg tuistr.cpp

tuistd.obj: tuis.cfg tuistd.cpp

tuiext.obj: tuis.cfg tuiext.cpp

tuigraph.obj: tuis.cfg tuigraph.cpp

tuidlgs.obj: tuis.cfg tuidlgs.cpp

tuidemo.obj: tuis.cfg tuidemo.cpp

gmouse.obj: tuis.cfg gmouse.asm
        $(TASM) /MX /M2 /ZI /O GMOUSE.ASM,GMOUSE.OBJ

#               *Compiler Configuration File*
tuis.cfg: tuis.mak
  copy &&|
-R
-ml
-2
-f-
-C
-v
-y
-k-
-r-
-d
-H=TUIDEMO.SYM
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-P-.C
| tuis.cfg


