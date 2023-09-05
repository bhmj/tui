#include <dir.h>        // findfirst, findnext
#include <alloc.h>      // farcoreleft
#include <search.h>

#include "textio.h"     // WriteXYC
#include "tuimouse.h"   // MouseHide, MouseShow
#include "tuistr.h"     // StrLen
#include "tuigraph.h"   // ShowGrWindow,HideGrWindow

#include "tuistd.h"     // common classes
#include "tuiext.h"     // extended classes
#include "tuidlgs.h"    // extern "C"
#include "tuiconst.h"   // constants & colors

#define MAX_FILES   1000u       // maximum number of files in OpenFile dlg

#define FA_NORMAL   0x00        // Normal file, no attributes
#define FA_RDONLY   0x01        // Read only file
#define FA_HIDDEN   0x02        // Hidden file
#define FA_SYSTEM   0x04        // System file
#define FA_LABEL    0x08        // Volume label
#define FA_DIREC    0x10        // Directory
#define FA_ARCH     0x20        // Archive

char* szYesNo[2][2]    = { { " ~Yes  ","  ~No  " }, { "  ~Да  "," ~Нет  " } };
int   iYesNo[2][2]     = { { 0x1500, 0x3100 }, { 0x2600, 0x1500 } };
char* szOkCancel[2][2] = { { "  ~Ok  ","~Cancel" }, { "  O~k  ","~Отмена" } };
int   iOkCancel[2][2]  = { { 0x1800, 0x2E00 }, { 0x2500, 0x2400 } };

// * Global functions* ========================================
char desktop_char='░', desktop_attr=cDesktop;
void SetDesktop(char ch, char attr) { desktop_char=ch; desktop_attr=attr; };
void ShowDesktop(void)
{
  MouseHide();
  SetAttr(desktop_attr);
  ClearScreen(desktop_char);
  CursorXY(0,0);
  SetCursor(CUR_THIN);
  MouseShow();
  StatusLine("");
};
void HideDesktop(void)
{
  MouseHide();
  SetAttr(cDosWindow);
  ClearScreen(' ');
  CursorXY(0,GetMaxRow());
  SetCursor(CUR_THIN);
//  MouseShow();
};

char statusline[128];
int  statuson=0;
void StatusLine(char* s)
{
  if (!statuson) { SetWindow(0,0,GetMaxCol(),GetMaxRow()-1); statuson=1; };
  StrCpy(statusline,s);
  SaveWindowPos();
  SetWindow(0,GetMaxRow(),GetMaxCol()+1,1);
  SetHiAttr(cStatusHi);
  SpawnXYC(0,0,cStatus,' ',GetMaxCol()+1);
  WriteXYC(1,0,cStatus,statusline);
  RestWindowPos();
};

// *Init/Reset*------------------------------------------------
void InitTui(void) {
  TextInit();
  if ( MouseInit() ) {
    PlaceMouse(GetMaxCol()*4,GetMaxRow()*4);
    SetGraphMouse();
    MouseShow();
  };
};
void ResetTui(void) {
  ResetGraphMouse();
  MouseHide();
};

// *Standard dialogs* -----------------------------------------
char dlg_back=ccDialog;
char dlg_text=ccStaticText;
char dlg_high=ccStaticTextHi;

int ErrorBox( char* head, char* msg )
{
  DlgColor(Red,Yellow,White,White);
  return MessageBox(head,msg);
};

