#include "textio.h"     // SINGLE,DOUBLE

#define MAKE_FP( s,o ) ( (void*)( (((long)s)<<16)+(((long)o & 0x0000FFFF)) ) )
#define LABEL(x) } x: asm {

// Video
#define VIDEO_SEG  0xB800

// EGA/VGA registers
#define MSLR       0x09        // Max Scan Line Register
#define CSR        0x0A        // Cursor Start Register
#define CER        0x0B        // Cursor End Register
#define SARH       0x0C        // Start Address Register. high byte
#define SARL       0x0D        // Start Address Register, low byte
// Text
char RectStyle[3][8] = {
  {'Ú','¿','À','Ù','³','³','Ä','Ä'},
  {'É','»','È','¼','º','º','Í','Í'},
  {'Û','Û','Û','Û','Û','Û','ß','Ü'}
};

int far YMul40(int);
int far YMul80(int);
int far YMul(int);

int WinPos[16*4];
int WinP=-1;

// hardware
int CrtAddress=0x03D4;
long VideoBuf = (long)MAKE_FP( VIDEO_SEG,0 );
long VideoPtr = VideoBuf;
// video modes
int VideoMode=3;
int TextCols=80;
int TextRows=25;
int ScreenSize=80*25*2;
int (far *YMulFunc)(int) = YMul80;
// video pages
int ActivePage=0;
int VisualPage=0;
int ActPageOff=0;
// output
int WinX=0,WinY=0;      //{ X,Y of left-top corner }
int WinW=80,WinH=80;    //{ actual width and height in symbols }
char TextAttr=0x07;     // default attribute for output
char TextHiAttr=0x0F;   // default highlight attribute for output
                        // ( calculated automatically for current TextAttr;
                        //   for best results use SetHi..Color() functions )
// current params
int CursorShape=CUR_THIN;

int far YMul40(int i)
{
  asm {
    push bx
    push cx
    mov  bx,i
    mov  ax,bx
    mov  cl,6
    shl  ax,cl
    mov  cl,4
    shl  bx,cl
    add  ax,bx
    pop  cx
    pop  bx
  };
  return _AX;
};
int far YMul80(int i)
{
  asm {
    push bx
    push cx
    mov  bx,i
    mov  ax,bx
    mov  cl,7
    shl  ax,cl
    mov  cl,5
    shl  bx,cl
    add  ax,bx
    pop  cx
    pop  bx
  };
  return _AX;
};
int far YMul(int i)
{
  asm {
    push dx
    mov  ax,i
    imul word ptr TextCols
    shl  ax,1
    pop  dx
  }
  return _AX;
};

void GetModeStats( void )
{
  TextCols = *(int far*)MAKE_FP(0x0000,0x044A);
  TextRows = (*(char far*)MAKE_FP(0x0000,0x0484)) + 1;
  ScreenSize = TextRows*TextCols*2;
  switch( TextCols )
  {
    case 40: YMulFunc = YMul40; break;
    case 80: YMulFunc = YMul80; break;
    default: YMulFunc = YMul; break;
  };
};

void SetVideoMode( int m )
{
  asm {
    mov  ax,m
    mov  ah,0
    int  10h
  };
  VideoMode = m;
  SetActivePage( 0 );
  SetVisualPage( 0 );
  GetModeStats();
};

int  GetVideoMode( void )
{
  asm {
    mov  ax,VideoMode
    cmp  ax,0
    jne  __exit
    push si
    push di
    mov  cx,sp
    mov  dx,bp
    mov  ah,0Fh
    int  10h
    mov  bp,dx
    mov  sp,cx
    pop  di
    pop  si
    mov  ah,0
    mov  VideoMode,ax
    LABEL(__exit)
  };
  return _AX;
};

int GetMaxCol(void) { return TextCols-1; };
int GetMaxRow(void) { return TextRows-1; };

