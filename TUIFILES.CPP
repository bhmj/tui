
#include "tuifiles.h"

int  FileExist( char far* fname )
{
  asm {
        push    ds
        mov     ax,3D00h
        lds     dx,fname
        int     21h
        jnc     __ok
        xor     ax,ax
        jmp     __end
      } __ok: asm {
        mov     bx,ax
        mov     ah,3Eh
        int     21h
        mov     ax,1
      } __end: asm {
        pop     ds
  };
  return _AX;
};