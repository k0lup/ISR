//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Common.h"
#include "Request.h"
#include "Variant.h"
#include "SelectionRep1.h"
#include <stdio.h>
#include <io.h>
#include "Main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSectionRep *SectionRep;
//---------------------------------------------------------------------------
__fastcall TSectionRep::TSectionRep(TComponent* Owner)
        : TForm(Owner)
{
  OpCount = new TList();
  CurrOp = 1;
  Op = 1;
  p = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSectionRep::FormActivate(TObject *Sender)
{
  FILE* f;
  K_OP* k;
  char* s, *p, *p1;
   int i, j, l = 0;
 ////////////
  if(ISRMainForm->Partision_Rep1.IsEmpty())
                                       return;
  if(OpCount->Count)
                  return;  // уже раздел сформирован
   if (dScr > 1)
     {
        StringGrid1->Canvas->Font->Size = nCellFontSizeSF;
     }
    else // мелкий шрифт
     {
        StringGrid1->Canvas->Font->Size = nCellFontSizeBF;
     }
   //AnsiString ss = ExtractFileName(ISRMainForm->Partision_Rep);
   //ss = StringReplace(ss,".dir","    ",TReplaceFlags());

   StringGrid1->Cells[0][0] = "№ ОП";
   StringGrid1->Cells[1][0] = "   Т";
   StringGrid1->Cells[2][0] = " Команда, выполняемая операция, примеч...";
   StringGrid1->Cells[3][0] = " Исполнитель";
   StringGrid1->Cells[4][0] = " Документация";
// формируем имя файла с полным путём
  AnsiString ss =  ISRMainForm->Partision_Rep1;
  //Caption =  "Раздел РЭП : " + ss;
  ss = ParameterSPOLocation +
                          "\\" + ss + "\\" + ss + ".dir";
  //ShowED(CurrentType, "", false);
 ///
  if((f = fopen(ss.c_str(),"rt"))==NULL)
   {
       ShowMessage("Не открылся файл РЭП");
       return;
   }
  else
   {  s = (char*)calloc(81, sizeof(char));
      for( i=0 ;; i++ )
          {
            if((i-5)==StringGrid1->RowCount)  // дошли до последней строки
                    StringGrid1->RowCount ++;
            if(fgets(s,80,f))
              {
                if(i<6) // заголовок файла структуры РЭП (файл с расширением dir)
                   continue;
                if(s[0] != '|') // считаем кол-во операций в разделе РЭП
                  {
                     k = new K_OP;
                     k->kol_op = 1;
                     k->v = 0;
                     OpCount->Add(k);
                  }
                else
                  if(OpCount->Count)  // если уже есть хоть одна операция
                     {  k = (K_OP*)OpCount->Items[OpCount->Count-1];
                        k->kol_op++;
                     }
                  else { i--; continue; }   
                p1 = s;
                j = 0;
                while ( p = StrScan(p1,'|') )
                 { if((l = p-p1)) // есть инф-я в этом поле, не пустая ячейка
                   {
                     if(j>4)
                      ErrorMessageBox("Нарушена структура файла РЭП в строке" + IntToStr(i-5));
                     else
                       StringGrid1->Cells[j][i-5] = AnsiUpperCase(AnsiString(p1,l));
                   }
                     p1 = p+1;
                     j++;
                 }
               }   // if fgets;
            else break;
            AnsiString hh = StringGrid1->Rows[i-5]->Text;
            hh = Trim(hh);  // проверка на пустую строку
            if(hh.IsEmpty())
                   { i--; // тогда не увеличиваем индекс строки
                     // и количество строк в операции
                     ((K_OP*)(OpCount->Items[OpCount->Count-1]))->kol_op--;
                   }
          } // for
     fclose(f);
     free(s);
   } // else fopen(...)
 StringGrid1->LeftCol = 0;
 StringGrid1->Row = 1;
}
//---------------------------------------------------------------------------
// Функция отрисовки ячейки StringGrid
//---------------------------------------------------------------------------
void __fastcall TSectionRep::StringGrid1DrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  TColor FontColor = (TColor)nNonSelFontColor, BackColor = (TColor)nGridBackColor;
  int n = 1, i;
  //bool bold = 0;
  AnsiString Text =  StringGrid1->Cells[ACol][ARow];
 if(ARow != 0) // строку заголовка не меняем
{
  for(i = 0; i < OpCount->Count; i++)
    { // берём данные об операции из массива операций
      K_OP* k = (K_OP*)OpCount->Items[i];
      // проверяем, относится ли текущая строка к этой операции
      if((n <= ARow) && (ARow < n + k->kol_op))
        if(k->v)
             { // если выполненная операция
               StringGrid1->Canvas->Brush->Color = (TColor)nMadeOperationColor;
               StringGrid1->Canvas->FillRect(Rect);
               // цвет шрифта
               StringGrid1->Canvas->Font->Color = clNavy;
               StringGrid1->Canvas->TextOut(Rect.Left+1 , Rect.Top+1, Text);
               // Жирным шрифтом
               //bold = 1;
              return;
             }
        else break;
      // переходим к следующей операции из массива
           n += k->kol_op;
    } //for
// текущая ?
if(CurrOp > OpCount->Count)
               return;  // уже вышли за последнюю операцию
else
    if((Op <= ARow) && (ARow < Op + ((K_OP*)(OpCount->Items[CurrOp-1]))->kol_op))
           { // да
             if( !ISRMainForm->KO_Rep.IsEmpty() &&
                          (ISRMainForm->KO_Rep == ISRMainForm->Partision_Rep1))
                 {  // операция уже выполняется, но ещё не пришёл ответ от ПРИС
                    BackColor = clNavy;
                    FontColor = clWhite;
                 }
             else
                {  // заливка ячейки для текущей операции
                  BackColor = (TColor)nCurOperationBackColor;
                  FontColor = clBlack;
                }
              // обычный шрифт
              //bold = 0;
            }
// выведем текст

  StringGrid1->Canvas->Brush->Color = BackColor;
  StringGrid1->Canvas->FillRect(Rect);
  // цвет шрифта
  StringGrid1->Canvas->Font->Color = FontColor;
 // StringGrid1->Canvas->Font->Name = "CurierNew";
 /*if(bold)
    StringGrid1->Font->Style = TFontStyles() << fsBold;
  else
    StringGrid1->Font->Style = TFontStyles();*/
  StringGrid1->Canvas->TextOut(Rect.Left+1 , Rect.Top+1, Text);
  } // if(ARow)
}
//---------------------------------------------------------------------------