void WaitRetrace(void)
{
  asm {
    push dx
    mov  dx,CrtAddress
    add  dl,6
  LABEL(__wait_off)
    in   al,dx
    test al,8
    jnz  __wait_off
  LABEL(__wait_on)
    in   al,dx
    test al,8
    jz   __wait_on
    pop  dx
  };
};

void SetActivePage(int p)
{
  ActivePage = p;
  VideoPtr = VideoBuf + ActivePage*ScreenSize;
};

void SetVisualPage(int p)
{
  asm {
    mov  ax,p
    mov  VisualPage,ax
    mul  word ptr ScreenSize
    mov  bx,ax
    shr  bx,1
    cli
    mov  dx,CrtAddress
    mov  al,SARL
    out  dx,al
    inc  dx
    mov  al,bl
    out  dx,al
    dec  dx
    mov  al,SARH
    out  dx,al
    inc  dx
    mov  al,bh
    out  dx,al
    sti
  };
};

int GetActivePage(void) { return ActivePage; };
int GetVisualPage(void) { return VisualPage; };

void CopyPage(int pdest, int psrc)
{
  long fromptr = VideoBuf + psrc*ScreenSize;
  long destptr = VideoBuf + pdest*ScreenSize;
  asm {
    push ds
    push si
    push es
    push di
    mov  di,word ptr destptr
    mov  ax,word ptr destptr+2
    mov  es,ax
    mov  cx,ScreenSize
    mov  si,word ptr fromptr
    mov  ax,word ptr fromptr+2
    mov  ds,ax
    shr  cx,1
    rep  movsw
    pop  di
    pop  es
    pop  si
    pop  ds
  };
};

void TextInit(void)
{
  CrtAddress = *(int far*)MAKE_FP(0x0000,0x0463);
  VideoMode = GetVideoMode();
  GetModeStats();
  SetActivePage( 0 );
  SetVisualPage( 0 );
  TextAttr = 0x07;
  TextAttr = 0x0F;
  ResetWindow();
};

void SetTextColor( char c ) {
  TextAttr = (TextAttr & 0xF0) + (c & 0x0F);
  TextHiAttr = (TextAttr & 0xF0) + ( (15-(TextAttr & 0x0F)-1) & 0x0F );
};
void SetBackColor( char c ) {
  TextAttr = (TextAttr & 0x0F) + ((c & 0x0F) << 4);
  TextHiAttr = (TextAttr & 0xF0) + ( (15-(TextAttr & 0x0F)-1) & 0x0F );
};
void SetAttr( char a ) { TextAttr = a; TextHiAttr = (a & 0xF0) + ( (15-(a & 0x0F)-1) & 0x0F ); };
void SetHiTextColor( char c ) { TextHiAttr = (TextHiAttr & 0xF0) + (c & 0x0F); };
void SetHiBackColor( char c ) { TextHiAttr = (TextHiAttr & 0x0F) + ((c & 0x0F) << 4); };
void SetHiAttr( char a ) { TextHiAttr = a; };

void GetWindow(int& x, int& y, int& w, int& h)
{
  x=WinX; y=WinY; w=WinW; h=WinH;
};

void SetWindow(int x, int y, int w, int h)
{
  if (x+w-1 >= TextCols) w=TextCols-x;
  if (y+h-1 >= TextRows) h=TextRows-y;
  WinX = x;
  WinY = y;
  WinH = h;
  WinW = w;
};

void SaveWindowPos( void )
{
  WinP++;
  WinPos[WinP*4+0] = WinX;
  WinPos[WinP*4+1] = WinY;
  WinPos[WinP*4+2] = WinW;
  WinPos[WinP*4+3] = WinH;
};

void RestWindowPos( void )
{
  if (WinP < 0) return;
  WinX = WinPos[WinP*4+0];
  WinY = WinPos[WinP*4+1];
  WinW = WinPos[WinP*4+2];
  WinH = WinPos[WinP*4+3];
  WinP--;
};

