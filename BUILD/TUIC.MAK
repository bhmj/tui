.AUTODEPEND

#               *Translator Definitions*
CC = bcc +TUIC.CFG
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
tuidemo.exe: tuic.cfg $(EXE_dependencies)
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
textio.obj: tuic.cfg textio.cpp

tuifiles.obj: tuic.cfg tuifiles.cpp

tuikeys.obj: tuic.cfg tuikeys.cpp

tuimouse.obj: tuic.cfg tuimouse.cpp

tuidate.obj: tuic.cfg tuidate.cpp

tuistr.obj: tuic.cfg tuistr.cpp

tuistd.obj: tuic.cfg tuistd.cpp

tuiext.obj: tuic.cfg tuiext.cpp

tuigraph.obj: tuic.cfg tuigraph.cpp

tuidlgs.obj: tuic.cfg tuidlgs.cpp

tuidemo.obj: tuic.cfg tuidemo.cpp

gmouse.obj: tuic.cfg gmouse.asm
        $(TASM) /MX /M2 /ZI /O GMOUSE.ASM,GMOUSE.OBJ

#               *Compiler Configuration File*
tuic.cfg: tuic.mak
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
| tuic.cfg


