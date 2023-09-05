//
// Textmode graphics
// (C) BlockHead aka Guru
//
// How to use:
//
//   SetGrWindow( xpos, ypos, color );
//     xpos, ypos = text coordinates of a 32x8 window
//     color      = initial window attribute (back/text)
//     returns the height of the graphics window:
//       standard VGA (80x25): 128
//       standard EGA (80x25): 112
//       high-res modes (80x43, 80x50 and above): 64
//
//   drawing:
//     ClearGrBuffer()
//       erase all pixels (but not colors!) in buffer
//     GrPixel / GrLine / GrCircle (coordiantes, color)
//       coordinates are: x in [0..255], y in [0..127 | 111 | 63]
//       color is a back/face color, shown on the screen immediately
//     GrWaitRetrace - you know
//     SwapGrBuffer()
//       copy pixels from buffer to screen
//     ClearGrWindow(color)
//       use it if you want to reset screen colors of the buffer
//   loop drawing
//
//   HideGrWindow
//     restore refresh rate, character tables, screen attributes
//     does not restore graphics window area
//
// THAT'S ALL!

//#define GRAPH_DEMO    // uncomment to compile as exe

#include "tuigraph.h"

byte far *txbuf=0;      // screen attributes buffer
byte far *grbuf=0;      // graphics buffer
int  pageoff,pagesize,winoff;   // page offset, page size, graphics offset
int  grcharheight;      // character height
int  grH;               // graphics window height in pixels

int  grTextCols(void) { return *(int far*)MAKE_FP(0x0000,0x044A); };
int  grTextRows(void) { return (*(char far*)MAKE_FP(0x0000,0x0484)) + 1; };
byte grGetCharHeight(void) {
  asm {
    push es; push bp;
    mov ax,0x1130; mov bh,0; int 0x10;
    pop bp; pop es;
  };
  return _CL;
};
void grCursorOff(void)
{
  asm {
        mov     dx,0x3D4
        mov     al,0x0a
        out     dx,al
        mov     dx,0x3D5
        in      al,dx
        or      al,32
        out     dx,al
  };
};
void grCursorOn(void)
{
  asm {
        mov     dx,0x3D4
        mov     al,0x0A
        out     dx,al
        mov     dx,0x3D5
        in      al,dx
        and     al,not(32)
        out     dx,al
  };
};

// save current page
void grSavePage(byte far* buf, int offs, int len)
{
  asm {
        push   ds
        push   es
        push   si
        push   di
        les    di,buf       ;//{ ES:DI = attribute buffer }
        mov    si,offs      ;//{ active page offset }
        shl    si,1
        inc    si
        mov    ax,0xB800
        mov    ds,ax        ;//{ DS:SI = source }
        mov    cx,len
      } _loop: asm {
        lodsb
        inc    si
        stosb
        loop   _loop
        pop    di
        pop    si
        pop    es
        pop    ds
  };
};

// restore current page
void grRestPage(byte far* buf, int offs, int len)
{
  asm {
        push   ds
        push   es
        push   si
        push   di
        mov    di,offs      ;//{ active page offset }
        shl    di,1
        inc    di
        mov    ax,0xB800
        mov    es,ax        ;//{ ES:DI = source }
        lds    si,buf       ;//{ DS:SI = attribute buffer }
        mov    cx,len
      } _loop: asm {
        lodsb
        stosb
        inc    di
        loop   _loop
        pop    di
        pop    si
        pop    es
        pop    ds
  };
};

// prepage screen
// convert all high-intensity symbols to low-intensity
void grPrepare(int offs, int len)
{
  asm {
        push    es
        push    di
        mov     di,offs
        shl     di,1
        mov     ax,0B800h
        mov     es,ax
        mov     cx,len
      } _loop: asm {
        mov     ax,word ptr es:di
        mov     bh,ah   ;//{ BH = back }
        mov     bl,ah   ;//{ BL = face }
        and     bl,0Fh
        cmp     bl,08h
        jb      _write
        shr     bh,4
        sub     bl,8
        cmp     bl,bh
        jne     _back
        inc     bl
        and     bl,07h
      } _back: asm {
        shl     bh,4
        add     bh,bl
        mov     ah,bh
      } _write: asm {
        stosw
        loop    _loop
        pop     di
        pop     es
  };
};

