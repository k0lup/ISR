//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Main.h"
#include "Setting.h"
#include "Common.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingBox *SettingBox;
//---------------------------------------------------------------------------
__fastcall TSettingBox::TSettingBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSettingBox::FormActivate( TObject *Sender )
{
    AllowBackCrossingCheckBox->Checked =  bAllowBackCrossing;
}
//---------------------------------------------------------------------------
void __fastcall TSettingBox::CancelBtnClick( TObject *Sender )
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TSettingBox::OKBtnClick( TObject *Sender )
{
    // если режим был изменен
	if( AllowBackCrossingCheckBox->Checked != bAllowBackCrossing )
	 {
		if( AllowBackCrossingCheckBox->Checked )
		 {
			SendStringToPrisProtokol( "Оператор установил режим работы ИСР, позволяющий делать переходы назад при перемещении по секции", stStandart, true, true, false );
		 }
		else
		 {
			SendStringToPrisProtokol( "Оператор установил режим работы ИСР, запрещающий делать переходы назад при перемещении по секции", stStandart, true, true, false );
		 }
	 }
    bAllowBackCrossing = AllowBackCrossingCheckBox->Checked;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TSettingBox::Button1Click(TObject *Sender)
{
  ISRMainForm->ConfigureMenu();
}
//---------------------------------------------------------------------------

