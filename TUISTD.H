
#include "tuitypes.h"
#include "tuikeys.h"    // keycodes

#ifndef _TUISTD_H
#define _TUISTD_H

#define ENGLISH 0
#define RUSSIAN 1
extern int TuiLng;      // language (in standard dialogs & calendar)

// Software Timer
#define TIMER_RATE      4

// TEdit constants
#define edView          0x0000
#define edEdit          0x0001
#define edInput         0x0010
//#define edEnter         0x0000

const BOOL AutoTab = True;
const BOOL NoAutoTab = False;

// QuestionBox return values
#define BTN_OK          0x0001
#define BTN_CANCEL      0x0002
#define BTN_YES         0x0004
#define BTN_NO          0x0008

// QuestionBox params
#define QB_YESNO        (BTN_YES | BTN_NO)

// Dialog params
#define DLG_NOACTION    0x0100
#define DLG_KEYUSED     0x0200
#define DLG_OK          0x0400
#define DLG_CANCEL      0x0800
#define DLG_UPDATE      0x1000
#define DLG_NEXT        0x2000
#define DLG_PREV        0x4000

// Event codes
const int evKeyb        = 0x0001;
const int evMouse       = 0x0002;
const int evBroadcast   = 0x0004;
const int evTimer       = 0x0008;
// Mouse codes
const int msMove        = 0x0001;
const int msButton      = 0x0002;
const int msKeyup       = 0x0001;
const int msKeydown     = 0x0002;
const int msIsup        = 0x0004;
const int msIsdown      = 0x0008;
const int msDblClick    = 0x0010;
// Predefined commands
const int cmUpdate      = 0x1000;
const int cmActivate    = 0x2000;
const int cmListMove    = 0x3000;

typedef struct {
  int what;             // evKeyb | evMouse | evBroadcast
  int code;             // keycode | object code | ( msMove | msButton )
  int msx,msy;          // mouse (x,y)
  int lbutton,rbutton;  // msKeyup, msKeydown
  void* sender;         // object address
  void* target;         // object address
  void far* dataptr;    // data pointer
} TMessage;

// INTERNAL

// Get next message / put a message in a queue
void GetMessage( TMessage &msg );
void PostMessage( int put_what, int put_code,
  void* target=0, void *sender=0, void far* dataptr=0 );

// TRect
class TRect {
public:
  int x0,y0,x1,y1;
  int width,height;
public:
  TRect( void );
  TRect( int ix0, int iy0, int ix1, int iy1 );
  BOOL InRect( int x, int y );
};

// TWindow
class TWindow : public TRect
{
private:
  char RColor,CColor,RStyle;
  char header[128];
  char far* buf;
  int bufsize;
  BOOL Shown;
  BOOL HasShadow;
public:
  TWindow( int x, int y, int w, int h, char c, char* head, BOOL shadow );
  virtual ~TWindow( void );
  void Show( void );
  void Hide( void );
  void Rect( char c, char style );
  void Move( int dx, int dy );
  void Shadow( void );
  virtual void Paint( void );
};

// TDlgItem
class TDlgItem : public TRect
{
public:
  TDlgItem *next,*prev;
  void* pData;
  BOOL TabStop;
  BOOL Default;
  BOOL HasFocus;
  BOOL Active;
public:
  TDlgItem( void );
  virtual ~TDlgItem( void );
  virtual void Paint( void );
  virtual void Hide( void );
  virtual void FocusOn( void );
  virtual void FocusOff( void );
  virtual void Update( void );
  virtual BOOL FlushData( void );
  virtual int Message( TMessage &msg );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
  virtual void MoveXY( int, int );
};

// TStatic
class TStatic : public TDlgItem
{
protected:
  char Name[128];
public:
  TStatic( int x, int y, char* n );
  virtual void Paint( void );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
  virtual void Update( void );
};

// TDivider
class TDivider : public TDlgItem
{
protected:
  int Len;
public:
  TDivider( int x, int y, int len );
  virtual void Paint( void );
};

// TButton
class TButton : public TDlgItem
{
protected:
  int NamePos;
  int BtnLen;
  char Name[128];
  void (far *Action)(void);
  int XCode;
  int hotkey;
  int Pushed;
public:
  TButton( int x, int y, char *n, int code, BOOL def=False, int key=0, void far* act=0 );
  virtual void Paint( void );
  virtual int Message( TMessage& );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
private:
  void Push( void );
  void Pop( void );
};

