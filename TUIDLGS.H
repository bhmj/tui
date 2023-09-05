
#ifndef _TUIDLGS_H
#define _TUIDLGS_H

#define FILE_OPEN       0
#define FILE_SAVE       1

extern "C" { void InitTui(void); }
extern "C" { void ResetTui(void); }

extern "C" { void SetDesktop(char ch, char attr); };
extern "C" { void ShowDesktop( void ); }
extern "C" { void HideDesktop( void ); }
extern "C" { void StatusLine( char* ); }

extern "C" { int  MessageBox( char* head, char* msg ); }
extern "C" { int  ErrorBox( char* head, char* msg ); }
extern "C" { int  QuestionBox( char* head, char* msg, int flags ); }
extern "C" { int  EditBox( char* head, char* msg, char* text, int len=32 ); }
extern "C" { int  PassBox( char* head, char* msg, char* text, int len=32 ); }

extern "C" { int  FileOpen( char* mask, char* fullname ); }
extern "C" { int  FileSave( char* mask, char* fullname ); }

extern "C" { void ProgressBar( unsigned long size, unsigned long pos, char* head, char* msg, unsigned long subsize=0, unsigned long subpos=0 ); }
extern "C" { void HideProgressBar( void ); }

extern "C" { int  MenuList( int x, int y, char*, char* ); }
//extern "C" { int  SelectList( int x, int y, char* ); }
//extern "C" { int  ListList(   int x, int y, char* ); }

extern "C" { void TuiBeep( int freq, int msec ); }

extern "C" { void TuiGreet( void ); }

#endif