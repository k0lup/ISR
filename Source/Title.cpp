//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Title.h"
#include "Main.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTitleBox *TitleBox;
//---------------------------------------------------------------------------
__fastcall TTitleBox::TTitleBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
// форма активируется
//---------------------------------------------------------------------------
void __fastcall TTitleBox::FormActivate(TObject *Sender)
{
    bIsOk = false;
    // максимальный номер протокола
	AnsiString MaxProtokolNumber = FindMaxProtokolNumber( ProtokolDir, Stencil );
	if( MaxProtokolNumber == "Occupied" )
	 {
		WarningMessageBox( "Не могу создать имя файла протокола с использованием текущего трафарета, так как все возможные имена файлов для текущего трафарета исчерпаны.");
		PostMessage( this->Handle, WM_CLOSE, 0, 0 );
        return;
	 }
    // предлагаемая информация
    TitleEdit->Text = Partitions[nSelectedPartitionNumber].Abbr;
    ProtokolNamePart1Edit->Text = Partitions[nSelectedPartitionNumber].Abbr;
    ProtokolNamePart2Edit->Text = MaxProtokolNumber;
    CancelBtn->SetFocus();
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------
void __fastcall TTitleBox::OKBtnClick(TObject *Sender)
{
    // если заголовок пуст
	if( TitleEdit->Text.IsEmpty() )
     {
		WarningMessageBox( "Введите заголовок раздела" );
		return;
     }
    // заголовок раздела
    TBPartitionTitle = TitleEdit->Text;
    // имя протокола
    TBProtokolNamePart1 = ProtokolNamePart1Edit->Text;
    TBProtokolNamePart2 = ProtokolNamePart2Edit->Text;
    // имя РЭП
    TBRAPName = RAPEdit->Text;
    // имя оператора ПУ АИС
    TBOperatorPUAISName = OperatorPUAISEdit->Text;
    // имя оператора АИС
    TBOperatorAISName = OperatorAISEdit->Text;
    // имя оператора ТКЦК
    TBOperatorTKCKName = OperatorTKCKEdit->Text;
	// имя оператора НТК СИ
    TBOperatorNTKSIName = OperatorNTKSIEdit->Text;
    // имя оператора БС
    TBOperatorBSName = OperatorBSEdit->Text;
	bIsOk = true;
	SendStringToPrisProtokol( "Операция 'Заголовок раздела'", stStandart, true, true, false);
    /*SendStringToPrisProtokol(Stringconst_sTitleProt2 + TitleEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt3 + GetProtokolName(), stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt4 + RAPEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt5 + OperatorPUAISEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt6 + OperatorAISEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt7 + OperatorTKCKEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt8 + OperatorNTKSIEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt9 + OperatorBSEdit->Text, stBlue, false, false, true);
    SendStringToPrisProtokol(Stringconst_sTitleProt10, stStandart, true, true, false);
    */
    // закроем форму
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TTitleBox::CancelBtnClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TTitleBox::TitleEditChange(TObject *Sender)
{
    // перенесем заголовок в первую часть протокола
    ProtokolNamePart1Edit->Text = TitleEdit->Text;
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------
void __fastcall TTitleBox::SetOKBtnEnabled()
{
	OKBtn->Enabled = ( !(TitleEdit->Text.IsEmpty())) && (!(ProtokolNamePart1Edit->Text.IsEmpty()) );
}

void __fastcall TTitleBox::ProtokolNamePart1EditChange(TObject *Sender)
{
    SetOKBtnEnabled();
}
//---------------------------------------------------------------------------

