//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Find.h"
#include "Common.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFindBox *FindBox;
//---------------------------------------------------------------------------
__fastcall TFindBox::TFindBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFindBox::CancelBtnClick(TObject *Sender)
{
    Abbr = "";
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TFindBox::OKBtnClick(TObject *Sender)
{
    Abbr = AbbrEdit->Text;
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TFindBox::FormActivate(TObject *Sender)
{
    AbbrEdit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFindBox::SetOKBtnEnabled()
{
    OKBtn->Enabled = !(AbbrEdit->Text.IsEmpty());
}

void __fastcall TFindBox::AbbrEditEnter(TObject *Sender)
{
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------
void __fastcall TFindBox::AbbrEditChange(TObject *Sender)
{
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------

