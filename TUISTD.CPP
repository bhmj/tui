#include <process.h>
#include <alloc.h>      // farcoreleft
#include "textio.h"     // WriteXYC
#include "tuimouse.h"   // GetMouseStatus
#include "tuikeys.h"    // keycodes
#include "tuistr.h"     // StrLen,StrCpy
#include "tuiconst.h"   // constants & colors
#include "tuistd.h"     // classes

// colors
char cDesktop         = ccDesktop        ;        // desktop
char cStatus          = ccStatus         ;        // status bar
char cStatusHi        = ccStatusHi       ;        // status bar highlight
char cDosWindow       = ccDosWindow      ;        // DOS color
char cShadow          = ccShadow         ;        // window shadow
char cDialog          = ccDialog         ;        // dialog window
char cDialogMove      = ccDialogMove     ;        // dialog window moving
char cStaticText      = ccStaticText     ;        // static text
char cStaticTextHi    = ccStaticTextHi   ;        // static text highlight
char cGroup           = ccGroup          ;        // group
char cDivider         = ccDivider        ;        // divider
char cButton          = ccButton         ;        // button normal
char cButtonHi        = ccButtonHi       ;        // button normal highlight
char cButtonD         = ccButtonD        ;        // button default
char cButtonDHi       = ccButtonDHi      ;        // button default highlight
char cButtonA         = ccButtonA        ;        // button selected
char cButtonAHi       = ccButtonAHi      ;        // button selected highlight
char cButtonShadow    = ccButtonShadow   ;        // button shadow
char cEdit            = ccEdit           ;        // edit line
char cEditA           = ccEditA          ;        // edit line focused
char cEditIcon        = ccEditIcon       ;        // edit icons
char cEditIconA       = ccEditIconA      ;        // edit icons focused
char cEditView        = ccEditView       ;        // edit line (view mode)
char cEditViewIcon    = ccEditViewIcon   ;        // edit icons (view mode)
char cEditDateIconE   = ccEditDateIconE  ;        // edit date symbols (edit mode)
char cEditDateIconV   = ccEditDateIconV  ;        // edit date symbols (view mode)
char cDateRect        = ccDateRect       ;        // date box rect
char cDateWeek        = ccDateWeek       ;        // date box weekday titles
char cDateWeekEnd     = ccDateWeekEnd    ;        // date box weekend titles
char cDateDays        = ccDateDays       ;        // date box current month days
char cDateDaysX       = ccDateDaysX      ;        // date box non-current month days
char cDateYear        = ccDateYear       ;        // date box year
char cDateMonth       = ccDateMonth      ;        // date box month
char cDateCur         = ccDateCur        ;        // date box cursor
char cCheck           = ccCheck          ;        // check box
char cCheckHi         = ccCheckHi        ;        // check box highlight
char cCheckA          = ccCheckA         ;        // check box selected
char cCheckAHi        = ccCheckAHi       ;        // check box selected highlight
char cList            = ccList           ;        // list box
char cListDiv         = ccListDiv        ;        // list box divider
char cListA           = ccListA          ;        // list box: focused item
char cListS           = ccListS          ;        // list box: selected item
char cCustomList      = ccCustomList     ;        // custom list
char cCustomListA     = ccCustomListA    ;        // custom list: focused item (cursor)
char cCustomListS     = ccCustomListS    ;        // custom list: selected item
char cCustomListTitle = ccCustomListTitle;        // custom list title bar
char cScroller        = ccScroller       ;        // scroll bar
char cScrollerIcon    = ccScrollerIcon   ;        // scroll bar icon
char cProgressWin     = ccProgressWin    ;        // progress window
char cProgressBar     = ccProgressBar    ;        // progress bar
char cProgressNum     = ccProgressNum    ;        // progress percent
char cMenuListNorm    = ccMenuListNorm   ;        // menu list item
char cMenuListNormHi  = ccMenuListNormHi ;        // menu list item highlight
char cMenuListFocus   = ccMenuListFocus  ;        // menu list focused
char cMenuListFocusHi = ccMenuListFocusHi;        // menu list focused highlight
char cMenuListSel     = ccMenuListSel    ;        // menu list selected
char cMenuListSelHi   = ccMenuListSelHi  ;        // menu list selected highlight

int TuiLng = ENGLISH;   // default language is english

void DlgColor( char back, char title, char text, char high )
{
  cDialog       = (back<<4)+(title&0x0F);
  cStaticText   = (back<<4)+(text&0x0F);
  cStaticTextHi = (back<<4)+(high&0x0F);
  cButtonShadow = (back<<4)+(cButtonShadow&0x0F);
};
void DlgReset( void ) {
  DlgColor(ccDialog>>4,ccDialog&0x0F,ccStaticText&0x0F,ccStaticTextHi&0x0F);
};

int ExecuteDialog( TDialog* dlg )
{
  TMessage msg;
  int xcode;

  dlg->Paint();

  Once:
  do {
    GetMessage(msg);
    xcode = dlg->Message(msg);
  } while ( xcode!=DLG_CANCEL && xcode!=DLG_OK );

  if (xcode==DLG_OK)
    if (!dlg->FlushData()) goto Once;

  dlg->Hide();

  DlgReset();

  return xcode;
};

#define MAX_MESSAGE     16
TMessage message[ MAX_MESSAGE ];
int msg_index=-1;
int clickx,clicky;
int clicktime;

// если очередь пуста - дождаться сообщения и вернуть его
// иначе выбрать первое сообщение из очереди
void GetMessage( TMessage &msg )
{
  static int prev_msx, prev_msy, prev_btns=0;
  int msx, msy, btns=0;
  int softimer;

  msg.what=0;
  msg.code=0;
  softimer=0;
  while ( msg_index == -1 )
  {
    if ( KeyPressed() )
    {
      // keyboard
      msg.what = evKeyb;
      msg.code = ReadKey();
      if (msg.code==0x2D00) exit(0);
      break;
    } else {
      // mouse
      MouseGetStatus( msx, msy, btns );
      if ( (msx!=prev_msx) || (msy!=prev_msy) || (btns!=prev_btns) )
      {
        msg.what = evMouse;
        msg.code = 0;
        msg.lbutton = 0;
        msg.rbutton = 0;
        msg.msx = msx>>3;
        msg.msy = msy>>3;
        // left
        if ( btns & 1 ) msg.lbutton |= msIsdown;
        else            msg.lbutton |= msIsup;
        if ( (prev_btns&1) > (btns&1) ) msg.lbutton |= msKeyup;
        if ( (prev_btns&1) < (btns&1) ) msg.lbutton |= msKeydown;
        // right
        if ( btns & 2 ) msg.rbutton |= msIsdown;
        else            msg.rbutton |= msIsup;
        if ( (prev_btns&2) > (btns&2) ) msg.rbutton |= msKeyup;
        if ( (prev_btns&2) < (btns&2) ) msg.rbutton |= msKeydown;
        if ( (msx!=prev_msx) || (msy!=prev_msy) ) msg.code |= msMove;
        if (btns!=prev_btns) msg.code |= msButton;
        // double click
        if (msg.lbutton&msKeydown)
        {
          if ( ((int)GetTimer())-clicktime < 18/3 )
            if ( Abs(clickx-msg.msx) < 2 && Abs(clicky-msg.msy) < 1 )
            {
              msg.lbutton|=msDblClick;
            };
          clickx = msg.msx;
          clicky = msg.msy;
          clicktime = (int)GetTimer();
        };
        prev_msx = msx;
        prev_msy = msy;
        prev_btns = btns;
        break;
      };
    };
    if (softimer++ > TIMER_RATE) break;
  };
  if ( softimer > TIMER_RATE ) msg.what |= evTimer;
  if ( msg_index == -1 ) return;        // keyboard/mouse message
  msg = message[0];
  for (msx=1; msx<=msg_index; msx++) message[msx-1] = message[msx];
  msg_index--;
}

