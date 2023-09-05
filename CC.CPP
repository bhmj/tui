#include <tui.h>

void main(void)
{
  DATE* d;

  DateToday(d);

  InitTui();
  TuiLng = ENGLISH;

  TDialog* dlg;

  dlg = new TDialog(24,9,28,3,"Calendar");
  dlg->Insert( new TEditDate(2,1,d) );
  dlg->Insert( new TButton(16,1," Close ",DLG_OK,True) );
  ExecuteDialog(dlg);
  delete dlg;
};