int MessageBox( char* head, char* msg )
{
  TDialog* dlg;
  int hlen,mlen;
  int i,h,len;
  char *p;

//  CursorXY( 0, GetMaxRow() );
  SetCursor(CUR_OFF);
  hlen = StrLen(head);
  mlen = StrLenC(msg);   // the longest string length
  h    = StrCount(msg); // number of strings in message
  if (hlen > mlen) len=hlen+6; else len=mlen+6;
  if (len < 20) len=20;
  dlg = new TDialog( (GetMaxCol()-len)/2, (GetMaxRow()-5-h)/2, len, 4+h, head );
  for (i=0; i<h; i++)
  {
    p=msg; while ( (*p!='\0') && (*p!='\n') ) p++; *p='\0';
    dlg->Insert( new TStatic ( (len-StrLenC(msg)+1)/2, i+1, msg ) );
    if (i!=h-1) *p='\n';
    p++;
    msg=p;
  }
  dlg->Insert( new TButton( (len-8)/2, 2+h, "  Ok  ", DLG_OK, True ) );
  i = ExecuteDialog( dlg );
  delete dlg;
  return i;
};

class TArrowButton : public TButton
{
public:
  TArrowButton( int x, int y, char *n, int code, BOOL def=False, int key=0, void far* act=0 );
  virtual int Message( TMessage &msg );
};
TArrowButton::TArrowButton( int x, int y, char *n, int code, BOOL def, int key, void far* act )
 : TButton( x, y, n, code, def, key, act )
{
};

int TArrowButton::Message( TMessage &msg )
{
  if ( msg.what & evKeyb )
  {
    switch( msg.code )
    {
      case kbRight: return DLG_NEXT;
      case kbLeft:  return DLG_PREV;
      default: return TButton::Message(msg);
    };
  } else return TButton::Message(msg);
};

int BtnCode;
void YesFunc(void) { BtnCode=BTN_YES; };
void NoFunc(void)  { BtnCode=BTN_NO;  };
int QuestionBox( char* head, char* msg, int flags )
{
  TDialog *dlg;
  TArrowButton *btn0,*btn1;
  int hlen,mlen;
  int i,h,len;
  char *p;

  CursorXY( 0, GetMaxRow() );
  SetCursor(CUR_OFF);
  hlen = StrLen(head);
  mlen = StrLenC(msg);   // the longest string length
  h    = StrCount(msg); // number of strings in message
  if (hlen > mlen) len=hlen+6; else len=mlen+6;
  if (len < 30) len=30;

  dlg = new TDialog( (GetMaxCol()-len)/2, (GetMaxRow()-5-h)/2, len, 4+h, head );
  for (i=0; i<h; i++)
  {
    p=msg; while ( (*p!='\0') && (*p!='\n') ) p++; *p='\0';
    dlg->Insert( new TStatic( (len-StrLenC(msg))/2, i+1, msg ) );
    if (i!=h-1) *p='\n';
    p++;
    msg=p;
  };

  if (flags & QB_YESNO) {
    btn0 = new TArrowButton( (len-19)/2, 2+h, szYesNo[TuiLng][0], DLG_OK, True, iYesNo[TuiLng][0], YesFunc );
    btn1 = new TArrowButton( (len-19)/2+8+3, 2+h, szYesNo[TuiLng][1], DLG_OK, False, iYesNo[TuiLng][1], NoFunc );
    dlg->Insert( btn0 );
    dlg->Insert( btn1 );
  }

  BtnCode=DLG_CANCEL;
  ExecuteDialog( dlg );
  delete dlg;

  return BtnCode;
};

int EditBox( char* head, char* msg, char* text, int len )
{
  TDialog *dlg;
  int boxwidth,boxheight,edity,x;

  boxwidth = len+4;
  if ( boxwidth < StrLenC(msg) ) boxwidth=StrLenC(msg)+4;
  if (boxwidth > 48) boxwidth=48;
  if (boxwidth < 20) boxwidth=20;
  if (!msg) boxheight=5; else boxheight=6;

  dlg = new TDialog( (GetMaxCol()-boxwidth)/2, (GetMaxRow()-boxheight)/2,
                     boxwidth, boxheight, head );
  if (msg) dlg->Insert( new TStatic( 2,1, msg ) );
  if (msg) edity=2; else edity=1;
  dlg->Insert( new TEdit( 2,edity,len,text,False ) );
  dlg->Insert( new TButton( (boxwidth-19)/2, edity+2, szOkCancel[TuiLng][0], DLG_OK, True ) );
  dlg->Insert( new TButton( (boxwidth-19)/2+8+3, edity+2, szOkCancel[TuiLng][1], DLG_CANCEL ) );

  x = ExecuteDialog( dlg );
  if ( x==DLG_OK ) x=StrLen(text); else x=0;

  delete dlg;

  return x;
}