void PostMessage( int put_what, int put_code,
  void* target, void* sender, void far* dataptr )
{
  msg_index++;
  message[msg_index].what = put_what;
  message[msg_index].code = put_code;
  message[msg_index].target  = target;
  message[msg_index].sender  = sender;
  message[msg_index].dataptr = dataptr;
}

// *TRect* ----------------------------------------------------
TRect::TRect( void ) { x0=0; y0=0; x1=0; y1=0; width=0; height=0; };
TRect::TRect( int ix0, int iy0, int ix1, int iy1 )
{
  int tmp;
  if (x1 < x0) { tmp=x0; x0=x1; x1=tmp; };
  if (y1 < y0) { tmp=y0; y0=y1; y1=tmp; };
  x0 = ix0; y0 = iy0;
  x1 = ix1; y1 = iy1;
  width  = x1-x0+1;
  height = y1-y0+1;
};
BOOL TRect::InRect( int x, int y )
{
  if ( (x<x0) || (x>x1) || (y<y0) || (y>y1) ) return False;
  return True;
};

// *TWindow* --------------------------------------------------

TWindow::TWindow( int x, int y, int w, int h, char c, char* head, BOOL shadow )
{
  w+=2; h+=2;
  if (x+w-1 > GetMaxCol()) x=GetMaxCol()-w+1;
  if (y+h-1 > GetMaxRow()) y=GetMaxRow()-h+1;
  TRect::TRect( x, y, x+w-1, y+h-1 );
  RColor   = c;
  CColor   = c;
  RStyle   = DOUBLE;
  if (head) StrCpy(header,head); else StrCpy(header,"");
  if (StrLen(header) > width-6) header[width-6]=0;
  buf      = 0;
  bufsize  = 0;
  HasShadow = shadow;
  Shown    = False;
};

TWindow::~TWindow(void) { if (Shown) Hide(); };

void TWindow::Show(void)
{
  // get current window settings
  SaveWindowPos();
  // save background
  bufsize = GetWindowSize(width+2,height+1);
  if (farcoreleft() > bufsize) {
    buf = new char[bufsize+2];
    MouseHide();
    SaveWindow( x0,y0,width+2,height+1,buf );
    MouseShow();
  };
  TWindow::Rect( RColor,RStyle );
  if (HasShadow) TWindow::Shadow();
  TWindow::Paint();
  Shown = True;
};

void TWindow::Hide(void)
{
  // restore window setting
  RestWindowPos();
  if (buf==0) return;
  // restore background
  MouseHide();
  RestoreWindow( x0, y0, buf );
  MouseShow();
  delete buf;
  buf=0;
  Shown = False;
};

void TWindow::Rect( char c, char style )
{
  MouseHide();
  SetWindow( x0, y0, width, height );
  SetTextColor( c & 0x0F );
  SetBackColor( c >> 4 );
  Rectangle( 0, 0, width, height, style );
  if ( (header) && (*header) ) {
    WriteXY( (width-StrLenC(header)-2)/2, 0, " " );
    WriteXY( (width-StrLenC(header)-2)/2+1, 0, header );
    WriteXY( (width+StrLenC(header))/2, 0, " " );
  };
  SetWindow( x0+1, y0+1, width-2, height-2 );
  RColor = c;
  RStyle = style;
  MouseShow();
};

void TWindow::Shadow(void)
{
  MouseHide();
  SetWindow( x1+1, y0+1, 2, height );
  for (int i=0; i<height; i++) AttrXY(0,i,cShadow,2);
  SetWindow( x0+2, y1+1, width, 1 );
  AttrXY(0,0,cShadow,width);
  SetWindow( x0+1, y0+1, width-2, height-2 );
  MouseShow();
};

void TWindow::Paint(void)
{
  MouseHide();
  SetWindow( x0+1, y0+1, width-2, height-2 );
  SetTextColor( CColor & 0x0F );
  SetBackColor( CColor >> 4 );
  ClearWindow(' ');
  MouseShow();
};

extern winoff;          // graphics window!

void TWindow::Move( int dx, int dy )
{
  char far* backgr, far* window;
  int ix,iy,iw,ih;
  int px,py;
  int cx,cy;

  if ( (x0+dx < 0) || (x1+dx > GetMaxCol()) ||
       (y0+dy < 0) || (y1+dy > GetMaxRow()) ) return;
  cx = GetCursorX();
  cy = GetCursorY();
  winoff += dx*2 + dy*160;

  MouseHide();

  if ( ( Abs(dx)>=width ) || ( Abs(dy) >= height ) )
  { // (( Full window redraw ))
    window = new char[bufsize];
    // save entire window
    SaveWindow(x0,y0,width,height,window);
    // restore background
    RestoreWindow(x0,y0,buf);
    x0 += dx;
    x1 += dx;
    y0 += dy;
    y1 += dy;
    // save new background
    SaveWindow(x0,y0,width+2,height+1,buf);
    // shadow
    if (HasShadow) TWindow::Shadow();
    // put a window on a new position
    RestoreWindow(x0,y0,window);
    delete window;
  } else {
    // (( Partial move ))
    backgr = new char[bufsize];
    window = new char[bufsize];
    // clear shadow
    PasteBufWindow(2,height,width,1,x0+2,y1+1,buf);
    PasteBufWindow(width,1,2,height-1,x1+1,y0+1,buf);
    // save new background
    SaveWindow(x0+dx,y0+dy,width+2,height+1,backgr);
    // save entire window
    SaveWindow(x0,y0,width,height,window);
    // copy intersection background area into new background buffer
    ix = dx; iy = dy;
    if (ix<0) ix=0;
    if (iy<0) iy=0;
    iw = width+2-(int)Abs(dx);
    ih = height+1-(int)Abs(dy);
    px = -dx;
    py = -dy;
    if (px<0) px=0;
    if (py<0) py=0;
    PasteBufBuf(ix,iy,iw,ih,buf,px,py,backgr);
    // show X slice
    if (dx>0) { ix=0; iy=0; iw=dx; ih=height; px=x0; py=y0; };
    if (dx<0) { ix=width+dx; iy=0; iw=-dx; ih=height; px=x1+dx+1; py=y0; };
    if (dx) PasteBufWindow(ix,iy,iw,ih,px,py,buf);
    // show Y slice
    if (dy>0) { ix=0; iy=0; iw=width; ih=dy; px=x0; py=y0; };
    if (dy<0) { ix=0; iy=height+dy; iw=width; ih=-dy; px=x0; py=y1+dy+1; };
    if (dy) PasteBufWindow(ix,iy,iw,ih,px,py,buf);
    // apply movement
    x0 += dx;
    x1 += dx;
    y0 += dy;
    y1 += dy;
    // shadow
    if (HasShadow) Shadow();
    // restore entire window on new position
    RestoreWindow( x0, y0, window );
    delete window;
    // replace background buffer
    delete buf;
    buf = backgr;
  };
  SetWindow(x0+1,y0+1,width-2,height-2);
  CursorXY(cx,cy);
  MouseShow();
};

