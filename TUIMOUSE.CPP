
extern "C" void far InstallGraphicsMouse(void);
extern "C" void far DeleteGraphicsMouse(void);
extern "C" void far ShowGraphicCursor(void);
extern "C" void far HideGraphicCursor(void);

#define LABEL(x) } x: asm {

int MousePresent=0;
int GraphMouse=0;

int MouseInit(void)
{
  int x;
  asm {
        mov     ax,0
        int     33h
        cmp     ax,0xFFFF
        je      __ok
        xor     bx,bx
      LABEL(__ok)
        mov     ax,bx
  };
  x = _AX;
  if (x) MousePresent=1;
  return x;
};

void SetGraphMouse(void) {
  if (MouseInit()==0) return;
  InstallGraphicsMouse();
  GraphMouse=1;
};
void ResetGraphMouse(void) {
  if (!MousePresent) return;
  DeleteGraphicsMouse();
  GraphMouse=0;
};

int MouseReset(void)
{
  asm {
        mov     ax,0
        int     33h
        cmp     ax,0xFFFF
        je      __ok
        xor     bx,bx
      LABEL(__ok)
        mov     ax,bx
  };
  return _AX;
};

void MouseShow( void )
{
  if (GraphMouse) {
    ShowGraphicCursor();
  } else {
    asm {
        mov     ax,0001h
        int     33h
    };
  };
};
void MouseHide( void )
{
  if (GraphMouse) {
    HideGraphicCursor();
  } else {
    asm {
        mov     ax,0002h
        int     33h
    };
  };
};
void MouseGetStatus(int &x, int &y, int& buttons )
{
  int xx,yy,bb;
  asm {
        mov     ax,0003h
        int     33h
        mov     xx,cx
        mov     yy,dx
        mov     bb,bx
  };
  x=xx; y=yy; buttons=bb;
};
void PlaceMouse(int x, int y )
{
  asm {
        mov     ax,0004h
        mov     cx,x
        mov     dx,y
        int     33h
  };
};