int PassBox( char* head, char* msg, char* text, int len )
{
  TDialog *dlg;
  int boxwidth,boxheight,edity,x;

  *text=0;
  boxwidth = len+4;
  if ( boxwidth < StrLenC(msg) ) boxwidth=StrLenC(msg)+4;
  if (boxwidth > 48) boxwidth=48;
  if (boxwidth < 20) boxwidth=20;
  if (!msg) boxheight=5; else boxheight=6;

  dlg = new TDialog( (GetMaxCol()-boxwidth)/2, (GetMaxRow()-boxheight)/2,
                     boxwidth, boxheight, head );
  if (msg) dlg->Insert( new TStatic( 2,1, msg ) );
  if (msg) edity=2; else edity=1;
  dlg->Insert( new TEdit( 2,edity,len,text,False,edEdit|edInput,32,'*' ) );
  dlg->Insert( new TButton( (boxwidth-19)/2, edity+2, szOkCancel[TuiLng][0], DLG_OK, True ) );
  dlg->Insert( new TButton( (boxwidth-19)/2+8+3, edity+2, szOkCancel[TuiLng][1], DLG_CANCEL ) );

  x = ExecuteDialog( dlg );
  if ( x==DLG_OK ) x=StrLen(text); else x=0;

  delete dlg;

  return x;
}

// File Open / File Save dialogs ------------------------------

typedef struct {
  char Fname[13];
  char Flags;
  long Fsize;
  int  Fdate;
  int  Ftime;
} FILEREC;                      // char *a
typedef FILEREC* PFILEREC;      //
long nFiles=0;
PFILEREC* FileList;             // char** a
unsigned int flistlen;

BOOL IsWildcard(char* s)
{
  while(*s)
    if ( (*s=='*') || (*s=='?') ) return True;
    else s++;
  return False;
};

void InitFileBuf(void) {
  long i;
  i = FreeMem();
  i /= sizeof(FILEREC)+sizeof(PFILEREC);
  if (i>MAX_FILES) i=MAX_FILES;
  flistlen = i;
  FileList = new PFILEREC[i];
  for (i=0; i<flistlen; i++) FileList[i] = new FILEREC;
};
void ResetFileBuf(void) {
  if (flistlen) {
    for (int i=0; i<flistlen; i++) delete FileList[i];
    delete FileList;
  };
  nFiles=0;
};
int S_Files(const void*,const void*);
int S_Files(const void* p1, const void* p2)
{
  return StrCmp(((FILEREC*)p1)->Fname,((FILEREC*)p2)->Fname);
};
void FindFiles( char* mask )
{
  ffblk findrec;
  int i,done;

//  ResetFileBuf();
  // count files
  done = findfirst( mask, &findrec, FA_DIREC|FA_ARCH );
  while (!done) { nFiles++; done = findnext( &findrec ); };
  // count directories
  done = findfirst( "*.*", &findrec, FA_DIREC );
  while (!done) {
    if ( findrec.ff_attrib&FA_DIREC )
      if ( StrCmp(findrec.ff_name,".") != 0 ) nFiles++;
    done = findnext( &findrec );
  };
  // store directories
  i = 0;
  done = findfirst( "*.*", &findrec, FA_DIREC );
  while (!done) {
    if ( findrec.ff_attrib&FA_DIREC )
      if ( StrCmp(findrec.ff_name,".") != 0 )
      {
        StrCpy( FileList[i]->Fname, findrec.ff_name );
        StrAdd( FileList[i]->Fname, "\\", 1 );
        FileList[i]->Fdate = findrec.ff_fdate;
        FileList[i]->Ftime = findrec.ff_ftime;
        FileList[i]->Flags = FA_DIREC;
        i++;
      };
    done = findnext( &findrec );
  };
  // store files
  done = findfirst( mask, &findrec, FA_ARCH );
  while (!done) {
    if ( !(findrec.ff_attrib&FA_DIREC) )
    {
      StrCpy( FileList[i]->Fname, findrec.ff_name );
      StrDown( &FileList[i]->Fname[0] );
      FileList[i]->Fsize = findrec.ff_fsize;
      FileList[i]->Fdate = findrec.ff_fdate;
      FileList[i]->Ftime = findrec.ff_ftime;
      FileList[i]->Flags = 0;
      if (i++ >= MAX_FILES) break;
    }
    done = findnext( &findrec );
  };
//  qsort( FileList, sizeof(PFILEREC), i, S_Files );
  nFiles = i;
};

