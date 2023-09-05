
#ifndef _MOUSE_H
#define _MOUSE_H

int  MouseInit( void );
void MouseShow( void );
void MouseHide( void );
void MouseGetStatus( int &x, int &y, int& buttons );
void PlaceMouse(int x, int y );

void SetGraphMouse(void);
void ResetGraphMouse(void);

#endif