void SetPrevWindow( void )
{
  if (WinP < 0) return;
  WinX = WinPos[WinP*4+0];
  WinY = WinPos[WinP*4+1];
  WinW = WinPos[WinP*4+2];
  WinH = WinPos[WinP*4+3];
};

void ResetWindow(void)
{
  WinX  = 0;
  WinY  = 0;
  WinW  = TextCols;
  WinH  = TextRows;
};

void SetVideoBuf( void far* ptr ) { VideoPtr=(long)ptr; };
void GetVideoBuf( long* ptr ) { *ptr = (long)VideoPtr; };

void ClearScreen(char c)
{
  asm {
    push es
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax
    mov  di,word ptr VideoPtr
    mov  cx,ScreenSize
    shr  cx,1
    mov  al,c
    mov  ah,TextAttr
    cld
    rep  stosw
    pop  di
    pop  es
  };
};

void ClearWindow(char c)
{
  asm {
    push es
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax
    mov  di,word ptr VideoPtr
    push word ptr WinY
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax
    mov  ax,WinX
    shl  ax,1
    add  di,ax            ;//{ ES:DI = x0:y0 }
    mov  dx,WinH          ;//{ DX = rows }
    or   dx,dx
    jz   __end
    mov  cx,WinW          ;//{ CX = columns }
    jcxz __end
    mov  bx,TextCols
    sub  bx,WinW
    shl  bx,1             ;//{ BX = line-to-line step }
    mov  ah,TextAttr
    mov  al,c             ;//{ AX = char/attr }
    cld
  LABEL(__yloop)
    push cx
    rep  stosw
    add  di,bx
    pop  cx
    dec  dx
    jnz  __yloop
  LABEL(__end)
    pop  di
    pop  es
  };
};

int  GetWindowSize( int w, int h ) {  return w*h*2 + 4; };

void SaveWindow( int x, int y, int w, int h, char far* p )
{
  asm {
    push ds
    push es
    push si
    push di

    cld
    les  di,dword ptr p   ;//{ ES:DI = buffer address }
    mov  cx,w             ;//{ CX = width }
    mov  dx,h             ;//{ DX = height }
    mov  ax,cx
    stosw
    mov  ax,dx
    stosw                 ;//{ store region size }
    mov  bx,TextCols
    sub  bx,cx
    shl  bx,1             ;//{ BX = line-to-line step }
    mov  si,word ptr VideoPtr
    push y
    call dword ptr YMulFunc
    add  sp,2
    add  si,ax
    mov  ax,x
    shl  ax,1
    add  si,ax            ;//{ SI = screen offset }
    mov  ax,word ptr VideoPtr+2
    mov  ds,ax            ;//{ DS = screen segment }
  LABEL(__ready)
    push cx
    rep  movsw
    pop  cx
    add  si,bx
    dec  dx
    jnz  __ready
    pop  di
    pop  si
    pop  es
    pop  ds
  };
};

void RestoreWindow( int x, int y, char far* p )
{
  asm {
    push ds
    push es
    push si
    push di

    mov  bx,TextCols
    mov  di,word ptr VideoPtr
    push y
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax
    mov  ax,x
    shl  ax,1
    add  di,ax            ;//{ DI = screen offset }
    mov  ax,word ptr VideoPtr+2
    mov  es,ax            ;//{ ES = screen segment }
    cld
    lds  si,dword ptr p   ;//{ DS:SI = buffer address }
    lodsw
    mov  cx,ax            ;//{ CX = width }
    lodsw
    mov  dx,ax            ;//{ DX = height }
    sub  bx,cx
    shl  bx,1             ;//{ BX = line-to-line step }
  LABEL(__ready)
    push cx
    rep  movsw
    pop  cx
    add  di,bx
    dec  dx
    jnz  __ready
    pop  di
    pop  si
    pop  es
    pop  ds
  };
};

