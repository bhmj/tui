#include <alloc.h>      // alloc,farcoreleft
#include <stdio.h>      // alloc
#include "tui.h"        // strings

char stmp[255];

#define MAXLIST 255
long nItems;
char* List[MAXLIST];
int  tagitem[MAXLIST];
int  shiftval=0;

void ResetList(void)
{
  int i;
  for (i=0; i<MAXLIST; i++) if (List[i]) { delete List[i]; List[i]=0; };
  for (i=0; i<MAXLIST; i++) tagitem[i]=0;
  shiftval=0;
  nItems=0;
};
void LoadList(char* fname)
{
  char buf[255];
  int i;
  FILE* f;

  ResetList();

  for (i=0; i<MAXLIST; i++) List[i]=0;
  f = fopen(fname,"r");
  if (!f) return;
  i=0;
  while (!feof(f))
  {
    if (i>=MAXLIST) break;
    if (feof(f)) break;
    fgets(buf,255,f);
    if (buf[StrLen(buf)-1]=='\n') buf[StrLen(buf)-1]=0;
    List[i] = new char[StrLen(buf)];
    StrCpy(List[i++],buf);
    nItems++;
  };
  fclose(f);
};

void far MenuDemo(void) {
  int i;
  i = MenuList(15,19,"~First item\0~Second item\0~Third item\0","List menu");
  if (i==-1) MessageBox("","Esc was pressed");
  else MessageBox("Your choice",IntToDec(i,stmp));
};
void far OpenDemo(void) {
  char fullname[128];
  int xcode = FileOpen("*.TXT",fullname);
  if (xcode==DLG_OK)
  {
    ResetList();
    LoadList(fullname);
    MessageBox("MessageBox()","File loaded");
    PostMessage(evBroadcast,cmUpdate);
  };
};
void far ProgressDemo(void) {
  int i;
  i = QuestionBox( "QuestionBox()", "Want to pass the waiting test?", QB_YESNO );
  if (i==BTN_YES)
  {
    for (i=0; i<100; i++)
    {
      ProgressBar( 100, i+1, "ProgressBar()", "Waiting..." );
      Delay(1);
    }
    MessageBox("MessageBox()","Test passed");
    HideProgressBar();
  }
};
void far InputDemo(void) {
  char text[128];
  *text=0;
  if ( EditBox("EditBox()","Type something:",text) > 0 )
    MessageBox("You typed",text);
};
void far MiscDemo(void) {
  TuiGreet();
};

void far cDraw(long item, int pos) {
  char a=0x30;
  if (pos&ITEM_FOCUS_ACTIVE)   a=0x0F;
  if (pos&ITEM_FOCUS_INACTIVE) a=0x31;
  SpawnXYC(0,0,a,' ',80);
  if (item>=nItems) return;
  if (StrLen(List[item])>shiftval) WriteXYC(4,0,a,List[item]+shiftval);
  if (tagitem[item]) WriteXYC(1,0,a,"û");
};
int far cKey(long item, int code) {
  switch (code)
  {
    case kbSpace: tagitem[item]^=1; PostMessage(evKeyb,kbDown); break;
    case kbLeft:  if (shiftval) shiftval--; break;
    case kbRight: shiftval++; break;
    default:
      return DLG_NOACTION;
  };
  PostMessage(evBroadcast,cmUpdate);
  return DLG_KEYUSED;
};

