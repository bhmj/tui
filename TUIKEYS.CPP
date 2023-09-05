
#define LABEL(x) } x: asm {

int KeyPressed( void )
{
  asm {
    push es
    xor  ax,ax
    mov  es,ax
    mov  bx,[word ptr es:41Ah]
    cmp  bx,[word ptr es:41Ch]
    je   nokey
    inc  ax
  LABEL(nokey)
    pop  es
  };
  return _AX;
}

int ReadKey( void )
{
  asm {
    xor  ax,ax
    int  0x16
  }
  return _AX;
}

void Delay( int ticks )
{
  asm {
        push    es
        mov     ax,0
        mov     es,ax
        mov     bx,46Ch
        mov     dx,word ptr es:bx
      LABEL(__wait)
        mov     ax,word ptr es:bx
        sub     ax,dx
        cmp     ax,ticks
        jl      __wait
        pop     es
  };
}

long GetTimer( void )
{
  long r;
  asm {
        push    es
        mov     ax,0
        mov     es,ax
        mov     bx,46Ch
        mov     ax,word ptr es:bx
        mov     dx,word ptr es:bx+2
        mov     word ptr r,ax
        mov     word ptr r+2,dx
        pop     es
  };
  return r;
};

static unsigned long Seed=1;
static unsigned long RandMul=0x015A4EC3L;
unsigned int Random(unsigned int max)
{
  unsigned long x = Seed*RandMul-1;
  Seed = x;
  return ((unsigned int)(x>>16))%max;
};

#pragma warn -rvl
unsigned long FreeMem(void)
{
  unsigned long xcode;
  asm {
    mov  ah,0x48
    mov  bx,0xFFFF
    int  0x21
    xor  dx,dx
    mov  ax,bx
    shl  ax,1
    rcl  dx,1
    shl  ax,1
    rcl  dx,1
    shl  ax,1
    rcl  dx,1
    shl  ax,1
    rcl  dx,1
    mov  word ptr xcode,ax
    mov  word ptr xcode+2,dx
  };
};
#pragma warn +rvl
