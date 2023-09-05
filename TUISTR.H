
#include "tuitypes.h"

#ifndef _STREDIT_H
#define _STREDIT_H

extern "C" { int  StrCmp( char far* str0, char far* str1 ); }
extern "C" { char* StriFind( char* buf, char* str ); }

extern "C" { char* StrUp( char* s ); }
extern "C" { char* StrDown( char* s ); }
extern "C" { char* StrCpy( char* dest, char* src ); }
extern "C" { char* StrNCpy( char* dest, char* src, int n ); }
extern "C" { char* StrAdd( char* dest, char* src, int n=1 ); }
extern "C" { char* StrCut( char* dest, char* s, int x0, int x1 ); }
extern "C" { char* StrEnter( char* dest, char* s, int cpos, char c, int insmode ); }
extern "C" { char* StrDel( char*dest, char* s, int cpos ); }
extern "C" { char* StrBksp( char* dest, char* s, int cpos ); }

extern "C" { int StrLen( char far* s ); }      // full length
extern "C" { int StrLenC( char far* s ); }     // length without '~' and ''
extern "C" { int StrCount( char far* s ); }    // number of strings separated by '\n'
extern "C" { int StrCountZ( char far* s ); }   // number of strings separated by '\0'
extern "C" { char StrFindKey( char far* s ); } // get character placed after first '~'

extern "C" { char* IntToHex( long i, char* s, int len=1, char czero='0' ); }
extern "C" { char* IntToDec( long i, char* s, int len=1, char czero='0' ); }
extern "C" { int   StrToInt( char far* s ); }
extern "C" { long  StrToLong( char far* s ); }

extern "C" { char* AltToWin(char* s); }
extern "C" { char* WinToAlt(char* s); }

#endif