char pathinfo[128];
char fileinfo[128];
#define FILE_UPDATE  10
#define PATH_UPDATE  11

char buf[128];
char fmask[64];
int iFile=0;
TEdit* edt;
TColumnList* list;
void UpdatePanel(void)
{
  edt->Update();
  list->Update();
};

void GetDirInfo(void)
{
  StrCpy(pathinfo,"X:\\");
  pathinfo[0]=getdisk()+'A';
  getcurdir( 0, pathinfo+3 );
  StrAdd(pathinfo,"\\");
  StrAdd(pathinfo,fmask);
};

class TFileInfo : public TNote {
public:
  TFileInfo(int x, int y, int w, int h);
  void Paint(void);
  int  Message(TMessage&);
};
TFileInfo::TFileInfo(int x, int y, int w, int h)
 : TNote(x,y,w,h,0) {};
#define cFileInfo 0x13
void TFileInfo::Paint(void)
{
  char s[64];
  int i;
  DATE d;

  SpawnXYC(x0,y0+0,cFileInfo,' ',width);
  SpawnXYC(x0,y0+1,cFileInfo,' ',width);
  WriteXYC(x0+1,y0+0,cFileInfo,pathinfo);
  WriteXYC(x0+1, y0+1,cFileInfo,FileList[iFile]->Fname);
  if (FileList[iFile]->Flags==FA_DIREC)
    WriteXYC(x0+14,y0+1,cFileInfo,"Dir");
  else
    WriteXYC(x0+14,y0+1,cFileInfo,IntToDec(FileList[iFile]->Fsize,s));
  i=FileList[iFile]->Fdate;
  d.day  = i&0x1F;
  d.month= (i>>5)&0xF;
  d.year = ((i>>9)&0x7F)+1980;
  DateToShort(&d,s,4,'-');
  WriteXYC(x0+23,y0+1,cFileInfo,s);
  i=FileList[iFile]->Ftime;
  WriteXYC(x0+34,y0+1,cFileInfo,IntToDec((i>>11)&0x1F,s,2));
  WriteXYC(x0+36,y0+1,cFileInfo,":");
  WriteXYC(x0+37,y0+1,cFileInfo,IntToDec((i>>5)&0x3F,s,2));
};
int TFileInfo::Message(TMessage& msg)
{
  if (msg.what&evBroadcast)
  {
    switch( msg.code )
    {
      case FILE_UPDATE: case PATH_UPDATE:
        Paint();
        return DLG_NOACTION;
      default:
        return TNote::Message(msg);
    };
  } else return TNote::Message(msg);
};

class TFileNameEdit : public TEdit
{
public:
  BOOL OpenSave;
public:
  TFileNameEdit( int x, int y, int w, char* buf, BOOL opensave );
  virtual int Message( TMessage& );
};