// *TDlgItem* -------------------------------------------------

TDlgItem::TDlgItem(void)
{
  TabStop  = False;
  Default  = False;
  HasFocus = False;
  Active   = False;
  next     = 0;
  prev     = 0;
  pData    = 0;
};

TDlgItem::~TDlgItem(void) {};
void TDlgItem::Paint(void) {};
void TDlgItem::Hide(void) {};
void TDlgItem::FocusOn(void) { if (!HasFocus) { HasFocus=True; Paint(); }; };
void TDlgItem::FocusOff(void) { if (HasFocus) { HasFocus=False; Paint(); }; };
void TDlgItem::Update(void) { Paint(); };
BOOL TDlgItem::FlushData(void) { return True; };
int  TDlgItem::Message( TMessage& msg ) {
  if (msg.what&evBroadcast)
    if (msg.code==cmUpdate)
      if ( (msg.target==0) || (msg.target==this) ) Update();
  return DLG_NOACTION;
};
int  TDlgItem::GetXsize(void) { return 0; };
int  TDlgItem::GetYsize(void) { return 0; };
void TDlgItem::MoveXY( int dx, int dy ) { x0+=dx; y0+=dy; x1+=dx; y1+=dy; };

// *TStatic* --------------------------------------------------

TStatic::TStatic( int x, int y, char* n )
{
  TabStop = False;
  Default = False;
  StrCpy(Name,n);
  pData = n;
  TRect::TRect(x,y,x+StrLenC(n)-1,y);
};
void TStatic::Paint(void) { MouseHide(); SetHiAttr(cStaticTextHi); WriteXYC( x0, y0, cStaticText, Name ); MouseShow(); };
void TStatic::Update(void)    { StrCpy(Name,(char*)pData); Paint(); };
int TStatic::GetXsize( void ) { return StrLenC(Name); };
int TStatic::GetYsize( void ) { return 1; };

// *TDivider* -------------------------------------------------

TDivider::TDivider( int x, int y, int len )
{
  TabStop = False;
  Default = False;
  Len = len;
  TRect::TRect(x,y,x+Len-1,y);
};
void TDivider::Paint(void) { MouseHide(); SpawnXYC(x0,y0,cDivider,'─',Len); MouseShow(); };

// *TButton* --------------------------------------------------

TButton::TButton( int x, int y, char *n, int code, BOOL def, int key, void far* act )
{
  TabStop = True;
  Default = def;
  BtnLen  = StrLenC(n)+2;
  if ( BtnLen<5 ) BtnLen=5;
  StrCpy(Name,n);
  NamePos = x0 + (BtnLen-StrLenC(Name))/2;
  Action  = ( void(far*)() )act;
  XCode   = code;
  hotkey  = key;
  Pushed  = False;
  TRect::TRect( x,y,x+BtnLen-1,y );
};

void TButton::Paint(void)
{
  char face,hi; // colors

  if (HasFocus) { SetCursor(CUR_OFF); face=cButtonA; hi=cButtonAHi; }
  else if (Default) { face=cButtonD; hi=cButtonDHi; }
       else { face=cButton; hi=cButtonHi; };

  MouseHide();
  SetHiAttr(hi);
  SpawnXYC( x0, y0, cButtonShadow, ' ', Pushed );
  SpawnXYC( x0+Pushed, y0, face, ' ', BtnLen );
  WriteXYC( x0+NamePos+Pushed, y0, face, Name );
  if (HasFocus) {
    WriteXYC( x0+Pushed, y0, face, "" );
    WriteXYC( x0+Pushed+BtnLen-1, y0, face, "" );
  };
  if (!Pushed) {
    SpawnXYC( x0+1, y0+1, cButtonShadow, '▀', BtnLen );
    SpawnXYC( x0+BtnLen, y0, cButtonShadow, '▄', 1 );
  } else
    SpawnXYC( x0+1, y0+1, cButtonShadow, ' ', BtnLen );
  MouseShow();
};

void TButton::Push(void) { Pushed=True; Paint(); };
void TButton::Pop(void)  { Pushed=False; Paint(); };

int TButton::Message( TMessage &msg )
{
  int xcode = DLG_NOACTION;

  // keyboard
  if ( msg.what & evKeyb )
  {
    switch ( msg.code )
    {
      case kbSpace: if (!HasFocus) break;
      case kbEnter:
        Push(); Delay(3); Pop();
        if (Action) Action();
        xcode = XCode;
        break;
      default:
        if ( (hotkey) && ( (msg.code==hotkey) ||           // scan+ascii
                           ((msg.code&0xFF)==hotkey) ||    // ascii
                           ((msg.code&0xFF00)==hotkey) ) ) // scan
        {
          Push(); Delay(3); Pop();
          if (Action) Action();
          xcode = XCode;
        };
    };
  };
  // mouse
  if ( msg.what&evMouse )
  {
    if ( Active )
    {
      if ( InRect(msg.msx,msg.msy) )
      {
        if ( (msg.lbutton&msIsdown) && (!Pushed) ) Push();
        if ( msg.lbutton&msKeyup )
        {
          Pop();
          Active = False;
          if (Action) Action();
          xcode = XCode;
        };
      } else if (Pushed) Pop();
    } else {
      if ( InRect(msg.msx,msg.msy) )
      {
        if ( msg.lbutton&msKeydown ) { Active=True; Push(); xcode = DLG_KEYUSED; };
      }
    };
  };//evMouse
  return xcode;
};
int TButton::GetXsize( void ) { return BtnLen+1; };
int TButton::GetYsize( void ) { return 2; };

// *TEdit* ----------------------------------------------------

TEdit::TEdit( int x, int y, int w, char *s, BOOL autotab,
  int editmode, int maxlen, int discreet )
{
  TabStop = True;
  Default = False;
  Len     = w;
  MaxLen  = (maxlen==0 ? 255 : maxlen);
  AutoTab = autotab;
  InsMode = True;
  Editable = (BOOL)( editmode & edEdit );
  EditMode = (BOOL)( editmode & edInput );
  Editing  = (BOOL)( EditMode ? 1 : 0 );
  pData    = s;
  StrCpy(str,s);
  SPos = 0;
  CPos = 1;
  if (discreet) dChar = '*'; else dChar=0;
  TRect::TRect( x,y,x+w,y );
};