void PasteBufBuf( int x, int y, int w, int h, char far* p0, int tx, int ty, char far* p1 )
{
  int p0w,p1w;
  int off0,off1;
  int step0,step1;

  p0w = *(int*)(p0);
  p1w = *(int*)(p1);
  off0 = ((y*p0w+x) << 1) + 4;
  off1 = ((ty*p1w+tx) << 1) + 4;
  step0 = (p0w-w) << 1;
  step1 = (p1w-w) << 1;
  asm {
    push ds
    push es
    push si
    push di
    lds  si,dword ptr p0
    les  di,dword ptr p1
    add  si,off0
    add  di,off1
    mov  cx,w
    mov  dx,h
    mov  ax,step0
    mov  bx,step1
    cld
  LABEL(__yloop)
    push cx
    rep  movsw
    pop  cx
    add  si,ax
    add  di,bx
    dec  dx
    jnz  __yloop
    pop  di
    pop  si
    pop  es
    pop  ds
  };
};

void PasteBufWindow( int x, int y, int w, int h, int tx, int ty, char far* p )
{
  int pw,off,step;

  pw    = *(int*)p;
  off   = ((pw*y+x) << 1) + 4;
  step  = pw-w;
  asm {
    push ds
    push es
    push si
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax
    mov  di,word ptr VideoPtr   ;//{ ES:DI = page }
    push ty
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax
    mov  ax,tx
    shl  ax,1
    add  di,ax                  ;//{ ES:DI = screen position = (TX,TY) }
    mov  ax,step
    shl  ax,1                   ;//{ AX = buf step }
    mov  bx,TextCols
    sub  bx,w
    shl  bx,1                   ;//{ BX = screen step }
    mov  cx,w                   ;//{ CX = width }
    mov  dx,h                   ;//{ DX = height }
    lds  si,dword ptr p
    add  si,off                 ;//{ DS:SI = buffer }
  LABEL(__yloop)
    push cx
    rep  movsw
    pop  cx
    add  si,ax
    add  di,bx
    dec  dx
    jnz  __yloop
    pop  di
    pop  si
    pop  es
    pop  ds
  };
};

void Rectangle( int x, int y, int w, int h, int style )
{
  asm {
    cmp  style,2          ;//{ if (style > 2) then style:=0; }
    jbe  __styleok
    mov  style,0
  LABEL(__styleok)
    cmp  w,2
    jb   __exit0          ;//{ if (rectwidth < 2) then exit }
    cmp  h,2
    jb   __exit0          ;//{ if (rectheight < 2) then exit }
    mov  ax,x
    add  ax,w
    cmp  ax,WinW
    ja   __exit0          ;//{ if (x+w > WinW) then exit }
    mov  ax,y
    add  ax,h
    cmp  ax,WinH
    ja   __exit0          ;//{ if (y+h >= WinH) then exit }
    jmp  __rect
  }
  __exit0: goto __exit;
  __rect:
  asm {
    push es
    push di
    push si

    mov  cx,w             ;//{ CX = width }
    mov  dx,h             ;//{ DX = height }
    dec  dx
    dec  dx               ;//{ DX = number of vertical sections }
    mov  si,TextCols
    sub  si,cx
    shl  si,1             ;//{ SI = line-to-line step }
    dec  cx
    dec  cx               ;//{ CX = number of horizontal sections }

    mov  ax,word ptr VideoPtr+2
    mov  es,ax            ;//{ ES = video segment }
    mov  di,word ptr VideoPtr ;//{ DI = current page }
    mov  ax,y
    add  ax,WinY
    push ax
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax
    mov  ax,x
    add  ax,WinX
    shl  ax,1
    add  di,ax            ;//{ DI = left top corner }
    mov  bx,style
    shl  bx,1
    shl  bx,1
    shl  bx,1             ;//{ BX = style index }
    mov  al,byte ptr RectStyle+bx+0
    mov  ah,TextAttr
    cld
    stosw                 ;//{ write left top corner }
    mov  al,byte ptr RectStyle+bx+6
    or   cx,cx
    jz   __nohorz1
    push cx
    rep  stosw
    pop  cx
  LABEL(__nohorz1)
    mov  al,byte ptr RectStyle+bx+1
    stosw
    add  di,si
    or   dx,dx
    jz   __horz2
  LABEL(__vert)
    mov  al,byte ptr RectStyle+bx+4
    stosw
    add  di,cx
    add  di,cx
    mov  al,byte ptr RectStyle+bx+5
    stosw
    add  di,si
    dec  dx
    jnz  __vert
  LABEL(__horz2)
    mov  al,byte ptr RectStyle+bx+2
    stosw
    or   cx,cx
    jz   __nohorz2
    mov  al,byte ptr RectStyle+bx+7
    rep  stosw
  LABEL(__nohorz2)
    mov  al,byte ptr RectStyle+bx+3
    stosw
    pop  di
    pop  di
    pop  es
  LABEL(__exit)
    nop
  };
};