void main(void)
{
  TDialog* dlg;
  TGroup *grp0,*grp1;

  char line0[128],line1[128],line2[128];
  long num=123456L;
  DATE date;
  int check0=1,check1=1,check2=1,check3=0;
  int radio=2;
  int xcode;

  StrCpy(line0,"type something");
  StrCpy(line1,"Enter to edit, up/down to move");
  StrCpy(line2,"this line cannot be edited; it is a view line");

  LoadList("TUINOTE.TXT");

  TuiLng=ENGLISH;
  InitTui();

  SetDesktop('°',Cyan);
  ShowDesktop();

  MessageBox("Welcome",
    "This program demonstrates basic capabilities of TUI library.\n"
    "This is a work in progress so it may contain errors.\n"
    "BTW this window shows multiline message box function."
  );

  dlg = new TDialog( 2,1,74,20,"TUI Demonstration" );
  dlg->Insert( new TStatic(2,0,"TCustomList") );
  dlg->Insert( new TCustomList(2,1,70,6,&nItems,"    File contents",cDraw,cKey) );
  dlg->Insert( new TStatic(2,9,"View/Edit components") );
  dlg->Insert( new TStatic(2,10,"TEdit /1/") );
  dlg->Insert( new TStatic(2,11,"TEdit /2/") );
  dlg->Insert( new TStatic(2,12,"TEdit /3/") );
  dlg->Insert( new TStatic(2,13,"TEditNum") );
  dlg->Insert( new TStatic(2,14,"TEditDate") );
  dlg->Insert( new TStatic(25,14,"(space for calendar)") );
  dlg->Insert( new TEdit(12,10,34,line0,AutoTab) );
  dlg->Insert( new TEdit(12,11,34,line1,AutoTab,edEdit) );
  dlg->Insert( new TEdit(12,12,34,line2,AutoTab,edView) );
  dlg->Insert( new TEditNum(12,13,8,&num,AutoTab) );
  dlg->Insert( new TEditDate(12,14,&date,AutoTab) );
  dlg->Insert( new TStatic(49,9,"TCheckBox") );
  dlg->Insert( new TStatic(49,14,"TRadioButton") );
    grp0 = new TGroup( 49,10 );
    grp0->Insert( new TCheckBox(0,0,"~640 Kb is enough",   &check0, '6') );
    grp0->Insert( new TCheckBox(0,1,"~Textmode rules!",    &check1, 't') );
    grp0->Insert( new TCheckBox(0,2,"Who needs ~GPU?",     &check2, 'g') );
    grp0->Insert( new TCheckBox(0,3,"Make ~ASCII not war", &check3, 'a') );
    grp1 = new TGroup( 49,15 );
    grp1->Insert( new TRadioButton(0,0,"Git~Hub",      &radio, 1, 'h') );
    grp1->Insert( new TRadioButton(0,1,"Bit~Bucket",   &radio, 2, 'b') );
    grp1->Insert( new TRadioButton(0,2,"Git~Lab",      &radio, 4, 'l') );
    grp1->Insert( new TRadioButton(0,3,"Source~Forge", &radio, 8, 'f') );
  dlg->Insert( grp0 );
  dlg->Insert( grp1 );
  dlg->Insert( new TStatic(2,16,"TButton") );
  dlg->Insert( new TButton(12,16,"MenuList",0,False,0,MenuDemo) );
  dlg->Insert( new TButton(24,16,"Progress",0,False,0,ProgressDemo) );
  dlg->Insert( new TButton(36,16,"OpenFile",0,False,0,OpenDemo) );
  dlg->Insert( new TButton(12,18,"InputBox",0,False,0,InputDemo) );
  dlg->Insert( new TButton(24,18,"Graphics",0,False,0,MiscDemo) );
  dlg->Insert( new TButton(36,18,"  E~xit  ",DLG_OK,True,'x') );

  printf("\nfree mem: %ld",farcoreleft());
  xcode = ExecuteDialog( dlg );

  delete dlg;

  if (xcode==DLG_OK) MessageBox("Thank you","Come again");
  else {
    DlgColor(Red,Yellow,Yellow,White);
    MessageBox("","Esc closes dialog without applying changes");
  };

  long num0 = 123;
  long num1 = 345;
  long num2 = 567;
  dlg = new TDialog(25,2,29,19,"TBigNum demo");
  dlg->Insert(new TBigNum(2,1,4,&num0));
  dlg->Insert(new TBigNum(2,6,4,&num1));
  dlg->Insert(new TBigNum(2,11,4,&num2));
  dlg->Insert(new TOkButton(10,17,"  Ok  "));
  ExecuteDialog(dlg);
  delete dlg;
  MessageBox("First line contained",IntToDec(num0,line0));

  PassBox("PassBox","Enter password",line0);
  MessageBox("Password",line0);

  HideDesktop();

  printf("\nfree mem: %ld",farcoreleft());
};
