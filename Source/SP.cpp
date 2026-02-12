//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SP.h"
#include "Common.h"
#include "Completing.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
// признак СП из операции "И" СП в файле dii
extern short ISR_prot;
TSPBox *SPBox;
//---------------------------------------------------------------------------
__fastcall TSPBox::TSPBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSPBox::FormActivate(TObject *Sender)
{
    // если дальше резать некуда
    if (nBisect > MaxBisectCount)
     {
        WarningMessageBox("Протокол дальше резать невозможно, т.к. количество частей не может быть больше " + IntToStr(MaxBisectCount) + ".");
        return;
     }
    ProtokolNamePart1Edit->Text = TBProtokolNamePart1;
    ProtokolNamePart2Edit->Text = TBProtokolNamePart2;
    AnsiString ProtokolNamePart3 = IntToStr(nBisect);
    if (nBisect < 10)
     {
        ProtokolNamePart3 = "0" + ProtokolNamePart3;
     }
    ProtokolNamePart3Edit->Text = ProtokolNamePart3;
    if(Sender)
        {
           CancelBtn->SetFocus();
           SetOKBtnEnabled();
        }
}
//---------------------------------------------------------------------------
#include <Main.h>
void __fastcall TSPBox::CancelBtnClick(TObject *Sender)
{
    ISR_prot = 0; // не будем ждать ответ
    ISRMainForm->nR=0;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TSPBox::OKBtnClick(TObject *Sender)
{
	SendKOToPris( "СП " + ProtokolNamePart1Edit->Text + "." + ProtokolNamePart2Edit->Text + "-" + ProtokolNamePart3Edit->Text, false);
    if( Sender == 0 )  // если в структуре была команда СП *
        totalOperationsInPris--;
    nBisect++;
    //if(Sender)
		  //ISRMainForm->nR=8; // через меню
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TSPBox::SetOKBtnEnabled()
{
    OKBtn->Enabled = !(ProtokolNamePart1Edit->Text.IsEmpty());
}

void __fastcall TSPBox::ProtokolNamePart1EditChange(TObject *Sender)
{
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------


