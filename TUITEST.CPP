#include <alloc.h>      // alloc
#include <stdio.h>      // alloc
#include <dos.h>
#include <dir.h>
#include "tuistr.h"     // strings

#include "tuistd.h"     // basic components
#include "tuiext.h"     // extended components
#include "tuidlgs.h"    // standard dialogs
#include "textio.h"
#include "tuiconst.h"

void far WaitTest(void)
{
  int i,answer;
  answer = QuestionBox( "Question Box", "Do you want to pass the waiting test?", QB_YESNO );
  if (answer==BTN_YES)
  {
    for (i=0; i<100; i++)
    {
      ProgressBar( 100, i+1, "Progress", "Waiting..." );
      Delay(1);
    }
    MessageBox("Message","Test passed");
    HideProgressBar();
  }
  return;
};

void far cDraw(int,int) {};
int  far cKey(int,int) {  return DLG_NOACTION; };

char* dirlist[256];
int   dirlen;

void ReadDir(void)
{
  int done,i;
  struct ffblk findrec;

  dirlen=0;
  done=findfirst("*.*",&findrec,0);
  while (!done) {
    dirlen++;
    done = findnext(&findrec);
  };
  if (dirlen>255) dirlen=255;
  i=0;
  done=findfirst("*.*",&findrec,0);
  while (!done && i<dirlen) {
    dirlist[i] = new char[18];
    StrCpy(dirlist[i],findrec.ff_name);
    i++;
    done = findnext(&findrec);
  };
};

void main(void)
{
  char text[256] = "SoMeThInG";
  char fullname[128];
  char line0[128],line1[128],line2[128];
  long longnum=654321L;
  int i;
  int cItems=2;
  DATE date;

  printf("free memory: %ld\n",farcoreleft());

  TWindow* win = new TWindow( 32,10,14,3,0x1F,"TWindow",True );
  win->Show();
  ReadKey();
  win->Hide();

  TDialog *dlg;
  dlg = new TDialog( 32,10,15,3,"TDialog" );
  dlg->Insert( new TStatic(2,1,"~Static text") );
  ExecuteDialog( dlg );
  delete dlg;

  StrCpy(line0,"lamerz of the world, unite!");
  StrCpy(line1,"AutoTab works on these lines");
  StrCpy(line2,"Press Enter to edit, up/down to move");

//  TDialog *dlg;

  InitTui();
  TuiLng=RUSSIAN;

  Once:

  ShowDesktop();

  switch( FileOpen("*.CPP",fullname) )
  {
    case DLG_OK:
      StrAdd(fullname," is your choice",1);
      MessageBox("Ok",fullname);
      break;
    case DLG_CANCEL:
      MessageBox("Why?","You have canceled the dialog");
      break;
  };

  i = MenuList( 10,3, "~Menu list\0~Demonstration\0~Choose any\0"
                      "item ~from\0the ~list\0", "Menu" );
  if (i>=0)
    MessageBox("Your choice is:", IntToDec(i,fullname) );
  else
    MessageBox(":", "You pressed Esc" );

  int i0=0,i1=1,i2=0,i3=0;
  int r0=0,r1=1,r2=0;

  ReadDir();
  dlg   = new TDialog( 10,2,57,17, "Test dialog" );
  dlg->Insert( new TStatic(3,1,"View line") );
  dlg->Insert( new TStatic(3,2,"Edit/Input") );
  dlg->Insert( new TStatic(3,3,"Edit/Enter") );
  dlg->Insert( new TStatic(3,4,"Edit long") );
  dlg->Insert( new TEdit( 14, 1, 23, line0, AutoTab, edView ) );
  dlg->Insert( new TEdit( 14, 2, 23, line1, AutoTab, edEdit|edInput ) );
  dlg->Insert( new TEdit( 14, 3, 23, line2, AutoTab, edEdit ) );
  dlg->Insert( new TEditNum( 14, 4, 10, &longnum,  AutoTab ) );
  dlg->Insert( new TEditDate( 3, 6, &date, True ) );
  dlg->Insert( new TStatic( 22, 6, "Tag sign" ) );
  dlg->Insert( new TEdit( 31, 6, 3, "û", AutoTab ) );
  dlg->Insert( new TDivider( 0, 7, 40 ) );
  TGroup* grp0    = new TGroup( 3,8 );
    grp0->Insert( new TCheckBox( 0, 0, "~foreva", &i0, 'f' ) );
    grp0->Insert( new TCheckBox( 0, 1, "M$ must ~die", &i1, 'd' ) );
    grp0->Insert( new TCheckBox( 0, 2, "fore~va 1", &i2, 'v' ) );
    grp0->Insert( new TCheckBox( 0, 3, "M~$ must 2", &i3, '$' ) );
  dlg->Insert( grp0 );
  dlg->Insert( new TList( 22, 8, 15, 6, dirlist, &dirlen ) );
  dlg->Insert( new TCustomList( 40, 8, 15, 6, &cItems, " Title", cDraw, cKey ) );
  dlg->Insert( new TRectangle( 39,7,14,6 ) );
  dlg->Insert( new TButton( 3, 15, "Ok", DLG_OK, True ) );
  dlg->Insert( new TButton( 14, 15, "Cancel", DLG_CANCEL ) );
  dlg->Insert( new TButton( 25, 15, "Wait test", 1, False, 0, WaitTest ) );

  switch( ExecuteDialog( dlg ) )
  {
    case BTN_OK:
      MessageBox( "Message :)", "OK pressed" );
      break;
    case DLG_CANCEL:
      MessageBox( "Message :(", "Why did you press ESC? You are LAMER!" );
      break;
  };

  switch( QuestionBox("Question", "Exit to operating system?", QB_YESNO) )
  {
    case BTN_YES: MessageBox("Message box","Goodbye\nCome again!"); break;
    case BTN_NO: MessageBox("Ok","As you wish..."); goto Once;
    default: MessageBox("LAMER!","I gave you a chance but once again\n"
                                 "you've proven yourself a lamer!"); break;
  };
  if ( EditBox( "Edit box", "Say something:", text ) )
    MessageBox( "You said:", text );
  else
    MessageBox( "Note", "You have canceled the dialog" );


  HideDesktop();

};