TFileNameEdit::TFileNameEdit( int x, int y, int w, char* buf, BOOL opensave )
 : TEdit( x, y, w, buf, True, edEdit|edInput, 64 )
{
  OpenSave = opensave;
};
int TFileNameEdit::Message( TMessage& msg )
{
  if (msg.what & evKeyb)
  {
    StrCpy(buf,str);
    switch( msg.code )
    {
      case kbEnter:
        if ( (!IsWildcard(str) ) && (OpenSave==FILE_SAVE) ) return DLG_NOACTION;
        else { // list
          if (*str==0) StrCpy(str,"*.*");
          StrCpy(fmask,str);
          GetDirInfo();
          FindFiles(str);
          StrCpy(buf,str);
          UpdatePanel();
          iFile=0;
          PostMessage(evBroadcast,FILE_UPDATE);
          return DLG_NEXT;
        };
      default:
        return TEdit::Message(msg);
    };
  } else return TEdit::Message(msg);
};

class TFileNameList : public TColumnList
{
public:
  TFileNameList( int x, int y, int ncol, int wcol, int h, char** buf, long* len );
  virtual int Message( TMessage& );
  virtual void Update(void);
};

TFileNameList::TFileNameList( int x, int y, int ncol, int wcol, int h, char** buf, long* len )
 : TColumnList( x, y, ncol, wcol, h, buf, len ) {};

int TFileNameList::Message( TMessage& msg )
{
  char tmps[16];
  int xcode;
  if (msg.what & evKeyb)
  {
    switch( msg.code )
    {
      case kbBackSp:
        StrCpy(tmps,"..");
        chdir(tmps);
        FindFiles(buf);
        GetDirInfo();
        iFile=0;
        PostMessage(evBroadcast,FILE_UPDATE);
        UpdatePanel();
        return DLG_KEYUSED;
      case kbEnter:
        if (!Len) return DLG_KEYUSED;
        if ( FileList[CPos]->Flags&FA_DIREC )
        {
          StrCpy(tmps,FileList[CPos]->Fname);
          tmps[StrLen(FileList[CPos]->Fname)-1]=0;
          chdir(tmps);
          FindFiles(buf);
          GetDirInfo();
          iFile=0;
          PostMessage(evBroadcast,FILE_UPDATE);
          UpdatePanel();
          return DLG_KEYUSED;
        };
        StrCpy(buf,pItem[CPos]);
        edt->Update();
        return DLG_NOACTION;
      default:
        xcode=TColumnList::Message(msg);
        iFile=CPos;
        PostMessage(evBroadcast,FILE_UPDATE);
        return xcode;
    };
  };
  if (msg.what&evMouse)
    if(msg.code&msDblClick) {
      PostMessage( evKeyb, kbEnter );
      return DLG_KEYUSED;
    } else return TColumnList::Message(msg);
  return TColumnList::Message(msg);
};
void TFileNameList::Update(void) { CPos=SPos=0; TColumnList::Update(); };

char curdir[100];
void SaveDir(void) {
  StrCpy(curdir,"X:\\");
  curdir[0]=getdisk()+'A';
  getcurdir( 0, curdir+3 );
};
void RestDir(void) {
  setdisk( curdir[0]-'A' );
  chdir( curdir );
};

char* szWinTitle[2][2] = {
  { "Open file", "Save file" },
  { "Открыть", "Сохранить" }
};
char* szComment[2] = { "File mask", "Имя файла" };
char* szOkButton[2][2] = {
  { " Open ", " Save " },
  { "Открыть", "Сохранить" }
};
char* szCancelButton[2] = { "Cancel", "Отмена " };

