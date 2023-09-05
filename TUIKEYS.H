#ifndef KEYS_H_
#define KEYS_H_

#define kbEnter    0x1C0D
#define kbEsc      0x011b
#define kbSpace    0x3920
#define kbBackSp   0x0E08
#define kbLeft     0x4B00
#define kbRight    0x4D00
#define kbUp       0x4800
#define kbDown     0x5000
#define kbHome     0x4700
#define kbEnd      0x4F00
#define kbPgUp     0x4900
#define kbPgDn     0x5100
#define kbCtrlPgUp 0x8400
#define kbCtrlPgDn 0x7600
#define kbIns      0x5200
#define kbDel      0x5300
#define kbTab      0x0F09
#define kbShiftTab 0x0F00
#define kbF1       0x3B00
#define kbF2       0x3C00
#define kbF3       0x3D00
#define kbF4       0x3E00
#define kbF5       0x3F00
#define kbF6       0x4000
#define kbF7       0x4100
#define kbF8       0x4200
#define kbF9       0x4300
#define kbF10      0x4400
#define kbCtrlF1   0x5E00
#define kbCtrlF2   0x5F00
#define kbCtrlF3   0x6000
#define kbCtrlF4   0x6000
#define kbCtrlF5   0x6200
#define kbCtrlF6   0x6300
#define kbCtrlF7   0x6400
#define kbCtrlF8   0x6500
#define kbCtrlF9   0x6600
#define kbCtrlF10  0x6700
#define kbCtrlEnter 0x1C0A
#define kbAltF1    0x6800
#define kbAltF2    0x6900
#define kbAltF3    0x6A00
#define kbAltF4    0x6B00
#define kbAltF5    0x6C00
#define kbAltF6    0x6D00
#define kbAltF7    0x6E00
#define kbAltF8    0x6F00
#define kbAltF9    0x7000
#define kbAltF10   0x7100


int KeyPressed( void );
int ReadKey( void );
void Delay( int ticks );
long GetTimer( void );
unsigned int Random(unsigned int max);
unsigned long FreeMem(void);

#endif