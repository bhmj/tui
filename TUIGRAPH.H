
#ifndef _TUIGRAPH_H_
#define _TUIGRAPH_H_

#include "tuitypes.h"

int  ShowGrWindow(int xpos, int ypos, byte color);
void HideGrWindow(void);
void ClearGrWindow(byte color);
void GrPixel(int x, int y, byte color);
void GrCircle(int x, int y, int r, byte color);
void GrLine(int x0, int y0, int x1, int y1, byte color);
void ClearGrBuffer(void);
void SwapGrBuffer(void);
void GrWaitRetrace(void);

#endif