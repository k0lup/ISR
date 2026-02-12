//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Variant.h"
#include "Main.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TVariantBox *VariantBox;
//---------------------------------------------------------------------
__fastcall TVariantBox::TVariantBox(TComponent *Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------
void __fastcall TVariantBox::FormActivate(TObject *Sender)
{
    // очистим старый список вариантов
    VariantListBox->Items->Clear();
    // создадим новый список вариантов
	for( int i = 1; i <= nVariantCount; i++ )
     {
        VariantListBox->Items->Add(IntToStr(i));
     }
    // установим фокус на ListBox
    VariantListBox->SetFocus();
    // вариант не выбран
    nChosenVariant = -1;
    VariantListBox->SetFocus();
}
//---------------------------------------------------------------------
void __fastcall TVariantBox::ChooseBtnClick(TObject *Sender)
{
    // если выбран вариант
	if( VariantListBox->ItemIndex != - 1 )
    {  // номер варианта
       nChosenVariant = VariantListBox->ItemIndex + 1;
       // закроем
       Close();
    }
}
//---------------------------------------------------------------------------

void __fastcall TVariantBox::VariantListBoxDblClick(TObject *Sender)
{
    ChooseBtnClick(NULL);
}
//---------------------------------------------------------------------------

