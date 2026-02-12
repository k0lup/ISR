//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "List.h"
#include "Find.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPartitionListBox *PartitionListBox;
//---------------------------------------------------------------------------
__fastcall TPartitionListBox::TPartitionListBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPartitionListBox::FormActivate(TObject *Sender)
{
	int i, j, pp;
	bool pr = 0;
	AnsiString as = "";
	// очистим старый список разделов
	PartitionListBox->Items->Clear();
	// цикл по всем разделам
	for ( i = 0; i < nTotalPartitionCount; i++, pr = 0 )
	  {
		if( HideAbbrList->Count )   //  если были разделы, загружаемые только по НАЧРАЗД
		for( j = 0; j < HideAbbrList->Count; j++)
		 {
		   as = AnsiString( (char*)HideAbbrList->Items[j] ) ;
		   pp = strcmp( as.c_str(), Partitions[i].Abbr.c_str() );
		   if( pp == 0 )
			 {
			   pr = 1;
			   break;
			 }
		 }
		if( pr == 0 )               // добавим раздел в список, если он не по НАЧРАЗД
			 PartitionListBox->Items->Add( Partitions[i].Abbr );
      }
    PartitionListBox->SetFocus();
    PartitionListBox->ItemIndex = 0;
    ChooseBtn->Enabled = nPartitionCount == 0;
    LoadBtn->Enabled = nPartitionCount != 0;
    Result = prCancel;
}
//---------------------------------------------------------------------------
void __fastcall TPartitionListBox::ChooseBtnClick( TObject *Sender )
{
  int i;
  AnsiString Abbr;
  //
  if( PartitionListBox->ItemIndex != -1 )                                   // если есть выбранный раздел
   {
	 Abbr = PartitionListBox->Items->Strings[PartitionListBox->ItemIndex];  // номер выбранного раздела
	 for( i = 0; i < nTotalPartitionCount; i++ )                            // цикл по всем разделам
	  {
		if( Abbr == Partitions[i].Abbr )
		 {
		   nSelectedPartition = i;
		   break;
		 }
	  }
	 if( (TButton*)Sender == ChooseBtn )                                   // нажата кнопка "Выбрать"
		   Result = prChoose;
	 else
		   Result = prLoad;
	 Close();                                                              // закроем форму
   }
}
//---------------------------------------------------------------------------

void __fastcall TPartitionListBox::PartitionListBoxDblClick( TObject *Sender )
{
  if( ChooseBtn->Enabled )
		ChooseBtnClick( (TObject*)ChooseBtn );
  else
        ChooseBtnClick((TObject*)LoadBtn);
}
//---------------------------------------------------------------------------

void __fastcall TPartitionListBox::CancelBtnClick( TObject *Sender )
{
    nSelectedPartition = -1;
    Result = prCancel;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TPartitionListBox::FindBtnClick( TObject *Sender )
{
    // окно запроса аббревиатуры
    FindBox->ShowModal();
    // аббревиатура
    AnsiString Abbr = FindBox->GetAbbr();
    // если аббревиатура для поиска задана
	if (Abbr != "")
	{
        int nAbbrLength = Abbr.Length();
        int i;
        bool bFind = false;
		for (i = 0; i < PartitionListBox->Items->Count; i++)
		{
			if (PartitionListBox->Items->Strings[i].SubString(1, nAbbrLength).Compare(Abbr) == 0)
			{
                bFind = true;
                break;
            }
        }
        // если нашли
		if (bFind)
		{
            PartitionListBox->ItemIndex = i;
        }
		else
		{
            WarningMessageBox( "Раздел не найден" );
        }
    }
}
//---------------------------------------------------------------------------


