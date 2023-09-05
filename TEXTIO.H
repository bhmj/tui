
#ifndef _TIO_H
#define _TIO_H

#ifndef WAIT_RETRACE
#define WAIT_RETRACE
#endif

#define Black           0
#define Blue            1
#define Green           2
#define Cyan            3
#define Red             4
#define Magenta         5
#define Brown           6
#define LightGray       7
#define Gray            7
#define DarkGray        8
#define LightBluze      9
#define LightGreen      10
#define LightCyan       11
#define LightRed        12
#define LightMagenta    13
#define Yellow          14
#define White           15

enum { SINGLE=0, DOUBLE=1, BLOCK=2 };
enum { CUR_OFF=0, CUR_ON=1, CUR_THIN=1, CUR_BLOCK=2 };

extern long VideoPtr;         // video buffer address

void TextInit( void );        // init all TextIO variables

// Video modes
void SetVideoMode( int m );
int  GetVideoMode( void );
int  GetMaxCol( void );
int  GetMaxRow( void );
// Misc video
void WaitRetrace(void);
// Video pages
void SetActivePage(int p);
void SetVisualPage(int p);
int  GetActivePage(void);
int  GetVisualPage(void);
void CopyPage(int pdest, int psrc);
// Colors
void SetTextColor( char c );
void SetBackColor( char c );
void SetAttr( char a );
void SetHiTextColor( char c );
void SetHiBackColor( char c );
void SetHiAttr( char a );
// Windows
void GetWindow( int& x, int& y, int& w, int& h );
void SetWindow( int x, int y, int w, int h );
void SaveWindowPos( void );
void RestWindowPos( void );
void SetPrevWindow( void );
void ResetWindow( void );       // whole screen
void ClearWindow( char c );
// Region save/restore
int  GetWindowSize( int w, int h );
void SaveWindow( int x, int y, int w, int h, char far* p );
void RestoreWindow ( int x, int y, char far* p );
void PasteBufBuf( int x, int y, int w, int h, char far* p0, int tx, int ty, char far* p1 );
void PasteBufWindow( int x, int y, int w, int h, int tx, int ty, char far* p );
// Video buffer manipulation
void SetVideoBuf( void far* );
void GetVideoBuf( long* );
// Absolute output
void ClearScreen( char c );
//Window output
void Rectangle( int x, int y, int w, int h, int style );
void WriteXY( int x, int y, char far* s );
void WriteXYC( int x, int y, char c, char far*s );
void SpawnXYC( int x, int y, char c, char s, int len );
void AttrXY( int x, int y, char c, int len );
// Graphics output

// Misc
void CursorXY( int x, int y );
int GetCursorX( void );
int GetCursorY( void );
int GetCursorShape( void );
void SetCursor( int x ); // { X = CUR_OFF, CUR_ON, CUR_THICK, CUR_BLOCK }

// User window

void Window( int x, int y, int w, int h, char* head, int style, char wa=0, char ha=0, char ra=0 );

#endif