#define XPORT_OUT(p,i,v) asm { mov dx,p; mov al,i; out dx,al; inc dx; mov al,v; out dx,al; };
#define XPORT_IN(p,i,r)  asm { mov dx,p; mov al,i; out dx,al; inc dx; in al,dx; mov r,al; };

// 0 = CGA or below
// 1 = EGA
// 2 = VGA or above
enum { CGA,EGA,VGA };
int AdapterType(void)
{
  int x;
  asm {
        mov     ax,1A00h
        int     10h
        cmp     al,01Ah
        jne     __cga
        cmp     bl,2
        jbe     __cga
        cmp     bl,5
        jbe     __ega
        jmp     __vga
      LABEL(__cga)
        mov     x,0
        jmp     __end
      LABEL(__ega)
        mov     x,1
        jmp     __end
      LABEL(__vga)
        mov     x,2
        jmp     __end
      LABEL(__end)
  };
  return x;
};
void grSetCharWidth8(void)
{
  byte c;
  asm {
        //{ change to 640 horz res }
        mov     dx,03CCh
        in      al,dx
        and     al,not(4 or 8)
        mov     dx,03C2h
        out     dx,al
  };
  XPORT_OUT(0x03C4,0,0);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  XPORT_IN (0x03C4,1,c);
  c|=1; XPORT_OUT(0x03C4,1,c);  // switch to 8 pixel wide fonts
  XPORT_OUT(0x03C4,0,0);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  asm {
        //{ Center Screen }
        mov     dx,03DAh
        in      al,dx
        mov     dx,03C0h
        mov     al,013h or 32
        out     dx,al
        mov     al,0
        out     dx,al
  };
};

void grSetCharWidth9(void)
{
  byte c;
  asm {
        //{ change to 720 horz res }
        mov     dx,03CCh
        in      al,dx
        or      al,4
        mov     dx,03C2h
        out     dx,al
  };
  XPORT_OUT(0x03C4,0,0);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  XPORT_IN (0x03C4,1,c);
  c&=0xFE; XPORT_OUT(0x03C4,1,c);  // switch to 9 pixel wide fonts
  XPORT_OUT(0x03C4,0,0);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  asm {
        //{ Center Screen }
        mov     dx,03DAh        // reset attribute controller trigger
        in      al,dx
        mov     dx,03C0h        //
        mov     al,013h or 32
        out     dx,al
        mov     al,-1
        out     dx,al
  };
};

void grFontMemory(void)
{
  asm cli;
  XPORT_OUT(0x03C4,0,1);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  XPORT_OUT(0x03C4,4,7);        // change from odd/even addressing to linear
  XPORT_OUT(0x03C4,2,4);        // switch write access to plane 2
  XPORT_OUT(0x03CE,4,2);        // set read map reg to plane 2
  XPORT_OUT(0x03CE,5,0);        // set graphics mode reg
  XPORT_OUT(0x03CE,6,12);       // set misc. reg
};
void grTextMemory(void)
{
  XPORT_OUT(0x03C4,0,1);        // turn off sequence controller
  XPORT_OUT(0x03C4,0,3);        // reset sequence controller
  XPORT_OUT(0x03C4,4,3);        // change to odd/even addressing
  XPORT_OUT(0x03C4,2,3);        // switch write access
  XPORT_OUT(0x03CE,4,0);        // set read map reg
  XPORT_OUT(0x03CE,5,0x10);     // set graphics mode reg
  XPORT_OUT(0x03CE,6,14);       // set misc. reg
  asm sti;
};
void grSetDualFonts(void)
{
  asm {
        //{ set fonts 0 & 1 }
        mov     bl,4
        mov     ax,1103h
        int     10h
  };
};

void grResetDualFonts(void)
{
  asm {
        //{ set fonts 0 & 1 }
        mov     bl,0
        mov     ax,1103h
        int     10h
  };
};

// Clear graphics window
void ClearGrWindow(byte color)
{
  int i,ix,iy,stx;
  word far* p = (word far*)MAKE_FP(0xB800,winoff);

  color |= 0x08; // for second char table
  stx = grTextCols()-32;
  i = 0;
  for (iy=0; iy<8; iy++) {
    for (ix=0; ix<32; ix++) *p++ = (((word)color)<<8)+(i++);
    p += stx;
  };
};