void __fastcall TSectionRep::FormDestroy(TObject *Sender)
{
   int i;
   K_OP* k;
  if(OpCount->Count)
       {  // работали с разделами РЭП
         for( i = 0; i < OpCount->Count ; i++ )
          { k = (K_OP*)OpCount->Items[i];
            delete k;
          }
         delete OpCount;
       }     
}
//---------------------------------------------------------------------------
// Функция выполнения операций раздела РЭП
//---------------------------------------------------------------------------
void __fastcall TSectionRep::StringGrid1DblClick(TObject *Sender)
{
  int k,j, i = StringGrid1->Row;
  AnsiString s, s_kom;
  // Проверка на пустую строку
  if( Trim(StringGrid1->Rows[i]->Text).IsEmpty())
              return;
  if(totalOperationsInPris || !ISRMainForm->KO_Rep.IsEmpty() )
       {  WarningMessageBox("ПРИС занят выполнением операции","Раздел РЭП");
          return;
       }
 // встаём на первую строку текущей операции
  while (StringGrid1->Cells[0][i].IsEmpty())
      {  i--;
         if(i == 0)  // дошли до верха и не нашли ни одного номера операции
           { ErrorMessageBox("Нет номера операции", "РЭП");
             return;
           }
      }
  // номер выбранной мышью операции
  try { k = StrToInt(StringGrid1->Cells[0][i]); }
   catch(...)
       { ErrorMessageBox("Неверный номер операции", "РЭП");
         goto KON;
       }
  // Если уже выполняли эту операцию
  if( ((K_OP*)(OpCount->Items[k - 1]))->v )
   {
     if (MessageBox(Handle, "Эта операция уже выполнена. Выполнить её ещё раз?",
                                 "ВНИМАНИЕ !", MB_YESNO) == IDNO)
                   return;
     else
      if( k != CurrOp )
           p = 1;  // установим признак повтора выполненной операции
   }
  else // Если перескочили вперёд через операции или назад к невыполненной операции
    if( (k != CurrOp) && ( k <= OpCount->Count) )
      if( MessageBox(Handle, "Нарушен порядок выполнения операций. Выполнить выбранную операцию?",
                                 "ВНИМАНИЕ !", MB_YESNO) == IDNO)
                    return;
      else p = 1;
  if(p)
   { k1 = CurrOp; // сохраним текущую операцию
     CurrOp = k;  // назначим текущей выбранную мышью операцию
     for(j = 0, Op = 1; j < k - 1; j++ ) // вычислим номер текущей строки в разделе РЭП
       Op += ((K_OP*)(OpCount->Items[j]))->kol_op;
     // перерисовка
       StringGrid1->LeftCol = 0;
       StringGrid1->Col = 2;
       StringGrid1->Refresh();
   }
 /////////////////////////////////////////////////////
  if(k == CurrOp)
      { // находимся на текущей операции
        RequestBox->Caption = "РЭП";
        RequestBox->OperationTextMemo->Lines->Clear();
        s = StringGrid1->Cells[1][i];
        switch(s.c_str()[0])
         { case 'О': RequestBox->SetCurrentType(otDipolOperation);
                     RequestBox->OperationTextMemo->Lines->Add(StringGrid1->Cells[2][i]);
                     break;
           case 'В': // вариант
                    { RequestBox->SetCurrentType(otVariant);
                      // Посчитаем кол-во вариантов
                      int kv = 0;
                      for( j = Op + 1; j < Op + ((K_OP*)(OpCount->Items[CurrOp-1]))->kol_op; j++ )
                      {
                        if( !StringGrid1->Cells[1][j].IsEmpty() )
                                kv++;
                        VariantBox->SetVariantCount(kv);
                      }
                      VariantBox->ShowModal();
                      kv = VariantBox->GetChosenVariant();
                      // Выведем строку выбранного варианта
                      for( j = Op + 1; j < Op + ((K_OP*)(OpCount->Items[CurrOp-1]))->kol_op; j++ )
                       {
                         if( !StringGrid1->Cells[1][j].IsEmpty() &&
                                           StrToInt(StringGrid1->Cells[1][j]) == kv )
                            { RequestBox->OperationTextMemo->Lines->Add(StringGrid1->Cells[2][j]);
                              KO = s_kom = StringGrid1->Cells[2][j];
                            }
                       }
                      break;
                     }
           case 'У':
           case 'П': // указание оператору
                     s_kom = "УКАЗАНИЕ ОПЕРАТОРУ";
                     RequestBox->SetCurrentType(otMessage);
                     goto V;
           case 'Д': // доклад
                     s_kom = "ДОКЛАД";
                     RequestBox->SetCurrentType(otReport);
                     goto V;
           case 'К': // команда
                     s_kom = "КОМАНДА";
                     RequestBox->SetCurrentType(otCommand);
   V:                for( j = 0; j < ((K_OP*)(OpCount->Items[CurrOp-1]))->kol_op; j++, i++ )
                      {
                          RequestBox->OperationTextMemo->Lines->Add(StringGrid1->Cells[2][i]);
                      }
                     break;
           default:  ErrorMessageBox("Недопустимый код операции",ISRMainForm->Partision_Rep1);
                     goto O;
         }
        RequestBox->ShowModal();
        if (RequestBox->GetMakeOperation())
            { // выбрали кнопку "Выполнить"
               if( s.c_str()[0] == 'О' || s.c_str()[0] == 'В' )
                 {
                   // установим признак запущенной операции
                   ISRMainForm->KO_Rep = ISRMainForm->Partision_Rep1 ;
                   if(s.c_str()[0] != 'В')
                          KO = StringGrid1->Cells[2][i];
                   SendStringToPrisProtokol("РЭП : Директива " + KO + "  ( Раздел РЭП " +
                                   ISRMainForm->KO_Rep + " операция " + IntToStr(CurrOp) + " )",
                                        stStandart, false, true, false);
                   SendKOToPris(KO,0);
                   goto KON;
                 }
               else
                 { SendStringToPrisProtokol("РЭП : раздел " + ISRMainForm->Partision_Rep1 +
                            ", операция " + IntToStr(CurrOp) + " - "
                              + s_kom, stStandart, false, true, false);
                     for( j=0;j<RequestBox->OperationTextMemo->Lines->Count;j++)
                          SendStringToPrisProtokol(RequestBox->OperationTextMemo->Lines->Strings[j] ,
                                   stRep, false, false, true);
                     // признак выполненной операции
                     ((K_OP*)(OpCount->Items[CurrOp-1]))->v = 1;
                     SendStringToPrisProtokol("РЭП : Конец операции " + s_kom, stStandart, false, true, false);
                 }
             } // выбрали "Выполнить"
 O:     if(p) // был нарушен порядок операции
                      { CurrOp = k1; // восстановим текущую операцию
                         for(j = 0, Op = 1; j < k1 - 1; j++ )
                            Op += ((K_OP*)(OpCount->Items[j]))->kol_op;
                        p = 0;  // сбросим признак
                      }
        else
          if (RequestBox->GetMakeOperation())
                      {
                        if( CurrOp <= OpCount->Count)
                              CurrOp++; // увеличим номер текущей операции
                        if(CurrOp>1)   // определяем номер строки следующей операции в StringGrid
                           Op += ((K_OP*)(OpCount->Items[CurrOp-2]))->kol_op;
                      }
KON:
      StringGrid1->LeftCol = 0;
       //StringGrid1->Row = Op;
       StringGrid1->Col = 2;
       StringGrid1->Refresh();
      }
  else return;
}
//---------------------------------------------------------------------------
// Функция обработки ответа от ПРИС на операцию от РЭП
//---------------------------------------------------------------------------
 void TSectionRep::AnswerForRep(int Answer)
 {
   int i;
   //
   SendStringToPrisProtokol("РЭП : Конец директивы " + KO, stStandart, false, true, false);
   switch (Answer)
    {
      case (1): // операция выполнена ненормально
      case (2): // БИНТ занят
               if (Answer == 1)
                 InformationMessageBox("Операция не выполнена", "РЭП");
               if (Answer == 2)
                 InformationMessageBox("ПРИС занят выполнением операции", "РЭП");
               break;
      case (0): // операция выполнена нормально
               InformationMessageBox("Операция выполнена нормально", "РЭП");
                // признак выполненной операции
               ((K_OP*)(OpCount->Items[CurrOp-1]))->v = 1;
    }
  // погасили признак запущенной операции
   ISRMainForm->KO_Rep.SetLength(0);
   // Очистим операцию
   KO.SetLength(0);
  //
  if(p) // был нарушен порядок операции
    {   CurrOp = k1; // восстановим текущую операцию
        for(i = 0, Op = 1; i < k1 - 1; i++ )
                     Op += ((K_OP*)(OpCount->Items[i]))->kol_op;
        p = 0;  // сбросим признак
    }
  else
    {  // увеличим номер текущей операции
       if( CurrOp <= OpCount->Count)
               CurrOp++;
       // определяем номер строки следующей операции в StringGrid
       if(CurrOp>1)
            Op += ((K_OP*)(OpCount->Items[CurrOp-2]))->kol_op;
    }
   StringGrid1->LeftCol = 0;
   StringGrid1->Row = Op;
   StringGrid1->Refresh();
}
