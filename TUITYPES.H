
#ifndef _TUITYPES_H
#define _TUITYPES_H

// standard types
typedef unsigned char      ubyte;
typedef signed   char      sbyte;
typedef unsigned short int uword;
typedef signed   short int sword;
typedef unsigned long      ulong;
typedef signed   long      slong;
// extended types
typedef ubyte byte;
typedef uword word;
typedef ulong dword;
typedef enum { False, True } BOOL;
// memory
#if defined(__COMPACT__)
#define FARDATA
#elif defined(__LARGE__)
#define FARDATA
#elif defined(__HUGE__)
#define FARDATA
#else
#define NEARDATA
#endif
// defines
#define MAKE_FP( s,o ) ( (void far*)( (((dword)s)<<16)+(((dword)o & 0x0000FFFFL)) ) )
#define Abs(x) ((x)>0?(x):-(x))
#define LABEL(x) } x: asm {
#ifndef NULL
#define NULL 0
#endif

#endif