// Show graphics window ( 32x8 symbols, 256x128 | 256x64 pixels)
//   allocate memory for attribute buffer
//   save screen attributes
//   prepare screen
//   get symbol height data
//   turn cursor off
//   set up controller
//   draw window
//   return window height in pixels
int  ShowGrWindow(int xpos, int ypos, byte color)
{
  if (!txbuf) {
    pagesize = grTextCols()*grTextRows();
    txbuf = new byte[pagesize];
    if (txbuf) {
      asm mov ah,0Fh;
      asm int 10h;
      asm mov bl,bh
      asm mov bh,0
      pageoff = _BX * pagesize;
      grSavePage(txbuf,pageoff,pagesize);
    };
  };
  if (!grbuf) grbuf = new byte[256*32];
  winoff = pageoff+(xpos+ypos*grTextCols())*2;
  grPrepare(pageoff,pagesize);
  grCursorOff();
  grcharheight = grGetCharHeight();
  if (AdapterType()==VGA) { GrWaitRetrace(); grSetCharWidth8(); };
  GrWaitRetrace();
  grSetDualFonts();
  ClearGrBuffer();
  ClearGrWindow(color);
  SwapGrBuffer();
  grH = grcharheight*8;
  return grH;
};
// Hide graphics window
//   reset controller
//   turn cursor on
//   restore screen attributes
void HideGrWindow(void)
{
  GrWaitRetrace();
  if (AdapterType()==VGA) { GrWaitRetrace(); grSetCharWidth9(); };
  grResetDualFonts();
  if (txbuf) {
    grRestPage(txbuf,pageoff,pagesize);
    delete txbuf;
    txbuf=0;
  };
  if (grbuf) { delete grbuf; grbuf=0; };
  grCursorOn();
};
// Graphics primitives (no swapping)
void GrPixel(int x, int y, byte color)
{
  word rx,ry;
  word ydiv;
  byte b;

  if (x<0 || x>=256) return;
  if (y<0 || y>=grH) return;

  // text color
  if (color) {
    ydiv = y/grcharheight;
    *(byte far*)(0xB8000000L+winoff+((x>>2)&0xFE)+(ydiv<<7)+(ydiv<<5)+1) = color|0x08;
  };

  // pixel
  rx = (x&0xF8)<<2;
  ry = (y%grcharheight) + (y/grcharheight)*32*32;
  b  = 0x80>>(x&0x07);
  if (color&0x0F) *(grbuf+rx+ry) |= b;
  else            *(grbuf+rx+ry) &= ~b;
};
byte grpixcolor;
void CircleSymmetry(int xc, int yc, int x, int y)
{
  GrPixel( x+xc,  y+yc, grpixcolor );
  GrPixel( x+xc, -y+yc, grpixcolor );
  GrPixel(-x+xc, -y+yc, grpixcolor );
  GrPixel(-x+xc,  y+yc, grpixcolor );
  GrPixel( y+xc,  x+yc, grpixcolor );
  GrPixel( y+xc, -x+yc, grpixcolor );
  GrPixel(-y+xc, -x+yc, grpixcolor );
  GrPixel(-y+xc,  x+yc, grpixcolor );
};
void GrCircle(int cx, int cy, int r, byte color)
{
  int d,x,y;

  grpixcolor=color;
  x=0;
  y=(r>0?r:-r);
  d=3-2*y;
  while(x < y)
  {
    CircleSymmetry( cx, cy, x, y );
    if (d < 0) d += (4*x)+6;
    else {
      d += 4*(x-y)+10;
      y--;
    };
    x++;
  };
  if (x == y) CircleSymmetry( cx, cy, x, y );
};
void GrLine(int x0, int y0, int x1, int y1, byte color)
{
  int i,dx,dy;
  long ml,c;

  dx = x1-x0;
  dy = y1-y0;
  if (Abs(dx)>Abs(dy))
  {
    if (!dx) return;
    if (dx<0) { x0^=x1; x1^=x0; x0^=x1; y0^=y1; y1^=y0; y0^=y1; dx=-dx; dy=-dy; };
    ml = (((long)dy)<<16)/dx;
    c = ((long)y0)<<16;
    for (i=x0; i<=x1; i++) {
      GrPixel(i,(sword)(c>>16),color);
      c+=ml;
    };
  } else {
    if (!dy) return;
    if (dy<0) { x0^=x1; x1^=x0; x0^=x1; y0^=y1; y1^=y0; y0^=y1; dx=-dx; dy=-dy; };
    ml = (((long)dx)<<16)/dy;
    c = ((long)x0)<<16;
    for (i=y0; i<=y1; i++) {
      GrPixel((sword)(c>>16),i,color);
      c+=ml;
    };
  };
};
void ClearGrBuffer(void)
{
  asm {
        push    es
        push    di
        les     di,grbuf
        mov     cx,256*32/2
        xor     ax,ax
        rep     stosw
        pop     di
        pop     es
  };
};
// Swap graphics buffer to the screen
void SwapGrBuffer(void)
{
  grFontMemory();
  asm {
        push    ds
        push    es
        push    si
        push    di
        mov     ax,0xB800
        mov     es,ax
        mov     di,0x4000
        lds     si,grbuf
        mov     cx,256*32
        rep     movsb
        pop     di
        pop     si
        pop     es
        pop     ds
  };
  grTextMemory();
};

