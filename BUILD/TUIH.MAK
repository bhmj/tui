.AUTODEPEND

#               *Translator Definitions*
CC = bcc +TUIH.CFG
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
tuidemo.exe: tuih.cfg $(EXE_dependencies)
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
textio.obj: tuih.cfg textio.cpp

tuifiles.obj: tuih.cfg tuifiles.cpp

tuikeys.obj: tuih.cfg tuikeys.cpp

tuimouse.obj: tuih.cfg tuimouse.cpp

tuidate.obj: tuih.cfg tuidate.cpp

tuistr.obj: tuih.cfg tuistr.cpp

tuistd.obj: tuih.cfg tuistd.cpp

tuiext.obj: tuih.cfg tuiext.cpp

tuigraph.obj: tuih.cfg tuigraph.cpp

tuidlgs.obj: tuih.cfg tuidlgs.cpp

tuidemo.obj: tuih.cfg tuidemo.cpp

gmouse.obj: tuih.cfg gmouse.asm
        $(TASM) /MX /M2 /ZI /O GMOUSE.ASM,GMOUSE.OBJ

#               *Compiler Configuration File*
tuih.cfg: tuih.mak
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
| tuih.cfg