// TEdit
class TEdit : public TDlgItem
{
protected:
  int Len;
  char str[256];
  char dChar;
  int SPos,CPos;
  BOOL InsMode;
  int MaxLen;
  int AutoTab;
  BOOL Editable;
  BOOL EditMode;
  BOOL Editing;
  BOOL StartEditing;
public:
  TEdit( int x, int y, int w, char *s, BOOL autotab=True, int
         editmode=edEdit|edInput, int maxlen=255, int diskreet=0 );
  virtual void Paint( void );
  virtual int  Message( TMessage& );
  virtual void FocusOn(void);
  virtual void FocusOff(void);
  virtual BOOL FlushData( void );
  virtual void Update( void );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
private:
  void ShowLine( void );
  void MoveLeft( void );
  void MoveRight( void );
  void MoveHome( void );
  void MoveEnd( void );
};

// TEditNum
class TEditNum : public TEdit {
private:
  long number;
  int datalen;
public:
  TEditNum( int x, int y, int w, long *p, BOOL autotab=True, int editmode=edEdit|edInput );
  TEditNum( int x, int y, int w, int *p, BOOL autotab=True, int editmode=edEdit|edInput );
  virtual int Message( TMessage& );
  virtual BOOL FlushData(void);
  virtual void Update(void);
};

// TGroup
class TGroup : public TDlgItem {
private:
  TDlgItem* cur;
  int nitems;
  int Xsize,Ysize;
  int grWinx,grWiny,grWinw,grWinh;
public:
  TGroup( int x, int y );
  virtual ~TGroup( void );
  void Insert( TDlgItem* i );
  virtual void FocusOn( void );
  virtual void FocusOff( void );
  virtual BOOL FlushData( void );
  virtual void Update( void );
  virtual void Paint( void );
  virtual int Message( TMessage& );
  void SetWin(void);
  void RestWin(void);
};

// TCheckBox
class TCheckBox : public TDlgItem
{
protected:
  char *Name;
  int checked;
  int hotkey;
public:
  TCheckBox( int x, int y, char *n, int *data, int key=0 );
  virtual ~TCheckBox(void);
  virtual void Paint( void );
  virtual int Message( TMessage& );
  virtual BOOL FlushData( void );
  virtual void Update( void );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
};

// TRadioButton
class TRadioButton : public TDlgItem
{
protected:
  char *Name;
  int checked;
  int value;
  int hotkey;
public:
  TRadioButton( int x, int y, char *n, int *data, int val=1, int key=0 );
  virtual ~TRadioButton(void);
  virtual void Paint( void );
  virtual int Message( TMessage& );
  virtual BOOL FlushData( void );
  virtual void Update( void );
  virtual int GetXsize( void );
  virtual int GetYsize( void );
};

// TList
class TList : public TDlgItem
{
protected:
  char** pItem;
  long *pLen;
  long *pSel;
  BOOL Allocated;           // Len = list length, ScrLen = items on screen
  long Len;
  int  ScrLen,ColLen;        // ColLen = column height (for Left,Right)
  long SPos,CPos;            // SPos = first onscreen item, CPos = cursor item
  unsigned int ScrollPos;
  int lsWinx,lsWiny,lsWinw,lsWinh;
public:
  TList( int x, int y, int w, int h, char *s, long* sel=0 );
  TList( int x, int y, int w, int h, char **ptr, long* len, long* sel=0 );
  virtual ~TList( void );
  virtual void Paint( void );
  virtual void ShowCursor( BOOL );
  virtual void ShowScroller( void );
  virtual void FocusOn( void );
  virtual void FocusOff( void );
  virtual int  Message( TMessage& );
  void Update(void);
private:
  void SetWin(int);
  void RestWin(void);
};

// TColumnList
class TColumnList : public TList
{
protected:
  int NCol,WCol;
public:
  TColumnList( int x, int y, int ncol, int wcol, int h, char *s );
  TColumnList( int x, int y, int ncol, int wcol, int h, char **ptr, long* len );
  virtual void Paint( void );
  virtual int  Message( TMessage& );
  virtual void ShowScroller( void );
  virtual void ShowCursor( BOOL );
private:
  void SaveWin();
  void RestWin();
  void SetCol( int );
};

// TNote
class TNote : public TDlgItem {
protected:
  int nItems;
public:
  TNote( int x, int y, int w, int h, int n );
  void Paint(void);
  int  Message( TMessage& );
  void Update(void);
};

// TDialog
class TDialog : public TWindow
{
protected:
  int nitems;
  TDlgItem *cur;
  int Cx,Cy,CShape;
public:
  TDialog( int x, int y, int w, int h, char* head, BOOL shadow=True );
  virtual ~TDialog( void );
  void Insert( TDlgItem* i );
  virtual void Paint( void );
  virtual void Hide( void );
  virtual int  Message( TMessage& );
  virtual int  FlushData( void );
private:
  void Move( TMessage& );
  int  DispatchMessage(TMessage&);
};

// Global functions ===========================================

int  ExecuteDialog( TDialog* );
void DlgColor( char back, char title, char text, char high );
void DlgReset( void );

#endif