void GrWaitRetrace(void)
{
  asm {
    push dx
    mov  dx,0x3D4
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

#ifdef GRAPH_DEMO
// DEMO =======================================================

#include "tuikeys.cpp"
#include <string.h>

#define MAX_POINTS      8
#define MAX_TRAILS      8
void main(void)
{
  int  i,p;     // loop
  int  winH;
  int  x[MAX_TRAILS][MAX_POINTS], y[MAX_TRAILS][MAX_POINTS];
  int  dx[MAX_POINTS], dy[MAX_POINTS];
  int  lnum,pnum,pt,ptnext,ptprev;
  byte color;
  byte buf[80L*50*2];

  // save entire screen
  _fmemcpy((char far*)buf,MAKE_FP(0xB800,0),grTextCols()*grTextRows()*2);

  winH=ShowGrWindow(24,8,color);

  for (i=0; i<MAX_TRAILS; i++)
    for (p=0; p<MAX_POINTS; p++)
    {
      x[i][p]=-1;
      y[i][p]=-1;
    }
  for (i=0; i<MAX_POINTS; i++)
  {
    x[0][i]=Random(256);
    y[0][i]=Random(winH);
    dx[i]=Random(4)+1;
    dy[i]=Random(3)+1;
  };
  lnum = 3;     // number of lines
  pnum = 5;     // number of trails
  color=5; // black, light magenta

  ptprev = 0;
  pt     = 1;
  while (!KeyPressed())
  {
//    ClearGrBuffer();

    // erase last line in trail
    for (p=0; p<lnum-1; p++) GrLine(x[pt][p],y[pt][p],x[pt][p+1],y[pt][p+1],0);
    GrLine(x[pt][p],y[pt][p],x[pt][0],y[pt][0],0);
    // move points
    for (p=0; p<lnum; p++) {
      if (x[ptprev][p] >= 256 || x[ptprev][p] <= 0) { dx[p]=-dx[p]; if (x[ptprev][p]<=0) color=(color+1)&0x0F; };
      if (y[ptprev][p] >= winH || y[ptprev][p] <= 0) dy[p]=-dy[p];
      x[pt][p] = x[ptprev][p]+dx[p];
      y[pt][p] = y[ptprev][p]+dy[p];
    };
    // draw the line
    for (p=0; p<lnum-1; p++) GrLine(x[pt][p],y[pt][p],x[pt][p+1],y[pt][p+1],color);
    GrLine(x[pt][p],y[pt][p],x[pt][0],y[pt][0],color);
    // next line
    ptprev = pt;
    pt = (pt+1==pnum?0:pt+1);

    GrWaitRetrace();
    SwapGrBuffer();
  };
  ReadKey();

  HideGrWindow();

  // resore entire screen
  _fmemcpy(MAKE_FP(0xB800,0),(char far *)buf,grTextCols()*grTextRows()*2);
};
#endif