int FileOpenSave( char* mask, char* fullname, int mode )
{
  TDialog *dlg;
  int xcode;

  SaveDir();

  InitFileBuf();
  FindFiles(mask);
  StrCpy(buf,mask);

  StrCpy(fmask,mask);
  GetDirInfo();

  edt  = new TFileNameEdit( 2, 2, 29, buf, (BOOL)mode );
  list = new TFileNameList( 2, 4, 2, 15, 7, (char**)FileList, &nFiles );

  dlg = new TDialog( 15, 4, 44+TuiLng, 14, szWinTitle[TuiLng][mode] );
  dlg->Insert( new TStatic( 2, 1, szComment[TuiLng] ) );
  dlg->Insert( edt );
  dlg->Insert( list );
  dlg->Insert( new TFileInfo(2,12,40,2) );
  dlg->Insert( new TButton( 33, 2, szOkButton[TuiLng][mode], DLG_OK, True ) );
  dlg->Insert( new TButton( 33, 4, szCancelButton[TuiLng], DLG_CANCEL ) );

  xcode = ExecuteDialog( dlg );
  delete dlg;

  if (IsWildcard(buf)) xcode=DLG_CANCEL;

  StrCpy(fullname,"X:\\");
  fullname[0]=getdisk()+'A';
  getcurdir( 0, fullname+3 );
  StrAdd( fullname, "\\", 1 );
  StrAdd( fullname, buf, 1 );

  ResetFileBuf();

  RestDir();

  return xcode;
};

int FileOpen( char* mask, char* fullname )
{
  return FileOpenSave( mask, fullname, FILE_OPEN );
};

int FileSave( char* mask, char* fullname )
{
  return FileOpenSave( mask, fullname, FILE_SAVE );
};

// *ProgressBar* ----------------------------------------------

TWindow* pbar=0;
long prevpos,prevsubpos;
unsigned int prevbarlen;
unsigned int prevbartime;

#define PBAR_W          60
#define PBAR_H          3
#define PBAR_LEN        50

void ProgressBar( unsigned long size, unsigned long pos, char* head, char* msg, dword subsize, dword subpos )
{
  int i,y,newbar=0;
  int is_msg=0;
  unsigned int barlen,bdiv2,bmod2,bartime;
  char buf[8];
  int h=0;

  // Write not more then 18 times per sec
  asm {
    push es
    xor  ax,ax
    mov  es,ax
    mov  bx,46Ch
    mov  ax,[word ptr es:bx]
    mov  bartime,ax
    pop  es
  };
  if (prevbartime==bartime) return;
  prevbartime=bartime;
  if (msg) if (*msg) is_msg++;
  if (!pbar)
  { // window
    if (subsize>0) h=2; else h=0;
    SaveWindowPos();
    pbar = new TWindow(
      (GetMaxCol()-PBAR_W)/2, (GetMaxRow()-PBAR_H-h-is_msg)/2,
      PBAR_W, PBAR_H+h+is_msg, cProgressWin, head, True );
    pbar->Show();
    RestWindowPos();
    newbar=1;
    prevpos=size;
    prevsubpos=subsize;
    prevbarlen=0;
  };
  // no changes
  if (pos==prevpos && subpos==prevsubpos) return;
  // > 100% ?
  if (pos>=size) pos=size;
  // exceptions
  if (size==0) { size=pos=1; };
  if (subpos>=subsize) subpos=subsize;
  // begin drawing
  SaveWindowPos();
  SetWindow( pbar->x0+1, pbar->y0+1, pbar->width-2, pbar->height-1 );
  // new loop
  y = (subsize>0?2:0);
  if (prevpos>pos)
    SpawnXYC( 1, 1+2+is_msg+y, cProgressWin, '═', 14 );
  // previous state
  prevpos=pos;
  prevsubpos=subpos;
  // status message
  if (newbar) if (is_msg)  WriteXYC( 2, 1, cProgressWin, msg );
  // bar
  barlen = (unsigned int)((PBAR_LEN*2*pos)/size);
  if (prevbarlen!=barlen) {
    bdiv2 = barlen / 2;
    bmod2 = barlen % 2;
    SpawnXYC( 2, 1+is_msg, cProgressBar, '█', bdiv2 );
    SpawnXYC( 2+bdiv2, 1+is_msg, cProgressBar, '▌', bmod2 );
    SpawnXYC( 2+bdiv2+bmod2, 1+is_msg, cProgressBar, ' ', PBAR_LEN-bdiv2-bmod2 );
    // percent
    IntToDec(barlen,buf,3,' ');
    StrAdd(buf,"%",1);
    barlen = (unsigned int)((100*pos)/size);
    WriteXYC( 2+PBAR_LEN, 1+is_msg, cProgressNum, buf );
  };
  prevbarlen=barlen;
  // numbers
  y = (subsize>0?2:0);
  IntToDec(pos,buf);
  WriteXYC( 1, 1+2+is_msg+y, cProgressWin, buf );
  i = StrLen(buf);
  IntToDec(size,buf);
  SpawnXYC( 1+i, 1+2+is_msg+y, cProgressWin, '/', 1 );
  WriteXYC(  1+i+1, 1+2+is_msg+y, cProgressWin, buf );

  if (subsize>0) {
    // sub: bar
    barlen = (unsigned int)((PBAR_LEN*2*subpos)/subsize);
    bdiv2 = barlen / 2;
    bmod2 = barlen % 2;
    SpawnXYC( 2, 1+2+is_msg, cProgressBar, '█', bdiv2 );
    SpawnXYC( 2+bdiv2, 1+2+is_msg, cProgressBar, '▌', bmod2 );
    SpawnXYC( 2+bdiv2+bmod2, 1+2+is_msg, cProgressBar, ' ', PBAR_LEN-bdiv2-bmod2 );
    SpawnXYC( 2+PBAR_LEN, 1+2+is_msg, cProgressWin, ' ', 3 );
    // sub: percent
    barlen = (unsigned int)((100*subpos)/subsize);
    IntToDec(barlen,buf,3,' ');
    StrAdd(buf,"%",1);
    WriteXYC( 2+PBAR_LEN, 1+2+is_msg, cProgressNum, buf );
  };

  RestWindowPos();
};