void TEdit::ShowLine(void)
{
  int i;
  char s[256];
  char cstr,cicon;

  // colors & cursor
  if (HasFocus)
  {
    if (Editing)  { cstr=cEditA; cicon=cEditIconA; }
    else          { cstr=cEditView;  cicon=cEditViewIcon;  };
    if (Editing)
    {
      CursorXY( x0+CPos-SPos, y0 );
      if (InsMode) SetCursor(CUR_THIN); else SetCursor(CUR_BLOCK);
    } else SetCursor(CUR_OFF);
  } else {
    cstr=cEdit;  cicon=cEditIcon;
  };

  MouseHide();
  if (SPos > 0) StrCpy(s,"\x11"); else StrCpy(s," ");
  WriteXYC( x0, y0, cicon, s );
  if (StrLen(str)-SPos > Len-2) StrCpy(s,"\x10"); else StrCpy(s," ");
  WriteXYC( x0+Len-1, y0, cicon, s );
  i = StrLen(str)-SPos;
  if (i > Len-2) i=Len-2;
  StrCut(s,str,SPos,i+SPos-1);
  StrAdd(s," ",Len-2-StrLen(s));
  if (dChar) {
    SpawnXYC(x0+1,y0,cstr,dChar,StrLen(str));
    SpawnXYC(x0+1+StrLen(str),y0,cstr,' ',Len-StrLen(str)); // discreet
  } else WriteXYC( x0+1, y0, cstr, s );                     // normal
  MouseShow();
};

void TEdit::MoveLeft(void)
{
  if (Editing) {
    if (CPos >= 2) CPos--;
    if (CPos < SPos+1) SPos=CPos-1;
    if (SPos < 0) SPos=0;
  } else {
    if(SPos>0) SPos--;
  };
};
void TEdit::MoveRight(void)
{
  if (Editing) {
    if (CPos <= StrLen(str)) CPos++;
    if (CPos > SPos+Len-1) SPos=CPos-Len+1;
  } else {
    if ( SPos < StrLen(str)+1-Len+1 ) SPos++;
  };
};
void TEdit::MoveHome(void) { CPos=1; SPos=0; };
void TEdit::MoveEnd(void) {
  CPos = StrLen(str)+1;
  SPos = CPos-Len+1;
  if (SPos < 0) SPos=0;
};

void TEdit::Paint(void) { ShowLine(); };

int TEdit::Message( TMessage &msg )
{
  unsigned char ch;
  int xcode=DLG_NOACTION;

  if ( msg.what & evBroadcast )
    return TDlgItem::Message(msg);

  if ( msg.what & evKeyb )
  {
    if (!HasFocus) return DLG_NOACTION;

    xcode = DLG_KEYUSED;
    switch( msg.code )
    {
      case kbUp:     if (!Editing) xcode=DLG_PREV; break;
      case kbDown:   if (!Editing) xcode=DLG_NEXT; break;
      case kbLeft:   MoveLeft(); break;
      case kbRight:  MoveRight(); break;
      case kbHome:   MoveHome(); break;
      case kbEnd:    MoveEnd(); break;
      case kbIns:    InsMode = (BOOL)InsMode^True; break;
      case kbDel:    StrDel(str,str,CPos-1); break;
      case kbBackSp:
        StrBksp(str,str,CPos-1);
        if (SPos) SPos--;
        MoveLeft();
        break;
      case kbEnter:
        if ( (Editable) && !(EditMode&edInput) )
        {
          Editing = (BOOL)Editing^True;
          Paint();
          xcode = DLG_KEYUSED;
        } else {
          if (AutoTab) xcode = DLG_NEXT;
          else xcode = DLG_NOACTION;
        };
        break;
      default:
        if (Editing)
        {
          ch = msg.code & 0x00FF;
          if (StartEditing) *str=0;
          if ( ch>=' ' )
            if ( (!MaxLen) || ( (CPos<=MaxLen) && ( !InsMode || (StrLen(str)<MaxLen) ) ) )
            {
              StrEnter(str,str,CPos-1,ch,InsMode);
              MoveRight();
            };
        } else xcode=DLG_NOACTION;
        break;
    };//switch (code)
    StartEditing=False;
  };// if evKeyb
  if (msg.what&evMouse)
    if (msg.lbutton&msKeydown)
      if (InRect(msg.msx,msg.msy))
        if (!HasFocus) xcode = DLG_KEYUSED;

  if (xcode!=DLG_NOACTION) Paint();
  return xcode;
};
void TEdit::FocusOn(void)   { if (!HasFocus) StartEditing=True; TDlgItem::FocusOn(); };
void TEdit::FocusOff(void)  { CPos=1; SPos=0; TDlgItem::FocusOff(); };
BOOL TEdit::FlushData(void) { StrCpy((char*)pData,str); return True; };
void TEdit::Update(void)    { StrCpy(str,(char*)pData); CPos=1; SPos=0; Paint(); };
int TEdit::GetXsize( void ) { return Len; };
int TEdit::GetYsize( void ) { return 1; };

// *TEditNum* -------------------------------------------------
TEditNum::TEditNum( int x, int y, int w, long *p, BOOL autotab, int editmode )
 : TEdit( x,y,w,0,autotab,editmode,11 )
{
  IntToDec(*p,str);
  pData = p;
  datalen=4;
};
TEditNum::TEditNum( int x, int y, int w, int *p, BOOL autotab, int editmode )
 : TEdit( x,y,w,0,autotab,editmode,6 )
{
  int i=*p;
  IntToDec(i,str);
  pData = p;
  datalen=2;
};
int TEditNum::Message(TMessage& msg)
{
  int code;
  if ( (msg.what==evKeyb) && (HasFocus) )
  {
    switch( msg.code )
    {
      case kbLeft:   case kbRight:  case kbUp:     case kbDown:
      case kbHome:   case kbEnd:    case kbIns:    case kbDel:
      case kbBackSp: case kbEnter:
        return TEdit::Message(msg);
      default:
        code = msg.code&0xFF;
        if ( ( code>='0' && code<='9' ) || (code=='-') )
          return TEdit::Message(msg);
        else
          return DLG_KEYUSED;
    };//switch
  };
  return TEdit::Message(msg);
};
BOOL TEditNum::FlushData(void) {
  if (datalen==2) *(unsigned int*)pData = StrToInt(str);
  if (datalen==4) *(unsigned long*)pData = StrToLong(str);
  return True;
};
void TEditNum::Update(void) {
  if (datalen==2) IntToDec(*(int*)pData,str);
  if (datalen==4) IntToDec(*(long*)pData,str);
  CPos=1; SPos=0;
  Paint();
};

// *TGroup* ---------------------------------------------------
TGroup::TGroup( int x, int y )
{
  TabStop = True;
  Xsize=0; Ysize=0;
  cur = 0;
  nitems = 0;
  TRect::TRect(x,y,x,y);
};

TGroup::~TGroup( void )
{
  TDlgItem *i;
  for (int n=0; n<nitems; n++) {
    i=cur->next;
    delete cur;
    cur=i;
  };
};

