//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "SelectionRep1.h"
#include "Main.h"
#include "PartisionRep1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
 TPartisionRep *PartisionRep;
//---------------------------------------------------------------------------
__fastcall TPartisionRep::TPartisionRep( TComponent* Owner )
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TPartisionRep::FormActivate( TObject *Sender )
{
 int i;
 /////////////////////////////////////////////////////////////
//             Формирование списка разделов РЭП
/////////////////////////////////////////////////////////////
if( PartisionRep )
  {
	ISRMainForm->FormPartition_REP();
	if( PartisionRep->ListBox1->Items->Count == 0 )
	{ // разделы РЭП не заданы, погасим пункты меню
	   //N13->Enabled = false ;
	   ISRMainForm->ToolButton7->Enabled = false;
	   ISRMainForm->ToolButton3->Enabled = false;
	   //N6->Enabled = false;
	}
  }
    ListBox1->SetFocus();
    ListBox1->ItemIndex = -1;
	rzd.SetLength(0);
	// начальный выбор строки (ничего не выбрано)
    LoadBtn->Enabled = 1; //nPartitionCount != 0;
}
//---------------------------------------------------------------------------
#include "CloseRep1.h"
void __fastcall TPartisionRep::LoadBtnClick( TObject *Sender )
{
  //int kz  = 0; // результат загрузки разделов РЭП
//////////
  if(ListBox1->ItemIndex == -1)  // ничего не выбрано
    {
      ShowMessage("Выберите раздел РЭП");
      return;
    }
   Close();
//////////
 rzd = ListBox1->Items->Strings[ListBox1->ItemIndex];
 if( rzd == ISRMainForm->Partision_Rep || rzd == ISRMainForm->Second_Rep ||
        rzd == ISRMainForm->Third_Rep )
             { ErrorMessageBox("Раздел уже загружен");
               return;
             }
  if(ISRMainForm->Partision_Rep.IsEmpty())
   { // загружаем первый радел
      LoadPart(etPart_REP1);
      return;
   }
  if(ISRMainForm->Second_Rep.IsEmpty())
   { // загружаем  второй раздел РЭП
      LoadPart(etPart_REP2);
      return;
   }
  if(ISRMainForm->Third_Rep.IsEmpty())
   { // загружаем  третий раздел РЭП
      LoadPart(etPart_REP3);
      return;
   }
 ErrorMessageBox("Уже загружено 3 раздела РЭП");
}
//---------------------------------------------------------------------------
//  Функция загрузки конкретного раздела РЭП с именем "rzd"
//---------------------------------------------------------------------------
void  TPartisionRep::LoadPart(EDType com)
{
  short kz = 0;
  AnsiString s;
  TMenuItem* cap;
  Visible = false; // уберём окно со списком разделов РЭП
  // Загрузка
   try{ kz = ISRMainForm->ShowED(com, "", false);}
   catch(...)
     { ShowMessage("Ошибка призагрузке раздела РЭП " + rzd); goto OSH; }
   if(!kz)
   {  // запомним имя выбранного раздела РЭП
      switch(com)
      {
        case etPart_REP1: ISRMainForm->Partision_Rep =  rzd;
                      cap =  ISRMainForm->N7;
                      break;
        case etPart_REP2: ISRMainForm->Second_Rep =  rzd;
                      cap = ISRMainForm->N9;
                      break;
        case etPart_REP3: ISRMainForm->Third_Rep =  rzd;
                       cap = ISRMainForm->N10;
                      break;
	   }
      CloseRep->ListBox1->Items->Add(rzd);
      cap->Caption = cap->Caption + " ( " + rzd + " )";
	  SendStringToPrisProtokol("РЭП : Загружен раздел " + rzd, stStandart, false, true, false);
    }
    else {
OSH:       switch(com)
            {
              case etPart_REP1: ISRMainForm->Partision_Rep.SetLength(0); break;
              case etPart_REP2: ISRMainForm->Second_Rep.SetLength(0); break;
              case etPart_REP3: ISRMainForm->Third_Rep.SetLength(0); break;
             }
           Close(); // Закроем окно со списком разделов РЭП
         }
}

//---------------------------------------------------------------------------
void __fastcall TPartisionRep::Button2Click(TObject *Sender)
{
  Close();
}

