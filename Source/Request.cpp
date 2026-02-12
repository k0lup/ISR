//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Request.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRequestBox *RequestBox;

//---------------------------------------------------------------------------
__fastcall TRequestBox::TRequestBox(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TRequestBox::FormActivate(TObject *Sender)
{
    // заголовок окна
	switch (CurrentType)
	{
        // доклад
        case (otReport):
            Caption = ReportName;
            break;
        // команда
        case (otCommand):
            Caption = CommandName;
            break;
        // сообщение
        case (otMessage):
            Caption = MessageName;
            break;
        // диполь-операци€
        case (otDipolOperation):
            Caption = DipolOperationName;
            break;
        // »—–-операци€
        case (otISROperation):
            Caption = ISROperationName;
            break;
    }
    bMakeOperation = false;
    CancelBtn->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TRequestBox::MakeButtonClick(TObject *Sender)
{
    bMakeOperation = true;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TRequestBox::CancelBtnClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TRequestBox::SetText(TMDIChild* ch, int nTextStart, int nTextEnd, int nFieldNumber)
{
    OperationTextMemo->Lines->Clear();
	AnsiString CurrentPart;
    // цикл по строкам текущей операции
    for (int i = nTextStart; i <= nTextEnd; i++)
    {
        CurrentPart = (ch->GetSectionStrings())[i].Field[nFieldNumber];
		// если не коментарий
        if (!IsComment(CurrentPart))
          { // текст операции
			if( !CurrentPart.IsEmpty() )
			  OperationTextMemo->Lines->Add( " " + CurrentPart);
		  }
    }
}