void TGroup::Insert( TDlgItem* i )
{
  if (!cur) {
    i->next=i;  i->prev=i;  cur=i;
  } else {
    i->next=cur; i->prev=cur->prev;
    cur->prev->next=i; cur->prev=i;
//    i->prev=cur; i->next=cur->next;
//    cur->next=i; cur->prev->prev=i;
  };
  if (i->x0 + i->GetXsize() > Xsize) Xsize = i->x0 + i->GetXsize();
  if (i->y0 + i->GetYsize() > Ysize) Ysize = i->y0 + i->GetYsize();
  nitems++;
  TRect::TRect( x0,y0,x0+Xsize,y0+Ysize );
};

void TGroup::SetWin(void) {
  GetWindow(grWinx,grWiny,grWinw,grWinh);
  SetWindow(grWinx+x0,grWiny+y0,Xsize,Ysize);
};
void TGroup::RestWin(void) { SetWindow(grWinx,grWiny,grWinw,grWinh); };

void TGroup::FocusOn( void )
{
  if (!HasFocus) {
    HasFocus=True;
    SetWin();
    if (cur) cur->FocusOn();
    RestWin();
  };
};
void TGroup::FocusOff( void )
{
  if (HasFocus) {
    HasFocus=False;
    SetWin();
    if (cur) cur->FocusOff();
    RestWin();
  };
};

void TGroup::Paint( void )
{
  int n;
  TDlgItem *i=cur;
  if (cur) {
    MouseHide();
    SetWin();
    for (n=0; n<Ysize; n++) SpawnXYC( 0, n, cGroup, ' ', Xsize );
    i=cur; for (n=0; n<nitems; n++) { i->Paint(); i=i->next; };
    if (HasFocus) cur->FocusOn();
    RestWin();
    MouseShow();
  };
};

int TGroup::Message( TMessage &msg )
{
  int n,xcode=DLG_NOACTION;
  TDlgItem* i;

  if ( msg.what & evBroadcast )
    return TDlgItem::Message(msg);

  SetWin();
  if ( msg.what & evKeyb )
  {
    if (HasFocus) xcode=DLG_KEYUSED; else xcode=DLG_NOACTION;
    switch( msg.code )
    {
      case kbUp:
        if (cur && HasFocus) { cur->FocusOff(); cur=cur->prev; cur->FocusOn(); };
        break;
      case kbDown:
        if (cur && HasFocus) { cur->FocusOff(); cur=cur->next; cur->FocusOn(); };
        break;
      case kbRight:
        if (cur && HasFocus) {
          cur->FocusOff();
          cur=cur->next;
          for (n=1; n<Ysize; n++) cur=cur->next;
          cur->FocusOn();
        };
        break;
      case kbLeft:
        if (cur && HasFocus) {
          cur->FocusOff();
          cur=cur->prev;
          for (n=1; n<Ysize; n++) cur=cur->prev;
          cur->FocusOn();
        };
        break;
      default:
        // give a message (cur)
        // for all the rest items till cur
        //   give a message to item i
        //   if (keyused)
        //     cur->FocusOff
        //     i->FocusOn
        //     cur = i (so that loop will last till this item)
        // for
        if (cur) xcode = cur->Message(msg);
        if (xcode==DLG_KEYUSED) cur->Paint();
        i=cur->next;
        while (i!=cur) {
          if ( i->Message(msg)==DLG_KEYUSED ) {
            cur->FocusOff();
            i->FocusOn();
            xcode=DLG_KEYUSED;
            msg.code=DLG_KEYUSED;
            cur=i;
          };
          i=i->next;
        };
        break;
    };
  };
  if ( msg.what & evMouse )
  {
    if (InRect(msg.msx,msg.msy))
    {
      msg.msx -= x0;
      msg.msy -= y0;
      if (cur) xcode = cur->Message(msg);
      if (xcode==DLG_KEYUSED) cur->Paint();
      i=cur->next;
      while (i!=cur) {
        if ( i->Message(msg)==DLG_KEYUSED ) {
          cur->FocusOff();
          i->FocusOn();
          xcode=DLG_KEYUSED;
          msg.what=evKeyb;
          msg.code=DLG_KEYUSED;
          cur=i;
        };
        i=i->next;
      };
      msg.msx -= x0;
      msg.msy -= y0;
    };
  };
  RestWin();
  return xcode;
};

BOOL TGroup::FlushData(void) {
  TDlgItem* i;
  i=cur;
  for (int n=0; n<nitems; n++) {
    if ( ! i->FlushData() ) return False;
    i=i->next;
  };
  return True;
};

void TGroup::Update(void) {
  TDlgItem* i;
  int n;

  i=cur;
  MouseHide();
  SetWin();
  for (n=0; n<Ysize; n++) SpawnXYC( 0, n, cGroup, ' ', Xsize );
  for (n=0; n<nitems; n++) {
    i->Update();
    i=i->next;
  };
  RestWin();
  MouseShow();
  if (HasFocus) cur->FocusOn();
};

// *TCheckBox* ------------------------------------------------
TCheckBox::TCheckBox( int x, int y, char *n, int* data, int key )
{
  TabStop  = True;
  Default  = False;
  HasFocus = False;
  Name     = new char[128];
  StrCpy(Name,n);
  pData    = data;
  checked  = *data;
  hotkey   = key;
  TRect::TRect(x,y,x+StrLenC(n)+6-1,y);
};
TCheckBox::~TCheckBox(void) {  delete Name;  };
void TCheckBox::Paint(void)
{
  char a,hi;
  char s[128];

  MouseHide();
  if (HasFocus) { a=cCheckA; hi=cCheckAHi; }
  else { a=cCheck; hi=cCheckHi; };

  if (checked) StrCpy(s," [X] "); else StrCpy(s," [ ] ");
  StrAdd(s,Name,1);
  SetHiAttr(hi);
  WriteXYC( x0, y0, a, s );
  if (HasFocus) { SetCursor(CUR_THIN); CursorXY( x0+2, y0 ); };
  MouseShow();
};
int  TCheckBox::Message( TMessage &msg )
{
  int code=msg.code;

  if ( msg.what & evBroadcast )
    return TDlgItem::Message(msg);

  if ( msg.what & evKeyb )
  {
    if ( (hotkey) && ( (code==hotkey) ||
                       ((code&0xFF)==hotkey) ||
                       ((code&0xFF00)==hotkey) ) )
    {
      if (checked) checked=0; else checked=1;
      Paint();
      msg.code=DLG_KEYUSED;
      return DLG_KEYUSED;
    };
    if( (HasFocus) && (code==kbSpace) )
    {
      if (checked) checked=0; else checked=1;
      Paint();
      msg.code=DLG_KEYUSED;
      return DLG_KEYUSED;
    } else return DLG_NOACTION;
  };//if evKeyb
  if (msg.what&evMouse)
    if (msg.lbutton&msKeydown)
      if (InRect(msg.msx,msg.msy))
      {
        if (checked) checked=0; else checked=1;
        Paint();
        msg.what=evKeyb;
        msg.code=DLG_KEYUSED;
        return DLG_KEYUSED;
      };
  return DLG_NOACTION;
};
BOOL TCheckBox::FlushData(void) { *(int*)pData=checked; return True; };
void TCheckBox::Update(void)    { checked=*(int*)pData; Paint(); };
int  TCheckBox::GetXsize(void)  { return StrLenC(Name)+6; };
int  TCheckBox::GetYsize(void)  { return 1; };