void HideProgressBar( void )
{
  if (pbar) delete pbar;
  pbar=0;
};

// *MenuList* -------------------------------------------------

typedef char* pchar;
pchar *PMenuList=0;
char MLHotkey[32];
long NItems;
int  SelectedItem;

char MenuColor[2][3] = {
  { cMenuListNorm, cMenuListFocus, cMenuListSel },
  { cMenuListNormHi, cMenuListFocusHi, cMenuListSelHi }
};
void far MLDraw( long i, int n )
{
  SpawnXYC( 0,0, MenuColor[0][n], ' ', 80 );
  SetHiAttr( MenuColor[1][n] );
  WriteXYC( 1,0, MenuColor[0][n], PMenuList[i] );
};
int far MLKeyPress( long i, int code )
{
  int n,c=code&0xFF;
  for (n=0; n<NItems; n++)
    if (MLHotkey[n])
      if (MLHotkey[n]==c) { SelectedItem=n; return DLG_OK; };

  if (code==kbEnter)
  {
    SelectedItem = i;
    return DLG_OK;
  } else return DLG_NOACTION;
};

class TMenuList : public TCustomList
{
private:
  int oldC;
public:
  TMenuList( int x, int y, int w, int h, long *n, void (far* draw)(long,int), int (far* key)(long,int) );
  int Message( TMessage& );
};
TMenuList::TMenuList( int x, int y, int w, int h, long *n, void (far* draw)(long,int), int (far* key)(long,int) )
 : TCustomList(x,y,w,h,n,0,draw,key) {};
