//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CloseRep1.h"
#include "SelectionRep1.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCloseRep *CloseRep;
//---------------------------------------------------------------------------
__fastcall TCloseRep::TCloseRep(TComponent* Owner)
        : TForm(Owner)
{
   ListBox1->Items->Clear(); 
}
//---------------------------------------------------------------------------
// Подпрограмма закрытия документа Word при закрытии раздела
//---------------------------------------------------------------------------
void TCloseRep::CloseWrd(char n)
{  // n - номер закрываемого раздела (1- первый, 2- второй, 3 - третий, 4 - все разделы)
    AnsiString s;
    int j;
    WordDocument* WD;
	bool mm = ISRMainForm->WordApplication->get_Visible();
	if( mm == 0 )
			 return;  // Word не активирован
  // проверим, не осталось ли открытых разделов РЭП
    for( j = 1; j <= ISRMainForm->WordApplication->Documents->Count ;j++ )
    {  OleVariant ItemIndex = j;
       s = ISRMainForm->WordApplication->Documents->Item(ItemIndex)->Name;
       s.SetLength(s.Length()-4);
       switch(n)
         { case 1: if( s == ISRMainForm->Partision_Rep ) break;
                   continue;
           case 2: if( s == ISRMainForm->Second_Rep ) break;
                   continue;
           case 3: if( s == ISRMainForm->Third_Rep ) break;
                   continue;
         }
        ISRMainForm->WordApplication->Documents->Item(ItemIndex)->Close();
        if( n == 4 ) continue;
    }
     if( ISRMainForm->WordApplication->Documents->Count == 0 )
         ISRMainForm->WordApplication->Disconnect();
}
//---------------------------------------------------------------------------
// Подпрограмма закрытия конкретного раздела из трёх загруженных
//---------------------------------------------------------------------------
void __fastcall TCloseRep::Button1Click(TObject *Sender)
{
  TSectionRep* sec_r;
  int i;
////////////
  if(ListBox1->ItemIndex == -1)  // ничего не выбрано
    {  ShowMessage("Выберите раздел РЭП");
       return;
    }
  if(ListBox1->Items->Strings[ListBox1->ItemIndex] == ISRMainForm->Partision_Rep)
       { // Закрываем первый раздел
          CloseWrd(1);
          SendStringToPrisProtokol("РЭП : Выгружен раздел " + ISRMainForm->Partision_Rep,
							  stStandart, false, true, false);
          ISRMainForm->Partision_Rep.SetLength(0);
          ISRMainForm->N7->Caption = "Раздел РЭП";
        }
  else
    if( ListBox1->Items->Strings[ListBox1->ItemIndex] == ISRMainForm->Second_Rep)
        { // Закрываем второй раздел
          CloseWrd(2);
          delete(ISRMainForm->SecondRep);
          SendStringToPrisProtokol("РЭП : Выгружен раздел " + ISRMainForm->Second_Rep,
							  stStandart, false, true, false);
          ISRMainForm->Second_Rep.SetLength(0);
          ISRMainForm->N9->Caption = "Второй раздел";
        }
    else
        { // Закрываем третий раздел
          CloseWrd(3);
          delete(ISRMainForm->ThirdRep);
          SendStringToPrisProtokol("РЭП : Выгружен раздел " + ISRMainForm->Third_Rep,
							  stStandart, false, true, false);
          ISRMainForm->Third_Rep.SetLength(0);
          ISRMainForm->N10->Caption = "Третий раздел";
        }
 // удалить из меню "Секция РЭП / Конец раздела"
  ListBox1->Items->Delete(ListBox1->ItemIndex);
  Close();              // закроем форму CloseRep
}

//---------------------------------------------------------------------------
void __fastcall TCloseRep::Button2Click(TObject *Sender)
{
   Close();     
}
//---------------------------------------------------------------------------