// *TRadioButton* ---------------------------------------------
TRadioButton::TRadioButton( int x, int y, char *n, int* data, int val, int key )
{
  TabStop  = True;
  Default  = False;
  HasFocus = False;
  Name     = new char[128];
  StrCpy(Name,n);
  pData    = data;
  checked  = (*data==val);
  value    = val;
  hotkey   = key;
  TRect::TRect(x,y,x+StrLenC(n)+6-1,y);
};
TRadioButton::~TRadioButton(void) {  delete Name;  };
void TRadioButton::Paint(void)
{
  char a,hi;
  char s[128];

  MouseHide();
  if (HasFocus) { a=cCheckA; hi=cCheckAHi; }
  else { a=cCheck; hi=cCheckHi; };
  if (checked) StrCpy(s," (\x07) "); else StrCpy(s," ( ) ");
  StrAdd(s,Name,1);
  SetHiAttr(hi);
  WriteXYC( x0, y0, a, s );
  if (HasFocus) { SetCursor(CUR_THIN); CursorXY( x0+2, y0 ); };
  MouseShow();
};
int  TRadioButton::Message( TMessage &msg )
{
  int code,xcode=DLG_NOACTION;

  if ( msg.what & evBroadcast )
    return TDlgItem::Message(msg);

  code = msg.code;
  if ( msg.what & evKeyb )
  {
    if ( msg.code==DLG_KEYUSED )
    {
      if (checked) { checked=0; Paint(); };
    };
    if ( (hotkey) && ( (code==hotkey) ||
                       ((code&0xFF)==hotkey) ||
                       ((code&0xFF00)==hotkey) ) )
    {
      if (!checked) checked=1;
      msg.code=DLG_KEYUSED;
      xcode=DLG_KEYUSED;
    };
    if( (HasFocus) && (code==kbSpace) )
    {
      if (!checked) checked=1;
      msg.code=DLG_KEYUSED;
      xcode=DLG_KEYUSED;
    };
  };//if evKeyb
  if (msg.what&evMouse)
    if (msg.lbutton&msKeydown)
      if (InRect(msg.msx,msg.msy))
      {
        if (!checked) checked=1;
        msg.what=evKeyb;
        msg.code=DLG_KEYUSED;
        xcode=DLG_KEYUSED;
      };
  return xcode;
};
BOOL TRadioButton::FlushData(void) { if (checked) *(int*)pData=value; return True; };
void TRadioButton::Update(void)    { if (*(int*)pData==value) checked=1; else checked=0; Paint(); };
int  TRadioButton::GetXsize(void)  { return StrLenC(Name)+6; };
int  TRadioButton::GetYsize(void)  { return 1; };

// *TList* ----------------------------------------------------
typedef char* pchar;
TList::TList( int x, int y, int w, int h, char *s, long* sel )
{
  int i;
  char *p;

  TRect::TRect( x,y,x+w-1,y+h-1 );
  ScrLen = height;
  ColLen = height;

  TabStop = True;
  Default = False;
  Len     = StrCountZ(s);
  pLen    = 0;
  pSel    = sel;
  if (Len) pItem = new pchar[Len]; else pItem = 0;
  if (pItem) Allocated=True; else Allocated=False;
  pData   = pItem;
  p=s; for (i=0; i<Len; i++) { pItem[i]=p; while (*p) p++; p++; };
  pLen    = &Len;
  ScrollPos = 1;
  SPos = 0;
  CPos = 0;
};
TList::TList( int x, int y, int w, int h, char** ptr, long* len, long* sel )
{
  TRect::TRect( x,y,x+w-1,y+h-1 );
  ScrLen = height;
  ColLen = height;

  TabStop = True;
  Default = False;
  Allocated = False;
  pData   = ptr;
  pItem   = ptr;
  Len     = *len;
  pLen    = len;
  pSel    = sel;
  ScrollPos = 1;
  SPos = 0;
  CPos = 0;
};
TList::~TList(void) { if (Allocated) delete pItem; };

void TList::Paint(void)
{
  int i;

  SetWin(0);
  MouseHide();
  if (SPos>Len-ScrLen) SPos=Len-ScrLen;
  if (SPos<0) SPos=0;
  for (i=0; i<height; i++)
  {
    SpawnXYC( 0, i, cList, ' ', width-1 );
    if ( SPos+i < Len ) WriteXYC( 1, i, cList, pItem[SPos+i] );
    if (Len > height)
      SpawnXYC( width-1, i, cScroller, '▒', 1 );
    else
      SpawnXYC( width-1, i, cList, ' ', 1 );
  };
  MouseShow();
  RestWin();
  ShowCursor(True);
  ShowScroller();
};

int TList::Message( TMessage &msg )
{
  int prevSPos,prevCPos;
  int xcode=DLG_NOACTION;

  if ( msg.what & evBroadcast )
    return TDlgItem::Message(msg);

  if ( msg.what & evKeyb )
  {
    if (!HasFocus) return DLG_NOACTION;

    xcode = DLG_KEYUSED;
    prevSPos=SPos;
    prevCPos=CPos;
    ShowCursor(False);
    switch( msg.code )
    {
      case kbUp:   if (CPos) if (--CPos < SPos) SPos--; break;
      case kbDown: if (CPos<Len-1) if (++CPos >= SPos+ScrLen) SPos++; break;
      case kbHome: CPos = SPos = 0; break;
      case kbEnd:  CPos=Len-1; SPos=CPos-ScrLen+1; break;
      case kbLeft: CPos-=ColLen-1; if (SPos>CPos) SPos=CPos; break;
      case kbPgUp: CPos-=ScrLen-1; if (SPos>CPos) SPos=CPos; break;
      case kbRight:CPos+=ColLen-1; if (SPos<CPos-ScrLen+1) SPos=CPos-ScrLen+1; break;
      case kbPgDn: CPos+=ScrLen-1; if (SPos<CPos-ScrLen+1) SPos=CPos-ScrLen+1; break;
      default: xcode=DLG_NOACTION;
    };
    if (CPos>Len-1) CPos=Len-1;
    if (SPos>Len-ScrLen) SPos=Len-ScrLen;
    if (CPos<0) CPos=0;
    if (SPos<0) SPos=0;
    if (prevSPos != SPos) Paint();
    if (prevCPos != CPos) ShowScroller();
    *pSel = CPos;
    ShowCursor(True);
  };// if evKeyb
  if (msg.what&evMouse)
    if (msg.lbutton&msIsdown)
      if (InRect(msg.msx,msg.msy))
      {
        if (msg.msx-x0+1<width)
        {
          ShowCursor(False);
          CPos = SPos+(msg.msy-y0);
          if (CPos>Len-1) CPos=Len-1;
          FocusOn();
          ShowCursor(True);
          ShowScroller();
          xcode = DLG_KEYUSED;
          if (msg.lbutton&msDblClick)
          {
            PostMessage(evKeyb,kbEnter);
            xcode=DLG_KEYUSED;
          };
        };
      };
  return xcode;
};