int TMenuList::Message( TMessage& msg )
{
  int xcode;
  oldC = CPos;
  xcode=TCustomList::Message(msg);
  if (msg.what==evKeyb)
  {
    if ( oldC==CPos )
    {
      if (msg.code==kbUp)   { PostMessage(evKeyb,kbEnd); Paint(); };
      if (msg.code==kbDown) { PostMessage(evKeyb,kbHome); Paint(); };
    };
  };
  return xcode;
};
int MenuList( int x, int y, char* str, char* head )
{
  char* p;
  int i;
  int w,h;
  TDialog* dlg;

  if (!str) return -1;

  NItems=0;
  // count menu items
  p=str;
  while (*p) { p++; if (*p==0) { NItems++; p++; }; };
  PMenuList = new pchar[NItems];
  // save menu items
  p=str; for (i=0; i<NItems; i++) {
    PMenuList[i]=p;
    MLHotkey[i] = StrFindKey(p);
    while (*p) p++; p++; // next string
  };
  // calculate width
  w=1;
  for (i=0; i<NItems; i++)
    if (StrLenC(PMenuList[i]) > w) w = StrLenC(PMenuList[i]);
  w+=3;

  h=NItems;
  if (h>GetMaxRow()-1) { h=GetMaxRow()-1; };
  dlg = new TDialog( x, y, w+1, h, head );
  dlg->Insert( new TMenuList( 1,0,w,h,&NItems,MLDraw,MLKeyPress ) );
  if ( ExecuteDialog(dlg)==DLG_OK ) i=SelectedItem; else i=-1;
  delete dlg;
  delete PMenuList;
  return i;
};

// Intro (secrect part :) -------------------------------------

#define MAX_POINTS      8
#define MAX_TRAILS      8
int  x[MAX_TRAILS][MAX_POINTS], y[MAX_TRAILS][MAX_POINTS];
int  dx[MAX_POINTS], dy[MAX_POINTS];
int pt,ptprev;
int lnum,pnum;
byte color;

void InitIntro(void)
{
  int i,p;
  for (i=0; i<MAX_TRAILS; i++)
    for (p=0; p<MAX_POINTS; p++)
    {
      x[i][p]=-1;
      y[i][p]=-1;
    }
  for (i=0; i<MAX_POINTS; i++)
  {
    x[0][i]=Random(256);
    y[0][i]=Random(128);
    dx[i]=Random(4)+1;
    dy[i]=Random(3)+1;
  };
  lnum = 5;
  pnum = 5;
  ptprev = 0;
  pt     = 1;
  color  = 1;
};
void far Intro(void)
{
  int p;

  // erase last line in trail
  for (p=0; p<lnum-1; p++) GrLine(x[pt][p],y[pt][p],x[pt][p+1],y[pt][p+1],0);
  GrLine(x[pt][p],y[pt][p],x[pt][0],y[pt][0],0);
  // move points
  for (p=0; p<lnum; p++) {
    if (x[ptprev][p] >= 256 || x[ptprev][p] <= 0) { dx[p]=-dx[p]; if (x[ptprev][p]<=0) color=(color+1)&0x0F; };
    if (y[ptprev][p] >= 128 || y[ptprev][p] <= 0) dy[p]=-dy[p];
    x[pt][p] = x[ptprev][p]+dx[p];
    y[pt][p] = y[ptprev][p]+dy[p];
  };
  // draw the line
  for (p=0; p<lnum-1; p++) GrLine(x[pt][p],y[pt][p],x[pt][p+1],y[pt][p+1],color);
  GrLine(x[pt][p],y[pt][p],x[pt][0],y[pt][0],color);
  // next line
  ptprev = pt;
  pt = (pt+1==pnum?0:pt+1);
};

#pragma warn -par
void TuiBeep( int freq, int msec )
{
};
#pragma warn +par

void TuiGreet(void)
{
  TDialog *dlg;

  InitIntro();
  dlg = new TDialog( 22,5,32,11,"TUI by BlockHead" );
  dlg->Insert( new TButton(12,9,"  Ok  ",DLG_OK,True) );
  dlg->Insert( new TGraph(0,0,1,Intro) );
  ExecuteDialog(dlg);
  delete dlg;
};