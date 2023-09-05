.AUTODEPEND

#               *Translator Definitions*
CC = bcc +TUIM.CFG
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
tuidemo.exe: tuim.cfg $(EXE_dependencies)
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
textio.obj: tuim.cfg textio.cpp

tuifiles.obj: tuim.cfg tuifiles.cpp

tuikeys.obj: tuim.cfg tuikeys.cpp

tuimouse.obj: tuim.cfg tuimouse.cpp

tuidate.obj: tuim.cfg tuidate.cpp

tuistr.obj: tuim.cfg tuistr.cpp

tuistd.obj: tuim.cfg tuistd.cpp

tuiext.obj: tuim.cfg tuiext.cpp

tuigraph.obj: tuim.cfg tuigraph.cpp

tuidlgs.obj: tuim.cfg tuidlgs.cpp

tuidemo.obj: tuim.cfg tuidemo.cpp

gmouse.obj: tuim.cfg gmouse.asm
        $(TASM) /MX /M2 /ZI /O GMOUSE.ASM,GMOUSE.OBJ

#               *Compiler Configuration File*
tuim.cfg: tuim.mak
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
| tuim.cfg