void TList::ShowCursor(BOOL what)
{
  char a;

  if (HasFocus) SetCursor(CUR_OFF);

  if (HasFocus)
    if (what) a=cListA; else a=cList;
  else
    if (what) a=cListS; else a=cList;
  SetWin(-1);
  MouseHide();
  if (CPos>Len-1) CPos=Len-1;
  SpawnXYC( 0, CPos-SPos, a, ' ', width-1 );
  if (Len) WriteXYC( 1, CPos-SPos, a, pItem[CPos] );
  MouseShow();
  RestWin();
};
void TList::ShowScroller(void)
{
  // scroll bar
  if (Len <= height) return;
  SetWin(0);
  MouseHide();
  SpawnXYC( width-1, 0, cScrollerIcon, '\x1E', 1 );
  SpawnXYC( width-1, height-1, cScrollerIcon, '\x1F', 1 );
  SpawnXYC( width-1, ScrollPos, cScrollerIcon, '▒', 1 );
  if ( Len > 1 )
  {
    ScrollPos = (unsigned int)( 1+((long)CPos*(height-3))/(Len-1) );
    SpawnXYC( width-1, ScrollPos, cScrollerIcon, '\xFE', 1 );
  };
  MouseShow();
  RestWin();
};

void TList::FocusOn( void )
{
  if (!HasFocus) { HasFocus=True; ShowCursor(True); };
};

void TList::FocusOff( void )
{
  if (HasFocus) { HasFocus=False; ShowCursor(True); };
};

void TList::SetWin(int i) {
  GetWindow(lsWinx,lsWiny,lsWinw,lsWinh);
  SetWindow(lsWinx+x0,lsWiny+y0,width+i,height);
};
void TList::RestWin(void) { SetWindow(lsWinx,lsWiny,lsWinw,lsWinh); };

void TList::Update(void) {
  if (Allocated) return;
  pItem = (char**)pData;
  Len  = *pLen;
  if (CPos>Len-1) CPos=Len-1;
  if (SPos>Len-ScrLen) SPos=Len-ScrLen;
  if (CPos<0) CPos=0;
  if (SPos<0) SPos=0;
  Paint();
};

// *TColumnList* ----------------------------------------------
TColumnList::TColumnList( int x, int y, int ncol, int wcol, int h, char* s )
 : TList( x, y, ncol*wcol, h, s )
{
  NCol = ncol;
  WCol = wcol;
  ScrLen = NCol*(h-1);
  ColLen = h;
  TRect::TRect( x,y,x+WCol*NCol-1,y+h-1 );
};
TColumnList::TColumnList( int x, int y, int ncol, int wcol, int h, char** ptr, long* len )
 : TList( x, y, ncol*wcol, h, ptr, len )
{
  NCol = ncol;
  WCol = wcol;
  ScrLen = NCol*(h-1);
  ColLen = h;
  TRect::TRect( x,y,x+WCol*NCol-1,y+h-1 );
};

int TColumnList::Message(TMessage& msg)
{
  int xcode=DLG_NOACTION;
  if ( msg.what & evBroadcast )
    return TList::Message(msg);

  if (msg.what&evKeyb) return TList::Message(msg);
  if (msg.what&evMouse)
    if (msg.lbutton&msIsdown)
      if (InRect(msg.msx,msg.msy))
      {
        if (msg.msy-y0+1 < height)
        {
          ShowCursor(False);
          CPos = SPos+(ColLen-1)*((msg.msx-x0)/WCol)+(msg.msy-y0);
          FocusOn();
          ShowCursor(True);
          ShowScroller();
          xcode = DLG_KEYUSED;
          if (msg.lbutton&msDblClick)
          {
            PostMessage(evKeyb,kbEnter);
            xcode=DLG_KEYUSED;
          };
        };
      };
  return xcode;
};

void TColumnList::Paint(void)
{
  int col,i;
  char ch='│';

  SaveWin();
  MouseHide();

  for (col=0; col<NCol; col++)
  {
    SetCol(col);
    for (i=0; i<height-1; i++)
    {
      SpawnXYC(0,i,cList,' ',WCol);           // фон
      if ( SPos+col*(height-1)+i < Len )
        WriteXYC( 1, i, cList, pItem[SPos+col*(height-1)+i] );
      SpawnXYC(WCol-1,i,cListDiv,ch,1);        // разделитель
    };
  };
  SetCol(-1);
  SpawnXYC( 0, 0, cScrollerIcon, '▒', width ); // scroller

  MouseShow();
  RestWin();
  ShowCursor(True);
  ShowScroller();
};

void TColumnList::ShowCursor(BOOL what)
{
  int y;
  char a;

  if (HasFocus) SetCursor(CUR_OFF);

  if (HasFocus)
    if (what) a=cListA; else a=cList;
  else
    if (what) a=cListS; else a=cList;
  SaveWin();
  MouseHide();
  SetCol( (CPos-SPos) / (height-1) );
  y = (CPos-SPos) % (height-1);
  SpawnXYC( 0, y, a, ' ', WCol-1 );
  if (Len) WriteXYC( 1, y, a, pItem[CPos] );
  MouseShow();
  RestWin();
};
void TColumnList::ShowScroller(void)
{
  // scroll bar
  SaveWin();
  MouseHide();
  SetCol(-1);
  SpawnXYC( 0, 0, cScrollerIcon, '\x11', 1 );
  SpawnXYC( width-2, 0, cScrollerIcon, '\x10', 1 );
  SpawnXYC( ScrollPos, 0, cScrollerIcon, '▒', 1 );
  if ( Len > 1 )
  {
    ScrollPos = 1+(CPos*(width-4))/(Len-1);
    SpawnXYC( ScrollPos, 0, cScrollerIcon, '\xFE', 1 );
  };
  MouseShow();
  RestWin();
};

void TColumnList::SaveWin(void) { GetWindow(lsWinx,lsWiny,lsWinw,lsWinh); };
void TColumnList::RestWin(void) { SetWindow(lsWinx,lsWiny,lsWinw,lsWinh); };
void TColumnList::SetCol(int i) {
  int last=0;
  if (i>=0) {
    if (i==NCol-1) last=1;
    SetWindow(lsWinx+x0+WCol*i,lsWiny+y0,WCol-last,height-1);
  } else
    SetWindow(lsWinx+x0,lsWiny+y0+height-1,width-1,1);
};

// *TNote* ----------------------------------------------------
TNote::TNote( int x, int y, int w, int h, int n )
{
  TabStop=False;
  Default=False;
  nItems=n;
  TRect::TRect(x,y,x+w-1,y+h-1);
};
void TNote::Paint(void) {};
int  TNote::Message( TMessage& msg ) { return TDlgItem::Message(msg); };
void TNote::Update(void) { Paint(); };

// *TDialog* ==================================================

TDialog::TDialog( int x, int y, int w, int h, char* head, BOOL shadow ) :
  TWindow( x,y,w,h, cDialog, head, shadow )
{
  cur=0;
  nitems=0;
};

