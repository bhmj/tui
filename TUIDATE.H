
#ifndef _TUIDATE_H
#define _TUIDATE_H

typedef struct {
  char day;
  char month;
  int  year;
} DATE;

#ifndef ENGLISH
#define ENGLISH 0
#define RUSSIAN 1
#endif
extern int DateLng;

int   DateDaysInMonth( int m, int y );
int   DateValid( DATE *d );
DATE* DateToday( DATE *d );
int   DateWeekday( unsigned day, unsigned month, unsigned year );
void  DateToShort( DATE *d, char* buf, int yearlen=4, char separator='.' );
void  DateToLong( DATE *d, char* buf, int yearlen=4 );
int   DateCmp( DATE *d0, DATE *d1 );

#endif