
#include "tuiconst.h"   // colors
#include "tuistd.h"     // TDialog, dialog items
#include "tuidate.h"    // struct DATE

#ifndef _TUIEXT_H
#define _TUIEXT_H

extern char *szMonthName[2][13];

// TCustomList item types
#define ITEM_NORMAL             0x00
#define ITEM_FOCUS_ACTIVE       0x01
#define ITEM_FOCUS_INACTIVE     0x02
#define ITEM_EMPTY_ACTIVE       0x03
#define ITEM_EMPTY_INACTIVE     0x04

// TDate ------------------------------------------------------
class TDate : public TRect {
private:
  DATE Date;
  DATE* dt;
  int VisYear,VisMonth;
  int Cx,Cy,Day;
public:
  TDate( int x, int y, DATE *date );
  void Execute( void );
  void Paint();
  void ShowCursor(BOOL);
private:
  void MoveDays(int);
  void NextMonth(void);
  void PrevMonth(void);
  void NextYear(void);
  void PrevYear(void);
};

// TEditDate --------------------------------------------------
class TEditDate : public TEdit {
private:
  DATE Date;
  DATE* dt;
  int YearLen;
  char Separator;
  char SYear[6];
  char SMonth[4];
  char SDay[4];
public:
  TEditDate( int x, int y, DATE* date, BOOL autotab=True, int editmode=edEdit|edInput, int yearlen=4, char separator='.' );
  virtual void Paint();
  virtual int  Message( TMessage& );
  virtual BOOL FlushData( void );
  virtual void Update( void );
  virtual void FocusOff( void );
private:
  int StrToDate( void );
  void DateToStr( void );
};

// *TCustomList* ----------------------------------------------
class TCustomList : public TList
{
protected:
  char *Title;
  int iTitle;
  void (far* Draw)(long,int);
  int (far* KeyPress)(long,int);
public:
  TCustomList( int x, int y, int w, int h, long* len, char* title, void (far *draw)(long,int), int (far *keypress)(long,int) );
  virtual int Message( TMessage& );
  virtual void Paint( void );
  virtual void ShowCursor(BOOL);
};

// *TRectangle* -----------------------------------------------
class TRectangle : public TDlgItem
{
protected:
  char Attr;
  char* Head;
public:
  TRectangle( int rx, int ry, int rw, int rh, char a=cDialog, char *head="" );
  virtual void Paint(void);
};
// *TGraph* ---------------------------------------------------
class TGraph : public TDlgItem
{
protected:
  void (far *Draw)(void);
  BOOL Inited;
  byte Color;
public:
  TGraph( int x, int y, byte color, void (far*)(void) );
  ~TGraph(void);
  virtual int Message( TMessage& );
  virtual void Paint( void );
  virtual void Hide( void );
};

// *TBigNum* -------------------------------------------------
class TBigNum : public TEditNum {
private:
  int  cur_blink;
  long cur_tick;
  void WriteXYCBig(int,int,char,char*);
  void BlinkCursor(void);
public:
  TBigNum( int x, int y, int w, long *p, BOOL autotab=True, int editmode=edEdit|edInput );
  TBigNum( int x, int y, int w, int *p, BOOL autotab=True, int editmode=edEdit|edInput );
  virtual void Paint( void );
  virtual int Message( TMessage &msg );
};


#endif