TDialog::~TDialog(void)
{
  TDlgItem* tmp;

  while (nitems > 0) {
    tmp  = cur;
    cur = tmp->next;
    delete tmp;
    nitems--;
  };
};

void TDialog::Insert( TDlgItem* i )
{
  if (!cur) {
    i->next=i;  i->prev=i;  cur=i;
  } else {
    i->next=cur; i->prev=cur->prev;
    cur->prev->next=i; cur->prev=i;
  };
  nitems++;
};

void TDialog::Paint(void)
{
  TDlgItem* tmp;

  Cx=GetCursorX();
  Cy=GetCursorY();
  CShape=GetCursorShape();

  TWindow::Show();      // paint window

  if (cur) {
    // focus
    tmp=cur->prev;
    while ( (!cur->TabStop) && (tmp!=cur) ) cur=cur->next;
    cur->FocusOn();
    // paint all items
    tmp = cur;
    if (cur) do {
        tmp->Paint();
        tmp = tmp->next;
      } while ( tmp != cur );
  };
};

void TDialog::Hide( void )
{
  TDlgItem *tmp;
  tmp=cur;
  if (cur) do { tmp->Hide(); tmp=tmp->next; } while (tmp!=cur);
  TWindow::Hide();
  CursorXY(Cx,Cy);
  SetCursor(CShape);
};

int TDialog::Message( TMessage& msg )
{
  TDlgItem *tmp;
  int xcode=DLG_NOACTION;

  if (!cur) {
    if ( msg.what & evKeyb )
      switch ( msg.code )
      {
        case kbEsc:
          xcode=DLG_CANCEL;
          break;
        case kbCtrlF5:
          Move(msg);
          break;
      };
    return xcode;
  };

  msg.msx -= x0+1;
  msg.msy -= y0+1;

  if ( msg.what & evBroadcast )
    switch( msg.code )
    {
      case cmActivate:
        tmp = cur;
        do {
          if ( msg.target==tmp) {
            cur->FocusOff();
            tmp->FocusOn();
            cur=tmp;
          };
          tmp=tmp->next;
        } while( tmp != cur );
        break;
      default:
        xcode = DispatchMessage(msg);
    };

  if ( msg.what & evKeyb )
  {
    switch( msg.code )
    {
      case kbCtrlEnter:
        msg.code = kbEnter;
        tmp = cur->next;
        while (tmp != cur) {
          if (tmp->Default) { xcode = tmp->Message(msg); break; };
          tmp = tmp->next;
        };
        break;
      case kbTab:
        cur->FocusOff();
        do { cur=cur->next; } while( cur->TabStop != True );
        cur->FocusOn();
        break;
      case kbShiftTab:
        cur->FocusOff();
        do { cur=cur->prev; } while( cur->TabStop != True );
        cur->FocusOn();
        break;
      case kbCtrlF5:
        Move(msg);
        break;
      case kbEsc:
        xcode = DLG_CANCEL;
        break;
      default:
        xcode = DispatchMessage(msg);
        break;
    };//switch
  };//if evKeyb

  if ( msg.what & evMouse )
  {
    if ( (msg.lbutton&msKeydown) && (msg.msy==-1) && (msg.msx>=-1) && (msg.msx<width-1) )
       Move( msg );
    xcode = DispatchMessage(msg);
  };//if evMouse

  if ( msg.what & evTimer )
  {
    tmp = cur;
    msg.what &= evTimer;
    do {
      tmp->Message(msg);
      tmp = tmp->next;
    } while(tmp != cur);
  };

  return xcode;
};

int TDialog::DispatchMessage( TMessage &msg )
{
  TDlgItem *tmp;
  int xcode = cur->Message(msg);
  // item generated "Update" command
  if (xcode==DLG_UPDATE) {
    tmp=cur;
    do { tmp->Update(); tmp=tmp->next; } while( tmp!=cur );
  };
  // item generated "Tab" command
  if (xcode==DLG_NEXT) {
    cur->FocusOff();
    do { cur=cur->next; } while( cur->TabStop != True );
    cur->FocusOn();
  };
  // item generated "Shift+Tab" command
  if (xcode==DLG_PREV) {
    cur->FocusOff();
    do { cur=cur->prev; } while( cur->TabStop != True );
    cur->FocusOn();
  };
  // if item hasn't used a message
  if (xcode==DLG_NOACTION) {
    // dispatch message to all other items
    tmp = cur->next;
    while (tmp != cur) {
      if ( (msg.what&evKeyb) && (msg.code==kbEnter) ) {
        // [Enter] to default item
        if (tmp->Default) xcode = tmp->Message(msg);
      } else {
        // [other keys] to all items (maybe hotkey)
        xcode = tmp->Message(msg);
      };
      // message used?
      if ( xcode != DLG_NOACTION ) break;
      tmp = tmp->next;
    };
    // if message used by item, activate it
    if ( xcode != DLG_NOACTION ) {
      if (cur!=tmp) cur->FocusOff();
      cur=tmp;
    };
  };
  cur->FocusOn();
  return xcode;
};

int TDialog::FlushData( void )
{
  TDlgItem* i;
  // flush data in all items
  i=cur;
  do {
    if ( ! i->FlushData() ) return False;
    i=i->next;
  } while ( i != cur );
  return True;
};

void TDialog::Move( TMessage& msg )
{
  int what,placed=0;
  int oldx,oldy;

  if ( msg.what&evKeyb )   { what=evKeyb; oldx=x0; oldy=y0; }
  if ( msg.what&evMouse ) { what=evMouse; oldx=msg.msx+x0+1; oldy=msg.msy+x0+1; };

  MouseHide();
  TWindow::Rect( cDialogMove, SINGLE );           // green rectangle

  do {
    GetMessage( msg );
    if ( (msg.what&what)==evKeyb )
    {
      switch( msg.code )
      {
        case kbLeft:   TWindow::Move(-1,0); break;
        case kbRight:  TWindow::Move(+1,0); break;
        case kbUp:     TWindow::Move(0,-1); break;
        case kbDown:   TWindow::Move(0,+1); break;
        case kbHome:   TWindow::Move(-1,-1); break;
        case kbEnd:    TWindow::Move(-1,+1); break;
        case kbPgUp:   TWindow::Move(+1,-1); break;
        case kbPgDn:   TWindow::Move(+1,+1); break;
        case kbEsc:    TWindow::Move(oldx-x0,oldy-y0); placed=1; break;
        case kbEnter:  placed=1; break;
      };
    };
    if ( (msg.what&what)==evMouse )
    {
      if ( (msg.code&msMove) && ( (oldx!=msg.msx) || (oldy!=msg.msy) ) )
          TWindow::Move( msg.msx-oldx, msg.msy-oldy );
      if ( msg.code&msButton )
        if ( msg.lbutton&msIsup )
          placed=1;
      oldx=msg.msx; oldy=msg.msy;
    };
  } while ( placed==0 );
  TWindow::Rect( cDialog, DOUBLE );
  MouseShow();
};
