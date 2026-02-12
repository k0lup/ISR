//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "End.h"
#include "Title.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEndBox *EndBox;
//---------------------------------------------------------------------------
__fastcall TEndBox::TEndBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEndBox::EndWithSavingProtBtnClick(TObject *Sender)
{
    nResult = erEndWithSaveProtokol;
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TEndBox::EndWithoutSavingProtBtnClick(TObject *Sender)
{
    nResult = erEndWithoutSaveProtokol;
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TEndBox::CancelBtnClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TEndBox::FormActivate(TObject *Sender)
{
	if (bEndAllPartitions)
	{
		Caption = "¬ыход из разделов";
        EndWithSavingProtBtn->Caption = "«акончить раздел с сохранением протокола";
		EndWithoutSavingProtBtn->Caption = "«акончить разделы без сохранени€ протокола";
    }
	else
	{
        Caption = "¬ыход из разделов";
		EndWithSavingProtBtn->Caption = "«акончить раздел с сохранением протокола";
		EndWithoutSavingProtBtn->Caption = "«акончить разделы без сохранени€ протокола";
    }
    ProtokolNamePart1Edit->Text = TBProtokolNamePart1;
    ProtokolNamePart2Edit->Text = TBProtokolNamePart2;
    // если была резка
    if (nBisect > 1)
       {
         AnsiString ProtokolNamePart3 = IntToStr(nBisect);
         if (nBisect < 10)
           {
             ProtokolNamePart3 = "0" + ProtokolNamePart3;
           }
         ProtokolNamePart3Edit->Text = ProtokolNamePart3;
         ProtokolNamePart3Edit->Visible = true;
         Delimiter2Label->Visible = true;
       }
    else {
        ProtokolNamePart3Edit->Visible = false;
        Delimiter2Label->Visible = false;
    }
    nResult = erCancel;
    CancelBtn->SetFocus();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TEndBox::GetProtokolName()
{
    AnsiString ResString;
    ResString = ProtokolNamePart1Edit->Text + "." + ProtokolNamePart2Edit->Text;
    // если была резка
    if (nBisect > 1)
     {
        ResString += "-" + ProtokolNamePart3Edit->Text;
     }
    return (ResString);
}

