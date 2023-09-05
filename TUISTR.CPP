#include "tuistr.h"     // extern "C"

unsigned char UpTable[256] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,      // 00
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,      // 10
  0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,      // 20
  0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,      // 30
  0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,      // 40
  0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,      // 50
  0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
  0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,      // 60
  0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,      // 70
  0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      // 80
  0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      // 90
  0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      // A0
  0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,      // B0
  0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
  0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,      // C0
  0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,      // D0
  0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      // E0
  0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,      // F0
  0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

unsigned char AltWin[128] = {
  0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
  0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
  0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
  0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
  0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
  0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
  0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
  0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
  0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
  0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
  0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
  0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
  0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

unsigned char WinAlt[64] = {
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
  0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
  0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
  0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};

int StrLen( char far* s )
{
  asm {
    push es
    push di
    les  di,s
    mov  bx,di
    xor  ax,ax
    mov  cx,-1
    repnz scasb
    sub  di,bx
    mov  ax,di
    dec  ax
    pop  di
    pop  es
  };
  return _AX;
};

int StrLenC( char far* s )
{
  asm {
    push es
    push di
    les  di,s
    xor  bx,bx
  } __start: asm {
    xor  dx,dx
  } __loop: asm {
    mov  al,[es:di]
    inc  di
  } __cmp: asm {
    or   al,al
    jz   __chk
    cmp  al,0x0A  ;// '\n'
    je   __chk
    cmp  al,''
    je   __loop
    cmp  al,'~'
    je   __loop
    inc  dx
    jmp  __loop
  } __chk: asm {
    cmp  dx,bx
    jl   __inc
    mov  bx,dx
  } __inc: asm {
    xor  dx,dx
    or   al,al
    jz   __end
    mov  al,[es:di]
    inc  di
    jmp  __cmp
  } __end: asm {
    mov  ax,bx
    pop  di
    pop  es
  };
  return _AX;
};

int StrCount( char far* s )
{
  asm {
    push es
    push di
    les  di,s
    xor  bx,bx
    mov  al,[es:di]
    inc  di
    or   al,al
    jz   __end
    inc  bx
  } __loop: asm {
    mov  al,[es:di]
    inc  di
    or   al,al
    jz   __end
    cmp  al,0x0A  ;// '\n'
    jne  __loop
    inc  bx
    jmp  __loop
  } __end: asm {
    mov  ax,bx
    pop  di
    pop  es
  };
  return _AX;
};

int StrCountZ( char far* s )
{
  asm {
    push es
    push di
    les  di,s
    xor  bx,bx  ;// lines
    xor  dx,dx  ;// line len
  } __loop: asm {
    mov  al,[es:di]
    inc  di
    or   al,al
    jnz  __next
    or   dx,dx
    jz   __end
    inc  bx
    xor  dx,dx
    jmp  __loop
  } __next: asm {
    inc  dx
    jmp  __loop
  } __end: asm {
    mov  ax,bx
    pop  di
    pop  es
  };
  return _AX;
};

char StrFindKey( char far* s )
{
  asm {
    push ds
    push si
    lds  si,s
    xor  bx,bx  ;// lines
  } __loop: asm {
    lodsb
    or   al,al
    jz   __end
    cmp  al,'~'
    jne  __loop
    lodsb
    or   al,al
    jz   __end
    mov  bl,al
    cmp  bl,41h
    jb   __end
    cmp  bl,5Ah
    ja   __end
    add  bl,'a'-'A'
  } __end: asm {
    mov  ax,bx
    pop  si
    pop  ds
  };
  return _AX;
};

int StrCmp( char far* str0, char far* str1 )
{
  asm {
        push    ds
        push    si
        push    es
        push    di
        les     di,str0
        mov     bx,di
        mov     cx,0FFFFh
        xor     al,al
        repne   scasb
        sub     di,bx
        mov     bx,di   ;// get str0 length (BX)
        les     di,str1
        mov     dx,di
        mov     cx,0FFFFh
        repne   scasb
        sub     di,dx
        mov     dx,di   ;// get str1 length (DX)
        cmp     bx,dx
        ja      __rlow
        mov     cx,bx
        jmp     __cmp
      } __rlow: asm {
        mov     cx,dx
      } __cmp: asm {
        dec     cx
        lds     si,str0
        les     di,str1
        repe    cmpsb
        ja      __left
        jb      __right
        cmp     dx,bx
        ja      __left
        jb      __right
        xor     ax,ax
        jmp     __end
      } __left: asm {
        mov     ax,1
        jmp     __end
      } __right: asm {
        mov     ax,-1
      } __end: asm {
        pop     di
        pop     es
        pop     si
        pop     ds
  };
  return _AX;
};

char* StriFind( char* buf, char* str )
{
  int i=0;
  int maxlen=StrLen(buf);
  unsigned char *b=(unsigned char*)buf,*s=(unsigned char*)str;
  while ( i<maxlen )
  {
    if ( UpTable[*b] == UpTable[*s] ) {
      b++; s++;
      if ( !(*s) ) return buf;
    } else {
      i = i - (int)( b-(unsigned char*)buf );
      b = ++(unsigned char*)buf;
      s = (unsigned char*)str;
    };
    i++;
  }
  return 0;
};

char* StrUp( char* s )
{
  unsigned char ch;
  while ( *s ) {
    ch=*s;
    if ( (ch>=(unsigned char)'a') && (ch<=(unsigned char)'z') ) ch-=0x20;
    if ( (ch>=(unsigned char)'†') && (ch<=(unsigned char)'Ø') ) ch-=0x20;
    if ( (ch>=(unsigned char)'‡') && (ch<=(unsigned char)'Ô') ) ch-=0x50;
    *s++ = ch;
  };
  return s;
}

char* StrDown( char* s )
{
  unsigned char ch;
  while ( *s ) {
    ch=*s;
    if ( (ch>=(unsigned char)'A') && (ch<=(unsigned char)'Z') ) ch+=0x20;
    if ( (ch>=(unsigned char)'Ä') && (ch<=(unsigned char)'è') ) ch+=0x20;
    if ( (ch>=(unsigned char)'ê') && (ch<=(unsigned char)'ü') ) ch+=0x50;
    *s++ = ch;
  };
  return s;
}

char* WinToAlt(char* s)
{
  byte *tmp = (byte*)s;
  byte c;
  while ( *s )
  {
    c = (byte)*s;
    if ( c >= 128+64 ) *s = WinAlt[c-128-64];
    s++;
  }
  return (char*)tmp;
};

char* AltToWin(char* s)
{
  byte *tmp = (byte*)s;
  byte c;
  while ( *s )
  {
    c = (byte)*s;
    if ( c >= 128 ) *s = AltWin[c-128];
    s++;
  }
  return (char*)tmp;
};

char* StrCpy( char* dest, char* src ) {
#if defined(FARDATA)
  asm {
        push    ds
        push    es
        push    si
        push    di
        les     di,dest
        lds     si,src
      LABEL(__cpy)
        lodsb
        stosb
        or      al,al
        jnz     __cpy
        pop     di
        pop     si
        pop     es
        pop     ds
  };
#elif defined(NEARDATA)
  asm {
        push    si
        push    di
        mov     di,dest
        mov     si,src
      LABEL(__cpy)
        lodsb
        mov     [byte ptr ds:di],al
        inc     di
        or      al,al
        jnz     __cpy
        pop     di
        pop     si
  };
#endif
/*  char* d=dest;
  while ( *dest++ = *src++ ) ;
*/
  return dest;
};

char* StrNCpy( char* dest, char* src, int n ) {
#if defined(FARDATA)
  asm {
        push    ds
        push    es
        push    si
        push    di
        les     di,dest
        lds     si,src
        mov     bx,n
      LABEL(__cpy)
        lodsb
        stosb
        or      al,al
        jz      __end
        dec     bx
        jnz     __cpy
        xor     ax,ax
        stosb
      LABEL(__end)
        pop     di
        pop     si
        pop     es
        pop     ds
  };
#elif defined(NEARDATA)
  asm {
        push    si
        push    di
        mov     di,dest
        mov     si,src
      LABEL(__cpy)
        lodsb
        mov     [byte ptr ds:di],al
        inc     di
        or      al,al
        jnz     __cpy
        pop     di
        pop     si
  };
#endif
/*  char* d=dest;
  while ( *dest++ = *src++ ) ;
*/
  return dest;
};


#pragma warn -pia
char* StrAdd( char* dest, char* src, int n ) {
  char *tmp, *d=dest;
  while ( *dest ) dest++;
  for (int i=0; i<n; i++) {
    tmp = src;
    while ( *dest++ = *tmp++ ) ;
    dest--;
  };
  return d;
}
#pragma warn +pia

char* StrCut( char* dest, char* s, int x0, int x1 )
{
  char* d=dest;
  s+=x0;
  for (int i=0; i<=x1-x0; i++) if ( ! (*dest++ = *s++) ) break;
  *dest = 0;
  return d;
};

char* StrEnter( char* dest, char* s, int cpos, char c, int insmode )
{
  int i;

  StrCpy(dest,s);
  if (!insmode) {
    i=dest[cpos];
    dest[cpos]=c;
    if (!i) dest[cpos+1]=0;
  } else {
    dest[StrLen(dest)+1]=0;
    for (i=StrLen(dest)+1; i>cpos; i--) dest[i]=dest[i-1];
    dest[cpos] = c;
  };
  return dest;
};

char* StrDel( char*dest, char* s, int cpos )
{
  StrCpy( dest, s );
  for(int i=cpos; i<StrLen(s); i++) dest[i]=s[i+1];
  return dest;
};

char* StrBksp( char* dest, char* s, int cpos )
{
  StrCpy( dest, s );
  if (cpos==0) return dest;
  for(int i=cpos-1; i<StrLen(s); i++) dest[i]=s[i+1];
  return dest;
}

char* IntToHex( long num, char* s, int len, char czero )
{
  int  i,n=0;
  char c;

  do {
    c = num&0x000F;
    c = ( c<10 ? c+'0' : c+'A'-0x0A );
    s[n++] = c;
    num >>= 4;
  } while (num);
  while (n<len) s[n++]=czero;
  for (i=0; i<n/2; i++) { c=s[i]; s[i]=s[n-i-1]; s[n-i-1]=c;};
  s[n]='h';
  s[n+1]=0;
  return s;
};

char* IntToDec( long num, char* s, int len, char czero )
{
  long k=num;
  int  i,n=0;
  char c;

  num=Abs(num);
  s[0]=0;
  if (num)
    do {
      s[n++] = (num%10)+'0';
      num /= 10;
    } while (num);
  while (n<len) s[n++]=czero;
  if (k<0) s[n++]='-';
  if (k==0) s[0]='0';
  for (i=0; i<n/2; i++) { c=s[i]; s[i]=s[n-i-1]; s[n-i-1]=c;};
  s[n]=0;
  return s;
};

int  StrToInt( char far* s )    // no leading or trailing spaces allowed!
{
  asm {
        push    es
        push    di
        les     di,dword ptr s
        xor     bx,bx           ;// BX = number
        mov     al,byte ptr es:di
        cmp     al,'-'
        jne     __loop
        mov     bx,-1
        inc     di
      } __loop: asm {
        db      26h,0Fh,0B6h,0Dh ;// movzx cx,byte ptr es:di
        inc     di
        jcxz    __end
        sub     cl,'0'
        mov     ax,bx
        mov     dx,10
        imul    dx              ;// AX = n*10
        add     ax,cx
        mov     bx,ax
        jmp     __loop
      } __end: asm {
        mov     ax,bx
        pop     di
        pop     es
  };
  return _AX;
};
#pragma warn -rvl
long StrToLong( char far* s )    // no leading or trailing spaces allowed!
{
  int nhi=0,nlo=0;
  asm {
        push    es
        push    di
        les     di,dword ptr s
        xor     bx,bx           ;// BX = number
        mov     al,byte ptr es:di
        cmp     al,'-'
        jne     __loop
        inc     di
      } __loop: asm {
        db      26h,0Fh,0B6h,0Dh ;// movzx cx,byte ptr es:di
        inc     di
        jcxz    __end
        sub     cl,'0'
        mov     ax,nhi
        mov     dx,10
        mul    dx              ;// AX = nhi*10
        mov     nhi,ax
        mov     ax,nlo
        mov     dx,10
        mul    dx
        add     nhi,dx
        add     ax,cx
        mov     nlo,ax
        jmp     __loop
      } __end: asm {
        les     di,dword ptr s
        mov     al,byte ptr es:di
        cmp     al,'-'
        jne     __pos
        neg     nlo
        mov     ax,nhi
        or      ax,ax
        jnz     __neg
        inc     nhi
      } __neg: asm {
        neg     nhi
      } __pos: asm {
        mov     ax,nlo
        mov     dx,nhi
        pop     di
        pop     es
  };
};
#pragma warn +rvl