void WriteXYC( int x, int y, char c, char far* s)
// supports ~ for single highlight,  for multiple highlight
// e.g. ~Window ('W' highlighted), Line one ('one' highlighted)
// or : Testi~fy (all string highlighted except 't')
{
  asm {
    mov  ax,y
    cmp  ax,WinH
    ja   __exit
    mov  ax,x
    cmp  ax,WinW
    ja   __exit
    push ds
    push es
    push si
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax            ;//{ ES = video segment }
    mov  di,word ptr VideoPtr ;//{ DI = current page }
    mov  ax,y
    add  ax,WinY
    push ax
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax            ;//{ DI = Page + Y*ScrWidth }
    mov  ax,x
    add  ax,WinX
    shl  ax,1
    add  di,ax            ;//{ DI = Page + Y*ScrWidth + X }
    mov  bx,WinW
    sub  bx,x             ;//{ BX = max string length }
    mov  dh,TextHiAttr    ;//{ DL = normal attrib }
    mov  dl,c             ;//{ DH = highlight attrib }
    lds  si,dword ptr s
    cld
    xor  cx,cx
  LABEL(__write)
    mov  ah,dl
    lodsb
    or   al,al
    jz   __done
    cmp  al,''
    jne  __tag
    lodsb
    or   al,al
    jz   __done
    xchg dh,dl
    mov  ah,dl
    jmp  __norm
  LABEL(__tag)
    cmp  al,'~'
    jne  __norm
    lodsb
    or   al,al
    jz   __done
    mov  ah,dh
  LABEL(__norm)
    stosw
    inc  cx
    cmp  cx,bx
    jb   __write
  LABEL(__done)
    pop  di
    pop  si
    pop  es
    pop  ds
  LABEL(__exit)
  };
};

void WriteXY( int x, int y, char far* s )
{
  WriteXYC( x, y, TextAttr, s );
};

void SpawnXYC( int x, int y, char c, char s, int len )
{
  asm {
    mov  ax,y
    cmp  ax,WinH
    ja   __exit
    mov  ax,x
    cmp  ax,WinW
    ja   __exit
    cmp  len,0
    je   __exit
    push ds
    push es
    push si
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax            ;//{ ES = video segment }
    mov  di,word ptr VideoPtr ;//{ DI = current page }
    mov  ax,y
    add  ax,WinY
    push ax
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax            ;//{ DI = Page + Y*ScrWidth }
    mov  ax,x
    add  ax,WinX
    shl  ax,1
    add  di,ax            ;//{ DI = Page + Y*ScrWidth + X }
    mov  bx,WinW
    sub  bx,x             ;//{ BX = max string length }
    mov  cx,len
    cmp  cx,bx
    jb   __normlen
    mov  cx,bx            ;//{ CX = output length }
  LABEL(__normlen)
    mov  ah,c
    mov  al,s
    rep  stosw
    pop  di
    pop  si
    pop  es
    pop  ds
  LABEL(__exit)
  };
};

