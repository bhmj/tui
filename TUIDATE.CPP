
#include "tuidate.h"    // struct DATE
#include "tuistr.h"     // IntToDec

int DateLng=ENGLISH;

int MonthDays[] = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
char* WeekDay[2][7] = {
  { "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" },
  { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" }
};
char* Month[2][12] = {
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec" },
  { "января", "февраля", "марта", "апреля", "мая", "июня", "июля",
    "августа", "сентября", "октября", "ноября", "декабря" }
};

int DateDaysInMonth(int m, int y)
{
  return MonthDays[m] + ((y%4==0 && m==2) ? 1 : 0 );
};

int  DateValid( DATE *d )
{
  if ( (d->month<1) || (d->month>12) ) return 0;
  if ( (d->day<1) || (d->day>DateDaysInMonth(d->month,d->year)) ) return 0;
  return 1;
};

DATE* DateToday( DATE *d )
{
  char day,month;
  int year;
  asm {
        mov     ah,2Ah
        int     21h
        mov     day,dl
        mov     month,dh
        mov     year,cx
  };
  d->day = day;
  d->month = month;
  d->year = year;
  return d;
};

int DateWeekday( unsigned day, unsigned month, unsigned year )
{
  int century,yr,dw;

  if (month < 3) {
    month += 10;
    --year;
  } else month -= 2;

  century = year / 100;
  yr = year % 100;
  dw = (((26 * (int)month - 2) / 10) + (int)day + yr + (yr / 4) +
        (century / 4) - (2 * century)) % 7;

  if (dw < 0) dw += 7;   // 0=sun, 1=mon, ..., 6=sat
  dw--; if (dw<0) dw=6;  // 0=mon, 1=tue, ..., 6=sun

  return((unsigned)dw);
};

// 17.10.1977
void DateToShort( DATE *d, char* buf, int yearlen, char separator )
{
  int y=d->year;
  if ( (yearlen!=2) && (yearlen!=4) ) yearlen=4;
  if ( yearlen==2 ) y %= 100;
  buf[0]=0;   IntToDec(d->day,buf,2);
  buf[2]=separator; IntToDec(d->month,buf+3,2);
  buf[5]=separator; IntToDec(y,buf+6,yearlen);
  buf[6+yearlen]=0;
};

// Mon, 17 Oct 1977
// Пн, 17 октября 1977
void DateToLong( DATE *d, char* buf, int yearlen )
{
  int m,i;
  if ( (yearlen!=2) && (yearlen!=4) ) yearlen=4;
  StrCpy( buf, WeekDay[DateLng][ DateWeekday( d->day, d->month, d->year ) ] );
  buf[2]=',';
  buf[3]=' ';
  IntToDec(d->day,buf+4,2);
  m=d->month; i=StrLen(Month[DateLng][m]);
  buf[6]=' '; StrCpy( buf+7, Month[DateLng][m] );
  buf[7+i]=' '; IntToDec(d->day,buf,yearlen);
  buf[8+i+yearlen]=0;
};

int DateCmp( DATE *d0, DATE *d1 )
{
  if (d0->year < d1->year) return -1;
  if (d0->year > d1->year) return +1;
  if (d0->month < d1->month) return -1;
  if (d0->month > d1->month) return +1;
  if (d0->day < d1->day) return -1;
  if (d0->day > d1->day) return +1;
  return 0;
};