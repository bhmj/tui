#include "textio.h"     // WriteXYC
#include "tuimouse.h"   // MouseShow, MouseHide
#include "tuistr.h"     // StrLen,StrCpy
#include "tuidate.h"    // date manipulation routines
#include "tuigraph.h"   // TUI graphics

#include "tuiconst.h"   // constants & colors
#include "tuiext.h"     // classes

#include "tuidlgs.h"

// Calendar
char *szMonthName[2][13] = {
  { "", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December" },
  { "", "Ÿ­¢ àì", "”¥¢à «ì", "Œ àâ", "€¯à¥«ì", "Œ ©", "ˆî­ì",
    "ˆî«ì", "€¢£ãáâ", "‘¥­âï¡àì", "Žªâï¡àì", "®ï¡àì", "„¥ª ¡àì" }
};
char *szDayName[2][2] = {
  { "Mo Tu We Th Fr", "Sa Su" },
  { "­ ‚â ‘à —â â", "‘¡ ‚á" }
};

// *TDate* ----------------------------------------------------
TDate::TDate( int x, int y, DATE *date )
{
  int winx,winy,winw,winh;
  dt = date;
  Date = *date;
  VisMonth = Date.month;
  VisYear  = Date.year;
  GetWindow( winx, winy, winw, winh );
  if ( winy+y+10  > GetMaxRow() ) y = GetMaxRow()-winy-10+1;
  if ( winx+x+25 > GetMaxCol() ) x = GetMaxCol()-winx-25+1;
  Day = Date.day;
  TRect::TRect( x, y, x+23, y+8 );
};
void TDate::Execute( void )
{
  char* buf;
  int i,winx,winy,winw,winh;
  BOOL xit=False;
  TMessage msg;

  MouseHide();
  SetCursor(CUR_OFF);

  // save background
  GetWindow( winx, winy, winw, winh );
  buf = new char [ GetWindowSize( 25,10 )+2 ];
  if (buf) SaveWindow( winx+x0, winy+y0, 25, 10, buf );

  // show window
  SetWindow( winx+x0, winy+y0, 24, 9 );
  SetAttr( cDateRect );
  ClearWindow(' ');
  SetAttr( cDateRect );
  Rectangle( 0, 0, 24, 9, SINGLE );
  SetWindow( winx+x0, winy+y0, 25, 10 );
  for( i=1; i<10; i++ ) AttrXY( 24, i, cShadow, 1 );
  AttrXY( 1, 9, cShadow, 23 );
  SetWindow( winx+x0, winy+y0, 24, 9 );

  Paint();
  ShowCursor(True);

  do {
    GetMessage( msg );
    if ( msg.what & evKeyb )
    {
      ShowCursor(False);
      switch( msg.code )
      {
        case kbEsc:   xit=True; break;
        case kbEnter: *dt=Date; xit=True; break;
        case kbPgUp:  PrevMonth(); break;
        case kbPgDn:  NextMonth(); break;
        case kbCtrlPgUp: PrevYear(); break;
        case kbCtrlPgDn: NextYear(); break;
        case kbLeft:  MoveDays(-1); break;
        case kbRight: MoveDays(+1); break;
        case kbUp:    MoveDays(-7); break;
        case kbDown:  MoveDays(+7); break;
        case kbHome:
          DateToday(&Date);
          Day=Date.day;
          VisMonth=Date.month;
          VisYear=Date.year;
          Paint();
          break;
      };
      ShowCursor(True);
    };
  } while (!xit);

  //restore background
  if (buf) RestoreWindow( winx+x0,winy+y0, buf );
  if (buf) delete buf;
  SetWindow( winx, winy, winw, winh );
  MouseShow();
};

void TDate::Paint(void)
{
  int i,days,weekday,x,y;
  char buf[32];

  // Header
  SpawnXYC( 1, 0, cDateRect, 'Ä', width-2 );
  IntToDec( VisYear, buf, 4 );
  SpawnXYC( 2, 0, cDateRect, ' ', 4+2 );
  WriteXYC( 3, 0, cDateYear, buf );
  i=StrLen(szMonthName[TuiLng][VisMonth]);
  SpawnXYC( 20-i, 0, cDateRect, ' ', i+2 );
  WriteXYC( 21-i, 0, cDateMonth, szMonthName[TuiLng][VisMonth] );
  // Week days
  WriteXYC(  2, 1, cDateWeek,    szDayName[TuiLng][0] );
  WriteXYC( 17, 1, cDateWeekEnd, szDayName[TuiLng][1] );
  // days
  // previous month
  weekday = DateWeekday( 1, VisMonth, VisYear );
  if (VisMonth==1) days=31;
  else days = DateDaysInMonth( VisMonth-1, VisYear );
  days = days - weekday + 1;
  for (i=0; i<weekday; i++) {
    IntToDec( days, buf, 2, ' ' ); days++;
    WriteXYC( 2+i*3, 2, cDateDaysX, buf );
  }
  // current month
  days = DateDaysInMonth( VisMonth, VisYear );
  x=weekday; y=2; i=1;
  while (i<=days) {
    IntToDec( i, buf, 2, ' ' );
    WriteXYC( 2+x*3, y, cDateDays, buf );
    if ( ++x > 6 ) { y++; x=0; };
    i++;
  };
  // next month
  i=1;
  while ( y<8 ) {
    IntToDec( i, buf, 2, ' ' );
    WriteXYC( 2+x*3, y, cDateDaysX, buf );
    if ( ++x > 6 ) { y++; x=0; };
    i++;
  };
};
void TDate::ShowCursor(BOOL show)
{
  char buf[20];
  int i;

  // i = linear cursor position
  i = DateWeekday( 1, VisMonth, VisYear ) + Day - 1;
  Cx = i % 7;
  Cy = i / 7;
  if (show)
  {
    // store date
    Date.day = Day;
    Date.month = VisMonth;
    Date.year = VisYear;
    // Show cursor
    IntToDec( Day, buf, 2, ' ' );
    WriteXYC( 2+Cx*3, 2+Cy, cDateCur, buf );
    // Show current date on the bottom
    SpawnXYC(  2, 8, cDateRect, ' ', 1 );
    SpawnXYC( 13, 8, cDateRect, ' ', 1 );
    SpawnXYC(  5, 8, cDateRect, '.', 1 );
    SpawnXYC(  8, 8, cDateRect, '.', 1 );
    IntToDec( Date.day, buf, 2, '0' );
    WriteXYC( 3, 8, cDateRect, buf );
    IntToDec( Date.month, buf, 2, '0' );
    WriteXYC( 6, 8, cDateRect, buf );
    IntToDec( Date.year, buf, 4, '0' );
    WriteXYC( 9, 8, cDateRect, buf );
  } else {
    // erase cursor
    IntToDec( Day, buf, 2, ' ' );
    WriteXYC( 2+Cx*3, 2+Cy, cDateDays, buf );
  };
};
void TDate::NextMonth(void) {
  int i;
  if (++VisMonth > 12) { VisYear++; VisMonth=1; };
  i = DateDaysInMonth(VisMonth,VisYear);
  if (Day > i) Day=i;
  Paint();
};
void TDate::PrevMonth(void) {
  int i;
  if (--VisMonth < 1) { VisYear--; VisMonth=12; };
  i = DateDaysInMonth(VisMonth,VisYear);
  if (Day > i) Day=i;
  Paint();
};
void TDate::NextYear(void) {
  int i;
  VisYear++;
  i = DateDaysInMonth(VisMonth,VisYear);
  if (Day > i) Day=i;
  Paint();
};
void TDate::PrevYear(void) {
  int i;
  VisYear--;
  i = DateDaysInMonth(VisMonth,VisYear);
  if (Day > i) Day=i;
  Paint();
};
void TDate::MoveDays(int d)
{
  int days;
  days = DateDaysInMonth(VisMonth,VisYear);
  Day += d;
  if (Day > days) { Day-=days; NextMonth(); };
  if (Day < 1)    { PrevMonth(); Day+=DateDaysInMonth(VisMonth,VisYear); };
};
// *TEditDate* ------------------------------------------------
TEditDate::TEditDate( int x, int y, DATE* date, BOOL autotab, int editmode,
                      int yearlen, char separator )
 : TEdit( x,y,10,0,autotab,editmode )
{
  str[0]=0;
  YearLen = yearlen;
  // Dialog item properties
  TabStop = True;
  Default = False;
  // Data
  if ( !DateValid(date) ) DateToday(date);
  dt = date;
  Date = *date;
  // Visualisation
  if ( YearLen!=2 && YearLen!=4) YearLen=4;
  TRect::TRect( x, y, x+12-4+YearLen-1, y );
  Len = 12-4+YearLen;
  MaxLen = 6+YearLen;
  Separator = separator;
  // Setup
  DateToStr();
  InsMode=False;
};
void TEditDate::Paint( void )
{
  char a;

  TEdit::Paint();
  if (Editing) a=cEditDateIconE; else a=cEditDateIconV;
  SpawnXYC( x0+3, y0, a, Separator, 1 );
  SpawnXYC( x0+6, y0, a, Separator, 1 );
};
int TEditDate::Message( TMessage& msg )
{
  int xcode=DLG_NOACTION;
  char c;
  TDate* dt;

  if ( msg.what & evBroadcast )
    return TEdit::Message(msg);

  if ( msg.what & evKeyb )
  {
    switch( msg.code )
    {
      case kbDel: case kbBackSp: return DLG_KEYUSED;
      case kbLeft:  if ( (CPos-1 == 3) || (CPos-1 == 6) ) CPos--; return TEdit::Message(msg);
      case kbRight: if ( (CPos+1 == 3) || (CPos+1 == 6) ) CPos++; return TEdit::Message(msg);
      case kbUp: case kbDown: case kbHome: case kbEnd:
        return TEdit::Message(msg);
      case kbEnter:
        xcode = TEdit::Message(msg);
        if (xcode==DLG_NEXT)
          if (!StrToDate()) { xcode=DLG_KEYUSED; CPos=0; };
        break;
      case kbSpace:
        if (!Editing) break;
        if (!HasFocus) break;
        if (!StrToDate()) DateToday(&Date);
        dt = new TDate(x0, y0+1, &Date);
        dt->Execute();
        delete dt;
        DateToStr();
        Paint();
        xcode=DLG_KEYUSED;
        break;
      default:
        c = msg.code&0x00FF;
        if ( (c>='0') && (c<='9') ) {
          StartEditing=False;
          xcode=TEdit::Message(msg);
          if ( (CPos == 3) || (CPos == 6) ) { CPos++; Paint(); };
        } else return DLG_NOACTION;
    };
  } else return TEdit::Message(msg);
  return xcode;
};
BOOL TEditDate::FlushData( void ) { StrToDate(); *dt=Date; return True; };
void TEditDate::Update( void )
{
  Date = *dt;
  if ( !DateValid(&Date) ) DateToday(&Date);
  DateToStr();
  Paint();
};
void TEditDate::FocusOff( void )  { CPos=1; TDlgItem::FocusOff(); };
void TEditDate::DateToStr( void )
{
  char s[2];
  s[0]=Separator;
  s[1]=0;
  if (YearLen==2) Date.year %= 100;
  IntToDec( Date.year,  SYear,  YearLen );
  IntToDec( Date.month, SMonth, 2, '0' );
  IntToDec( Date.day,   SDay,   2, '0' );
  StrCpy( str,SDay );
  StrAdd( str,s );
  StrAdd( str,SMonth );
  StrAdd( str,s );
  StrAdd( str,SYear );
};
int TEditDate::StrToDate(void)
{
  int i;
  for (i=0; i<2; i++)       SDay[i]=str[i];     SDay[2]=0;
  for (i=0; i<2; i++)       SMonth[i]=str[i+3]; SMonth[2]=0;
  for (i=0; i<YearLen; i++) SYear[i]=str[i+6];  SYear[YearLen]=0;
  Date.day   = StrToInt( SDay );
  Date.month = StrToInt( SMonth );
  Date.year  = StrToInt( SYear );
  if (YearLen==2)
    if (Date.year>50) Date.year+=1900;
    else Date.year+=2000;
  if ( !DateValid(&Date) ) return False;
  return True;
};

// *TCustomList* ----------------------------------------------
TCustomList::TCustomList( int x, int y, int w, int h, long* len, char* title, void (far *draw)(long,int), int (far *keypress)(long,int) )
 : TList( x, y, w, h, (char**)0, len, 0 )
{
  Title  = title;
  iTitle = 0;
  if (title) if (title[0]) iTitle=1;
  y0 += iTitle;
  Draw = draw;
  KeyPress = keypress;
};
int  TCustomList::Message( TMessage& msg )
{
  int xcode,cp;
  if ( msg.what & evBroadcast )
  {
    if ( msg.code==cmListMove )
    {
      if ( msg.target==this )
      {
      CPos = *(int*)msg.dataptr;
      if (CPos>Len-1) CPos=Len-1;
      if (CPos<0) CPos=0;
      if (CPos-SPos >= height) SPos=CPos;
      if (CPos < SPos) SPos=CPos;
      if (SPos>Len-ScrLen) SPos=Len-ScrLen;
      if (SPos<0) SPos=0;
      Paint();
      ShowCursor(True);
      return DLG_NOACTION;
      };
    } else return TList::Message(msg);
  };

  if (msg.what & evKeyb)
  {
    if (!HasFocus) return DLG_NOACTION;

     switch( msg.code )
    {
      case kbLeft: case kbRight: case kbUp: case kbDown:
      case kbPgUp: case kbPgDn: case kbHome: case kbEnd:
        xcode = KeyPress(CPos,msg.code);
        if ( xcode == DLG_NOACTION ) {
          cp=CPos;
          xcode=TList::Message(msg);
          if (CPos!=cp) KeyPress(CPos,cmListMove);
          return xcode;
        } else
          return xcode;
      default:
        return KeyPress(CPos,msg.code);
    };//switch
  };// if
  if (msg.what & evMouse)
  {
    cp=CPos;
    xcode=TList::Message(msg);
    if (CPos!=cp) KeyPress(CPos,cmListMove);
    return xcode;
  };
  return TList::Message(msg);
};
void TCustomList::Paint( void )
{
  int wx,wy,ww,wh;
  int a;

  Len = *pLen;

  MouseHide();
  SaveWindowPos();
  GetWindow(wx,wy,ww,wh);
  SetAttr(cCustomListTitle);
  if (iTitle) {
    SetWindow( wx+x0, wy+y0-iTitle, width-1, 1 );
    SpawnXYC(0,0,cCustomListTitle,' ',width-1);
    WriteXYC(0,0,cCustomListTitle,Title);
  };
  for (int i=0; i<height; i++)
  {
    // scroller bar
    SetWindow( wx+x0+width-1, wy+y0+i, 1, 1 );
    if (Len > height) SpawnXYC(0,0,cScroller,'±',1);
    else SpawnXYC(0,0,cCustomList,' ',1);
    // items
    SetWindow( wx+x0, wy+y0+i, width-1, 1 );
    SpawnXYC(0,0,cCustomList,' ',width-1);
    if (SPos+i < Len)
    {
      if (SPos+i == CPos) {
        if (HasFocus) a=ITEM_FOCUS_ACTIVE;
        else          a=ITEM_FOCUS_INACTIVE;
        Draw(SPos+i,a);
      } else
        Draw(SPos+i,ITEM_NORMAL);
    };
  };
  RestWindowPos();
  ShowScroller();
  MouseShow();
  if (HasFocus) SetCursor(CUR_OFF);
};

void TCustomList::ShowCursor( BOOL what )
{
  int wx,wy,ww,wh;

  int a  = ITEM_NORMAL;

  if (what)
    if (HasFocus)
      if (Len) a=ITEM_FOCUS_ACTIVE; else a=ITEM_EMPTY_ACTIVE;
    else
      if (Len) a=ITEM_FOCUS_INACTIVE; else a=ITEM_EMPTY_INACTIVE;

  MouseHide();
  SaveWindowPos();
  GetWindow(wx,wy,ww,wh);
  SetWindow( wx+x0, wy+y0+(CPos-SPos), width-1, 1 );
  SpawnXYC(0,0,cCustomList,' ',width-1);
  Draw(CPos,a);
  RestWindowPos();
  MouseShow();
  if (HasFocus) SetCursor(CUR_OFF);
};

// *TRectangle* -----------------------------------------------
TRectangle::TRectangle(int rx, int ry, int rw, int rh, char a, char* head)
{
  Attr=a;
  Head=head;
  TabStop = False;
  Default = False;
  TRect::TRect(rx,ry,rx+rw+1,ry+rh+1);
};
void TRectangle::Paint(void)
{
  SetAttr(Attr);
  Rectangle( x0,y0,width,height,SINGLE );
  if (Head)
    if (Head[0]) WriteXYC(2,0,Attr,Head);
};

// *TGraph* ---------------------------------------------------
TGraph::TGraph( int x, int y, byte color, void (far *dr)(void) )
{
  TRect::TRect(x,y,32,8);
  Draw = dr;
  TabStop=False;
  Default=False;
  Inited=False;
  Color=color;
};
void TGraph::Hide(void) { if (Inited) { HideGrWindow(); Inited=False; }; };
TGraph::~TGraph(void) { if (Inited) { HideGrWindow(); Inited=False; }; };
int TGraph::Message( TMessage& /*msg*/ )
{
  /*if (msg.what&evTimer)*/ Paint();
  return DLG_NOACTION;
};
void TGraph::Paint( void )
{
  int wx,wy,ww,wh;
  if (!Inited) {
    Inited=True;
    GetWindow(wx,wy,ww,wh);
    ShowGrWindow(wx+x0,wy+y0,Color);
  };
  Draw();
  WaitRetrace();
  SwapGrBuffer();
};

// *TBigNum* -------------------------------------------------

void TBigNum::TBigNum( int x, int y, int w, long *p, BOOL autotab, int editmode )
 : TEditNum(x,y,w,p,autotab,editmode)
{
  Len+=2; cur_blink=1; cur_tick=0;
};
void TBigNum::TBigNum( int x, int y, int w, int *p, BOOL autotab, int editmode )
 : TEditNum(x,y,w,p,autotab,editmode)
{
  Len+=2; cur_blink=1; cur_tick=0;
};

char* BigChar[] = {
  "  ÜÜ   Û  Û  Û  Û  ßÜÜß ",     // 0
  "   Ü    ÜÛ     Û    ÜÛÜ ",     // 1
  "  ÜÜ   ß  Û   Üß   ÛÜÜÜ ",     // 2
  "  ÜÜ   ß  Û    ßÜ  ßÜÜß ",     // 3
  "  Ü Ü   Û Û  ÛÜÜÛ     Û ",     // 4
  " ÜÜÜÜ  ÛÜÜ      Û  ßÜÜß ",     // 5
  "  ÜÜ   Û  ß  ÛßßÜ  ßÜÜß ",     // 6
  " ÜÜÜÜ    Üß   Üß    Û   ",     // 7
  "  ÜÜ   Û  Û  ÜßßÜ  ßÜÜß ",     // 8
  "  ÜÜ   Û  Û   ßßÛ  ßÜÜß "      // 9
};

#define TBE_XSTEP 6
#define TBE_YSTEP 4
#define TBE_BLINK 80000L
void TBigNum::WriteXYCBig(int ix, int iy, char attr, char *s )
{
  int x,y,cx;

  cx = ix;
  while (*s) {
    for (y=0; y<4; y++)
      for (x=0; x<6; x++)
        SpawnXYC(cx+x,iy+y,attr,BigChar[(*s)-'0'][y*6+x],1);
    SpawnXYC(cx,iy+y,(cDialog&0xF0)+(attr>>4),'ß',6);
    s++;
    cx += TBE_XSTEP;
  };
};

int TBigNum::Message( TMessage &msg )
{
  if (msg.what & evTimer) {
    if (cur_tick++ > TBE_BLINK) {
      cur_tick=0; cur_blink^=1;
      if (HasFocus && Editing) BlinkCursor();
    };
  };
  if (msg.what & evKeyb) {
  };
  return TEdit::Message(msg);
};

void TBigNum::BlinkCursor(void)
{
  int i;
  if (HasFocus && Editing) {
    for (i=0; i<TBE_YSTEP; i++)
      if (cur_blink)
        SpawnXYC(x0+(CPos-SPos-1)*TBE_XSTEP, y0+i, cEditA, 'Ý',1);
      else
        SpawnXYC(x0+(CPos-SPos-1)*TBE_XSTEP, y0+i, cEditA, ' ',1);
  };
};

void TBigNum::Paint(void)
{
  int i,cur_on;
  char s[256];
  char cstr;

  // colors & cursor
  if (HasFocus)
  {
    if (Editing)  { cstr=cEditA; }
    else          { cstr=cEditView; };
  } else {
    cstr=cEdit;
  };

  MouseHide();
  for (i=0; i<TBE_YSTEP; i++)
    SpawnXYC(x0,y0+i,cstr,' ',(Len-2)*TBE_XSTEP+1);
  SpawnXYC(x0,y0+i,(cDialog&0xF0)+(cstr>>4),'ß',(Len-2)*TBE_XSTEP+1);
  WriteXYCBig( x0, y0, cstr, str );
  BlinkCursor();
  MouseShow();
};

// EOF