void AttrXY( int x, int y, char c, int len )
{
  asm {
    mov  ax,y
    cmp  ax,WinH
    ja   __exit
    mov  ax,x
    cmp  ax,WinW
    ja   __exit
    cmp  len,0
    je   __exit
    push ds
    push es
    push si
    push di
    mov  ax,word ptr VideoPtr+2
    mov  es,ax            ;//{ ES = video segment }
    mov  di,word ptr VideoPtr ;//{ DI = current page }
    mov  ax,y
    add  ax,WinY
    push ax
    call dword ptr YMulFunc
    add  sp,2
    add  di,ax            ;//{ DI = Page + Y*ScrWidth }
    mov  ax,x
    add  ax,WinX
    shl  ax,1
    add  di,ax            ;//{ DI = Page + Y*ScrWidth + X }
    mov  bx,WinW
    sub  bx,x             ;//{ BX = max string length }
    mov  cx,len
    cmp  cx,bx
    jb   __normlen
    mov  cx,bx
  LABEL(__normlen)         //{ CX = output length }
    jcxz __end
    mov  bh,c
    cld
  LABEL(__write)
    mov  ax,word ptr es:di
    mov  ah,bh
    stosw
    loop __write
  LABEL(__end)
    pop  di
    pop  si
    pop  es
    pop  ds
  LABEL(__exit)
  }
};

void CursorXY( int x, int y )
{
  asm {
    mov  ax,ActivePage
    mov  ah,02h
    mov  dx,x
    add  dx,WinX
    mov  bx,y
    add  bx,WinY
    mov  dh,bl
    push si
    push di
    push bp
    mov  cx,sp
    int  10h
    mov  sp,cx
    pop  bp
    pop  di
    pop  si
  }
};

int GetCursorX(void)
{
  asm {
    mov  bx,ActivePage
    mov  bh,bl
    mov  ah,3
    int  10h
    xor  ax,ax
    mov  al,dl
    sub  ax,WinX
  }
  return _AX;
};

int GetCursorY(void)
{
  asm {
    mov  bx,ActivePage
    mov  bh,bl
    mov  ah,3
    int  10h
    xor  ax,ax
    mov  al,dh
    sub  ax,WinY
  };
  return _AX;
};

int GetCursorShape( void ) { return CursorShape; };

void SetCursor( int x )
{
  asm {
    mov  dx,CrtAddress
    push es
    mov  ax,0
    mov  es,ax
    mov  ax,word ptr es:0485h
    pop  es
    dec  ax
    push ax               ;//{ Get character height -1 }
    mov  ax,x
    mov  CursorShape,ax
    cmp  ax,CUR_OFF
    je   __off
    cmp  ax,CUR_ON
    je   __thin
    cmp  ax,CUR_THIN
    je   __thin
    cmp  ax,CUR_BLOCK
    je   __block
  LABEL(__thin)
    pop  cx
    mov  bl,cl            ;//{ BX = height-2 }
    dec  bl               ;//{ CX = height }
    dec  bl
    jmp  __set
  LABEL(__block)
    mov  bl,0             ;//{ BX = 1 }
    pop  cx               ;//{ CX = height }
    jmp  __set
  LABEL(__off)
    pop  bx
    inc  bl               ;//{ BX = no_line }
    mov  cl,bl            ;//{ CX = no_line }
    jmp  __set            ;
    mov  ax,ActivePage
    mov  ah,02h
    mov  bh,0
    mov  dh,-1
    mov  dl,-1
    push si
    push di
    push bp
    mov  cx,sp
    int  10h
    mov  sp,cx
    pop  bp
    pop  di
    pop  si
    jmp  __end
  LABEL(__set)
    mov  al,CSR
    out  dx,al
    inc  dx
    mov  al,bl
    out  dx,al
    dec  dx
    mov  al,CER
    out  dx,al
    inc  dx
    mov  al,cl
    out  dx,al
  LABEL(__end)
  };
};
