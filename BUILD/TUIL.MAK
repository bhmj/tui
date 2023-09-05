.AUTODEPEND

#               *Translator Definitions*
CC = bcc +TUIL.CFG
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
tuidemo.exe: tuil.cfg $(EXE_dependencies)
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
textio.obj: tuil.cfg textio.cpp

tuifiles.obj: tuil.cfg tuifiles.cpp

tuikeys.obj: tuil.cfg tuikeys.cpp

tuimouse.obj: tuil.cfg tuimouse.cpp

tuidate.obj: tuil.cfg tuidate.cpp

tuistr.obj: tuil.cfg tuistr.cpp

tuistd.obj: tuil.cfg tuistd.cpp

tuiext.obj: tuil.cfg tuiext.cpp

tuigraph.obj: tuil.cfg tuigraph.cpp

tuidlgs.obj: tuil.cfg tuidlgs.cpp

tuidemo.obj: tuil.cfg tuidemo.cpp

gmouse.obj: tuil.cfg gmouse.asm
        $(TASM) /MX /M2 /ZI /O GMOUSE.ASM,GMOUSE.OBJ

#               *Compiler Configuration File*
tuil.cfg: tuil.mak
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
| tuil.cfg


