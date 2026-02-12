//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ChildWin.h"
#include "Variant.h"
#include "Main.h"
#include "Request.h"
#include "SP.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//---------------------------------------------------------------------
__fastcall TMDIChild::TMDIChild(TComponent *Owner, int nTransSectionNumber,
	StructureString* pTransSectionStrings, int nTransSectionStringCount,
    AnsiString TransSectionCaption,AnsiString TransStructureFileName,
	int nTransPartitionNumber
)
	: TForm(Owner), nSectionNumber(nTransSectionNumber), pSectionStrings(pTransSectionStrings),
    nSectionStringCount(nTransSectionStringCount), SectionCaption(TransSectionCaption),
    StructureFileName(TransStructureFileName), bIsConstructed(true), nPartitionNumber(nTransPartitionNumber)
{
	bSelectedVariant = false;
    bPrisExecutesOperation = false;
    bExecuteAutoBlock = false;
    bStopAutoRequest = false;
	bAutoBlockFromPris = false;
    bISRPartitionOperationProcess = false;
    int i, j;
    AnsiString ErrorMessage;
    // положение заголовка
    SectionCaptionLabel->Left = nCaptionLeft;
    SectionCaptionLabel->Top = nCaptionTop;
    // если крупный щрифт
    if (dScr > 1)
    {
        SectionCaptionLabel->Font->Size = nCaptionFontSizeBF;
    }
    else // мелкий шрифт
    {
        SectionCaptionLabel->Font->Size = nCaptionFontSizeSF;
    }
    // заголовок секции
    SectionCaptionLabel->Caption = SectionCaption;
    // положение таблицы
    Grid->Left = nGridLeft;
    Grid->Top = nGridTop;
    Grid->Width = nGridWidth;
    Grid->Height = nGridHeight;
    Grid->DefaultRowHeight =  nGridRowHeight;

    // кол-во строк учитывая верхню
    // если строк в секции меньше, чем значащих строк в таблице
    if (nSectionStringCount < nGridRowCount)
        Grid->RowCount = nGridRowCount + 1;
    else
        Grid->RowCount = nSectionStringCount + 1;
    // набор символов таблицы
    Grid->Canvas->Font->Charset = RUSSIAN_CHARSET;
    // если крупный щрифт
    if (dScr > 1)
     {
		Grid->Canvas->Font->Size = nCellFontSizeBF;
     }
    else // мелкий шрифт
     {
        Grid->Canvas->Font->Size = nCellFontSizeSF;
     }
    // установим размеры колонок
    for (i = 0; i < 5; i++)
     {
        Grid->ColWidths[i] = anColWidth[i];
     }
    // сформируем заголовок окна
    WindowCaption = GetPartitionTitle(nPartitionNumber) + " - " + SectionType[nSectionNumber];
    // установим заголовок окна
    Caption = WindowCaption;
    // определим кол-во операций, сначала считаем равным нулю
    nOperationCount = 0;
    // цикл по строкам
    for (i = 0; i < nSectionStringCount; i++)
        { // если в поле 1 что-то есть
	  if (!pSectionStrings[i].Field[0].IsEmpty())
           {
	     nOperationCount++;
	   }
	}
    if (nOperationCount > 0)
     { // выделим память под члены-данные
       pnOperationStart = new int[nOperationCount];
       pnOperationEnd = new int[nOperationCount];
       pbMadeOperation = new bool[nOperationCount];
       pOperationType = new OperationType[nOperationCount];
       pnVariantCount = new int[nOperationCount];
       ppnVariantStart = new PInt[nOperationCount];
       ppnVariantEnd = new PInt[nOperationCount];
     }
    // определим первые строки операций
    // номер текущей операции
    int nOperationNumber = 0;
    // цикл по строкам
    for (i = 0; i < nSectionStringCount; i++)
     { // если в поле 1 что-то есть
       if (!pSectionStrings[i].Field[0].IsEmpty())
        { // запомним начало операции
	  pnOperationStart[nOperationNumber] = i;
	  // перебор по символу в поле 1 первой строки операции
	  if (pSectionStrings[i].Field[1][1] == DIPOL_OPERATION_SYMBOL)
           {  // O
	     pOperationType[nOperationNumber] = otDipolOperation;
           }
	  if (pSectionStrings[i].Field[1][1] == ISR_OPERATION_SYMBOL)
           {  // И
	     pOperationType[nOperationNumber] = otISROperation;
           }
	  if (pSectionStrings[i].Field[1][1] == VARIANT_SYMBOL)
           {  // В
	     pOperationType[nOperationNumber] = otVariant;
           }
	  if (pSectionStrings[i].Field[1][1] == COMMAND_SYMBOL)
           { // К
	     pOperationType[nOperationNumber] = otCommand;
           }
	  if (pSectionStrings[i].Field[1][1] == REPORT_SYMBOL)
           {  // Д
	     pOperationType[nOperationNumber] = otReport;
           }
	  if (pSectionStrings[i].Field[1][1] == MESSAGE_SYMBOL)
           {  // П
    	     pOperationType[nOperationNumber] = otMessage;
	   }
	  nOperationNumber++;
	}
      }

 // установим флаги: операция не выполнена
	for (i = 0; i < nOperationCount; i++)
         { // флаг: операция не выполнена
	   pbMadeOperation[i] = false;
	 }
 // начало и конец текущей разбираемой операции
 int nCurrentOperationStart, nCurrentOperationMaxEnd;
 // цикл по операциям
 for (i = 0; i < nOperationCount; i++)
  {
    nCurrentOperationStart = pnOperationStart[i];
    // если мы на последней операции
    if (i == nOperationCount - 1)
      { // последняя строка операции - это последняя строка документа
	nCurrentOperationMaxEnd = nSectionStringCount - 1;
      }
    else // мы не на последней операции
     { // последняя строка операции - это строка перед первой строкой следующей операции
       nCurrentOperationMaxEnd = pnOperationStart[i + 1] - 1;
     }
    // сначала считаем, что последняя строка операции -
    // это макс. возможная последняя строка
    pnOperationEnd[i] = nCurrentOperationMaxEnd;
    // цикл по строкам текущей операции
	for (j = nCurrentOperationStart; j <= nCurrentOperationMaxEnd; j++ )
     { // если строка пуста или метка
	   if (( pSectionStrings[j].Field[0].IsEmpty() && (pSectionStrings[j].Field[1].IsEmpty()
			|| pSectionStrings[j].Field[1] == "М" ) && IsEmptyOrNShS( pSectionStrings[j].Field[2] )
				&& pSectionStrings[j].Field[3].IsEmpty() ) || (pSectionStrings[j].Field[1] == "М" ) )
		{ // предыдущая строка - конец текущей операции
		  pnOperationEnd[i] = j - 1;
		  // выйдем из разбора этой операции
		  break;
		}
     }
  }

    int nVariantCount=0;
    // цикл по операциям
    for (i = 0; i < nOperationCount; i++)
      { // если директива - вариант
	if (pOperationType[i] == otVariant)
         { // цикл по всем строкам операции снизу
           nVariantCount=0; // очистим кол-во вариантов
	   for (j = pnOperationEnd[i]; j >= pnOperationStart[i]; j--)
            { // если поле 1 строки не пусто
	      if (!pSectionStrings[j].Field[1].IsEmpty())
               { // переведем поле 1 строки в число
                 nVariantCount++;// = StrToInt(pSectionStrings[j].Field[1]);
                 //break;
	       }
	    }
           // запомним кол-во вариантов
           pnVariantCount[i] = nVariantCount-1;
           // выделим память
           ppnVariantStart[i] = new int[nVariantCount];
           ppnVariantEnd[i] = new int[nVariantCount];
           int nCurrentVariantNumber = 0, nVariantNumber;
	   // цикл по всем строкам операции сверху
	   for (j = pnOperationStart[i] + 1; j <= pnOperationEnd[i]; j++)
            { // если поле 1 строки не пусто
	      if (!pSectionStrings[j].Field[1].IsEmpty())
               { // переведем поле 1 строки в число
                 nVariantNumber = StrToInt(pSectionStrings[j].Field[1]);
                 if((nVariantNumber-nCurrentVariantNumber)!=1)
                    {
                      nVariantNumber=nCurrentVariantNumber+1;
                      //ShowMessage("ИСР: Неверная нумерация вариантов в опрации "+IntToStr(i));
                      //break;
                    }
                 // если перешли к след. номеру
                 if (nVariantNumber > nCurrentVariantNumber)
                  { // если перешли не к номеру 1
                    if (nVariantNumber != 1)
                     { // конец предыдущего варианта
                       ppnVariantEnd[i][nVariantNumber - 2] = j -1;
                     }
                    // начало варианта
                    ppnVariantStart[i][nVariantNumber - 1] = j;
                    // смена текущего варианта
                    nCurrentVariantNumber = nVariantNumber;
               }
	     }
	   }
            ppnVariantEnd[i][nVariantNumber - 1] = pnOperationEnd[i];
	  }
	  else  // директива - не вариант
            {
	      pnVariantCount[i] = 0;
	    }
       }

    if (nOperationCount == 0)
     { // операций нет
       nCurrentOperation = -1;
     }
    else
     { // начнем с первой операции
           nCurrentOperation = 0;
     }
    nAutoFirst = -1;
    // для секции НШС
    if (nSectionNumber == 1)
     {
        AnsiString Field1, Field2;
        int nField2Length;
        int nBlockStart;
        int nBlockStartLabelNumber;
        AnsiString BlockNumber;
        int nBlockNumber;
        enum {NotInBlock, InBlock} Position;
        nNShSBlockCount = 0;
        Position = NotInBlock;
        int nNShSLabelLength = NShSLabel.Length();
        int nNShSEndLabelLength = NShSEndLabel.Length();
        bool bGood;
        bool bStdMessage;
        // цикл по операциям
        for (i = 0; i < nSectionStringCount; i++)
         {
            bGood = true;
            bStdMessage = true;
            Field1 = pSectionStrings[i].Field[1];
            // поле 3
            Field2 = pSectionStrings[i].Field[2];
            nField2Length = Field2.Length();
            // если поле кончается как метка блока НШС
            if (Field2.SubString(nField2Length - nNShSLabelLength + 1, nNShSLabelLength) == NShSLabel && Field1 == LABEL_SYMBOL)
             {
              // если мы не в блоке НШС
              if (Position == NotInBlock)
               { // если начало метки - неправильное
                   if (Field2[1] != cNShSLabelSymbol)
                    {
                        bGood = false;
                    }
                // если имеет смысл продолжать
                if (bGood)
                 { // номер блока НШС
                   BlockNumber = Field2.SubString(2, nField2Length - nNShSLabelLength - 1);
                   nBlockNumber = atoi(BlockNumber.c_str());
                   // если преобразование - неудачно
                   if (nBlockNumber == 0)
                    {
                            bGood = false;
                    }
                 }
                 // если имеет смысл продолжать
                 if (bGood)
                  { // запомним начало блока
                    nBlockStart = i;
                    // номер блока
                    nBlockStartLabelNumber = nBlockNumber;
                    Position = InBlock;
                  }
               }
               // мы в блоке НШС
               else
                 { // если строка начинается не с символов метки конца блока
                   if (Field2.SubString(1, nNShSEndLabelLength) != NShSEndLabel)
                    {
                        bGood = false;
                    }
                    // если имеет смысл продолжать
                    if (bGood)
                     { // если длина слишком мала
                       if (nField2Length <= nNShSEndLabelLength)
                         {
                            bGood = false;
                         }
                     }
                    // если имеет смысл продолжать
                    if (bGood)
                    { // если нет символа НШС
                      if (Field2[nNShSEndLabelLength + 1] != cNShSLabelSymbol)
                       {
                            bGood = false;
                       }
                    }
                   // если имеет смысл продолжать
                   if (bGood)
                    { // номер блока НШС
                       BlockNumber = Field2.SubString(nNShSEndLabelLength + 2, nField2Length - nNShSLabelLength - nNShSEndLabelLength - 1);
                       nBlockNumber = atoi(BlockNumber.c_str());
                       // если преобразование - неудачно
                       if (nBlockNumber == 0)
                         {
                            bGood = false;
                         }
                    }
                    // если имеет смысл продолжать
                    if (bGood)
                     { // если номер метки конца блока не совпал с меткой начала блока
					   if( nBlockNumber != nBlockStartLabelNumber )
                        {
                            bGood = false;
                        }
                     }
                    // если имеет смысл продолжать
					if(bGood)
					 {
					   if( nNShSBlockCount == MaxNShSBlockCount )
                        {
                            bGood = false;
							ErrorMessage = "Количество блоков НШС в секции превышает максимально допустимое (" + IntToStr(MaxNShSBlockCount) + ").";
                        }
                     }
                    // если имеет смысл продолжать
					if(bGood)
                     {
                        aNShSBlock[nNShSBlockCount].nBlockNumber = nBlockNumber;
                        aNShSBlock[nNShSBlockCount].nBlockStart = nBlockStart;
                        aNShSBlock[nNShSBlockCount].nBlockEnd = i;
                        nNShSBlockCount++;
                        Position = NotInBlock;
                     }
                }
                // при анализе выявили ошибку
                if (!bGood)
				 { // если стандартное сообщение об ошибке
                   if (bStdMessage)
                    {
						ErrorMessage = "Некорректно расставлены метки блоков НШС. Ошибка в строке " + IntToStr(i + 1) + " .";
                    }
                   // выведем сообщение
                   ErrorMessageBox(ErrorMessage);
                   // кончаем анализ
                   break;
                 }
            }
        }
    }

    DrawGrid();
    // номер последней выполненной операции
    // вначале имеет значение -1
    nLastMadeOperation = -1;
    // закончили конструктор
    bIsConstructed = false;
}
//---------------------------------------------------------------------
void __fastcall TMDIChild::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------
void __fastcall TMDIChild::GridDrawCell(TObject *Sender, int nCol,
	  int nRow, TRect &Rect, TGridDrawState State)
{
    TColor FontColor, BackColor;
    AnsiString OutString, FontName;
    // флаг: жирный шрифт
    bool bBold = false;
    // отступ
    int nIndent;
    bool bMadeOperation = false;
    // признак, что была выполнена последняя операция раздела в данной секции
    bool bMadeLastOperation = false;
    int i;

    // если строка без текста (внизу)
    if (nRow - 1 > nSectionStringCount - 1)
	 { // заливка ячейки
	   Grid->Canvas->Brush->Color = clGray;
	   Grid->Canvas->FillRect(Rect);
	   return;
     }
    // если шапка
    if (nRow == 0)
     {
       if (nCol == 0)
        {
           OutString = "";
        }
       else
        {
		   OutString = aHeader[nSectionNumber][nCol - 1];
        }
	   BackColor = (TColor)nHeaderColor;
	   FontColor = (TColor)nNonSelFontColor;
	   FontName = "Courier New";
	   bBold = true;
	   nIndent = nCellTextIndent + 1;
	 }
	else // не шапка
     { // номер строки начиная не от шапки, а от первой строки таблицы
       int nCorrRow = nRow - 1;
       // определим номер операции
       int nDrawingOperation = -1;
       // цикл по операциям
       for (i = 0; i < nOperationCount; i++)
        { // если мы на операции
          if(nCorrRow >= pnOperationStart[i] && nCorrRow <= pnOperationEnd[i])
           {
             nDrawingOperation = i;
             break;
           }
        }
       // если мы на какой-либо операции
       if (nDrawingOperation >= 0)
        { // если выполненная операция
          if (pbMadeOperation[nDrawingOperation])
           { // жирный шрифт
             bMadeOperation = true;
             if((nDrawingOperation==nOperationCount-1)
                 &&(nLastMadeOperation!=nOperationCount-2))
               bMadeLastOperation = true;
             else  bMadeLastOperation = false;
             //WriteToDebugFile(IntToStr(nLastMadeOperation));
             //WriteToDebugFile("nCurrentOperation="+IntToStr(nCurrentOperation));
             //WriteToDebugFile("nDrawingOperation="+IntToStr(nDrawingOperation));
           }
        }
        // определим текст
        if (nCol == 0)
         { // если мы в блоке "Авто"
           if (nAutoFirst != -1)
             {
                if(nCorrRow >= pnOperationStart[nAutoFirst] && nCorrRow <= pnOperationEnd[nAutoLast])
                 {
                    OutString = cAuto;
                 }
                else
                 {
                    OutString = "";
                 }
             }
            else
             {
                OutString = "";
             }
         }
        else
         { // текст из массива строк секции
           OutString = pSectionStrings[nRow - 1].Field[nCol - 1];
         }
        // жирный шрифт - выполненная ли операция
        bBold = bMadeOperation;
        FontName = "Courier New";
        bool bProcessedOperation = false;
        // если ПРИС обрабатывает операцию
        if (bPrisExecutesOperation)
         { // если операция, из которой послан запрос
           if (nDrawingOperation == nExecutesOperationNumber)
            { // если выбран вариант
              if (bSelectedVariant)
               { // если на варианте
                 if (nCorrRow >= ppnVariantStart[nVariantOperationNumber][nChosenVariantNumber - 1] && nCorrRow <= ppnVariantEnd[nVariantOperationNumber][nChosenVariantNumber - 1])
                  {
                    bProcessedOperation = true;
                  }
               }
              // прис обрабатывает строку типа 'О'
              else
               {
                  bProcessedOperation = true;
               }
            }
         }
       // если обрабатывается операция ИСР "Загрузка раздела"
       if (bISRPartitionOperationProcess)
        { // если операция, из которой послан запрос
          if (nDrawingOperation == nExecutesOperationNumber)
           {
              bProcessedOperation = true;
           }
        }
       // флаг: рисуем фон
       bool bBackGround = false;
       // если обрабатываемая ПРИСом операция
       if (bProcessedOperation)
        {
            FontColor = (TColor)nSelFontColor;
            BackColor = (TColor)nGridSelColor;
        }
        else
         {
            FontColor = (TColor)nNonSelFontColor;
            // если текущая операция
            if ((nCurrentOperation != -1 )&&
               (nDrawingOperation == nCurrentOperation)
               &&!bMadeLastOperation)
             {
               //bMadeLastOperation = false;
               // если выделен вариант
               if (bSelectedVariant)
                 { // если мы на строке, в которой выделен вариант
                   if (nCorrRow >= ppnVariantStart[nCurrentOperation][nChosenVariantNumber - 1] && nCorrRow <= ppnVariantEnd[nCurrentOperation][nChosenVariantNumber - 1])
                     {
                        BackColor = (TColor)nCurOperationBackColor;
                     }
                    else
                      {
                        bBackGround = true;
                      }
                  }
                else
                  {
                     BackColor = (TColor)nCurOperationBackColor;
                  }
             }
            // не текущая операция
            else
             {
                bBackGround = true;
             }
            // если рисуем фон
            if (bBackGround)
             { // если выполненная операция
               if (bMadeOperation)
                {
                    BackColor = (TColor)nMadeOperationColor;
                }
               else // невыполненная операция
                 {
                    BackColor = (TColor)nGridBackColor;
                 }
              }
          }
        nIndent = nCellTextIndent;
    }
	// заливка ячейки
	Grid->Canvas->Brush->Color = BackColor;
	Grid->Canvas->FillRect(Rect);
    // цвет шрифта
    Grid->Canvas->Font->Color = FontColor;
	Grid->Canvas->Font->Name = FontName;
    // если жирный шрифт
    if (bBold)
     {
        Grid->Canvas->Font->Style = TFontStyles()<< fsBold;
     }
    else
     {  // шрифт без атрибутов стиля
		Grid->Canvas->Font->Style = TFontStyles();
     }
    // выведем текст
    Grid->Canvas->TextOut(Rect.Left + nIndent, Rect.Top, OutString);
}
//---------------------------------------------------------------------------

void __fastcall TMDIChild::GridDblClick(TObject *Sender)
{
	// если выполняется текущая операция
    if (bPrisExecutesOperation)
     {
        return;
     }
    // если в данный момент обрабатывается операция ИСР "Загрузка раздела"
    if (bISRPartitionOperationProcess)
     {
        return;
     }
    // выделенная строка
    int nSelRow = Grid->Row - 1;
    // определим номер операции
    int nOperationWithSel = -1;
    // цикл по операциям
    for (int i = 0; i < nOperationCount; i++)
     { // если мы на операции
	   if(nSelRow >= pnOperationStart[i] && nSelRow <= pnOperationEnd[i])
        {
            nOperationWithSel = i;
            break;
        }
     }
	bool bMakeCurrentOperation = false;
    // если мы щелкнули на операции
	if (nOperationWithSel != -1)
     { // если переход в вверх не разрешен
	   if (!bAllowBackCrossing)
        { // если номер операции меньше текущего и не мое рабочее место
          if (nOperationWithSel < nCurrentOperation && !bDeveloperWorkPlace )
            {
				WarningMessageBox( "Вы не можете перемещаться вверх по секции, так как это запрещено в текущих параметрах настройки" );
                return;
            }
        }
	 }
  // если мы на операции
  if (nOperationWithSel != -1)
	{  // если на текущей операции
	   if (nOperationWithSel == nCurrentOperation)
		{ // если есть выбранный вариант
          if (bSelectedVariant)
           { // если мы на строке выбранного варианта
			 if (nSelRow >= ppnVariantStart[nCurrentOperation][nChosenVariantNumber - 1] && nSelRow <= ppnVariantEnd[nCurrentOperation][nChosenVariantNumber - 1])
              {
                    bMakeCurrentOperation = true;
              }
           }
            else
             {
			   if (pbMadeOperation[nCurrentOperation])
				  {
                     pbMadeOperation[nCurrentOperation] = false;
					 Grid->Refresh();
				  }
			   //else
				bMakeCurrentOperation = true;
             }
		}
		// мы не на текущей операции
		else
		 {
		   if (!bSelectedVariant)                      // если нет выбранного варианта
			{
			  nCurrentOperation = nOperationWithSel;   // сделаем оп-ю с выделенной строкой текущей
			  Grid->Refresh();                         // перерисуем таблицу
			  bMakeCurrentOperation = true;
			}
		 }
	}
  // если выполнить операцию
	if (bMakeCurrentOperation)
	 {
	   if (nCurrentOperation == nOperationCount - 1)   // если мы на посл. операции
		{
		   String sOperationWasMade = L"Эта операция уже выполнена. Выполнить ее еще раз?";
		   String Attention = L"Внимание!";
           // если операция выполнена
		   if (pbMadeOperation[nCurrentOperation])
			{
			  if ( MessageBox( Handle, L"Эта операция уже выполнена. Выполнить ее еще раз?", L"Внимание!", MB_YESNO) == IDNO)
               {
                    bMakeCurrentOperation = false;
               }
			  else
				 Grid->Refresh();
			}
        }
        // если выполнить операцию
        if (bMakeCurrentOperation)
         {
		   if (nCurrentOperation > nLastMadeOperation + 1)
			{
			   String Message = L"Вы хотите выполнить операции непоследовательно. Вы сделали переход вперед и пропустили " + IntToStr(nCurrentOperation - nLastMadeOperation - 1) + " операций. Вы уверены, что необходимо выполнить текущую операцию?";
			   String Attention = L"Внимание!";
			   if (MessageBox( Handle, L"Вы хотите выполнить операции непоследовательно. Вы сделали переход вперед и пропустили ", L"Внимание!", MB_YESNO ) == IDNO )
                {
                    bMakeCurrentOperation = false;
				}
			}
		 }
        // если выполнить операцию
        if (bMakeCurrentOperation)
         {
			MakeOperation();
         }
     }
 }
// функция выполяет текущую операцию
// перед этим проверяет, допустимо ли выполнить операцию из данного раздела
// если недопустимо, функция выдает предупр. сообщение и не выполняет операцию
void __fastcall TMDIChild::MakeOperation()
{
    if(ISR_prot)  // признак ожидания ответа на СП
       {  ErrorMessageBox("Не завершилась операция СП");
          return;
       }
    // если мы не в последнем разделе
    if (nCurrentPartition < nPartitionCount - 1)
     {
		WarningMessageBox( "Операция из данного раздела не может быть выполнена, так как данный раздел не является текущим" );
        return;
     }
    // выполним
	MakeCurrentOperation();
}

//---------------------------------------------------------------------------
void __fastcall TMDIChild::MakeCurrentOperation()
{
    int nChosenVariant;
    bool bMade = false;
    int nAnswer;
    int i;
    AnsiString Message;
    AnsiString CurrentPart;
    if (nCurrentOperation < nLastMadeOperation)
	 {
		Message = "Совершен переход назад при перемещении по секции \""
				  + SectionType[nSectionNumber] + "\" в разделе \""
				  + GetPartitionTitle(nCurrentPartition) + "\" с операции";
        // если секция НШС
		if (nSectionNumber == 1 )
         {
            Message += pSectionStrings[pnOperationStart[nLastMadeOperation]].Field[0] + "(" + NShSBlockDescription(nLastMadeOperation) + ")";
         }
        else
         {
            Message += pSectionStrings[pnOperationStart[nLastMadeOperation]].Field[0];
         }
		Message += " на операцию";
        // если секция НШС
        if (nSectionNumber == 1 )
         {
            Message += pSectionStrings[pnOperationStart[nCurrentOperation]].Field[0] + "(" + NShSBlockDescription(nCurrentOperation) + ")";
         }
        else
         {
            Message += pSectionStrings[pnOperationStart[nCurrentOperation]].Field[0];
         }
		SendStringToPrisProtokol(Message, stBlackBold, true, true, false);
    }
    switch (pOperationType[nCurrentOperation])
    {
        case (otVariant): // если есть выбранный вариант
            if (bSelectedVariant)
             {
				int nVariantStart = ppnVariantStart[nVariantOperationNumber][nChosenVariantNumber - 1];
                int nVariantEnd = ppnVariantEnd[nVariantOperationNumber][nChosenVariantNumber - 1];
                bool ISROperation;
                if (pSectionStrings[nVariantStart].Field[3].Trim().SubString(1, ISRStartDirName.Length()) == ISRStartDirName)
                 {
                    ISROperation = true;
                    RequestBox->SetCurrentType(otISROperation);
                 }
                else
                 {
                    ISROperation = false;
                    RequestBox->SetCurrentType(otDipolOperation);
                 }
				// запрос
				RequestBox->SetText( this, nVariantStart, nVariantEnd, 3 );
				if( RequestBox->OperationTextMemo->Lines->Count == 0 )
				 {
				   ShowMessage( "ИСР: пустое поле операции в файле структуры");
				   bSelectedVariant = false;
				   AnswerForKO( 0 ); //-1 );
				   return;
				 }
				RequestBox->ShowModal();
                // если ответ - выполнить вариант
				if( RequestBox->GetMakeOperation() )
                 {
                    if (ISROperation)
					 {
						MakeISROperation( nVariantStart, nVariantEnd );
                     }
                    else
                     {
                        MakeDipolOperation(nVariantStart, nVariantEnd);
                     }
                    ISRMainForm->ConfigureMenu();
                 }
                else // дали отмену
					bSelectedVariant = false;
             }
            // выбранного варианта нет
            else
             { // установим кол-во вариантов
               VariantBox->SetVariantCount(pnVariantCount[nCurrentOperation]);
               // запросим вариант
               VariantBox->ShowModal();
               // запросим номер выбранного варианта
               nChosenVariant = VariantBox->GetChosenVariant();
               // если какой-либо вариант выбран
               if (nChosenVariant > 0)
                {
				   bSelectedVariant = true;
                   // номер операции "Вариант", в которой сейчас выбран один из вариантов
                   nVariantOperationNumber = nCurrentOperation;
                   // номер выбранного варианта
                   nChosenVariantNumber = nChosenVariant;
                   // меню
                   ISRMainForm->ConfigureMenu();
                }
             }
            break;
        case (otDipolOperation): // если не вып. блок "Авто"
            if (!bExecuteAutoBlock)
             { // запрос
               RequestBox->SetCurrentType(otDipolOperation);
               RequestBox->SetText(this, pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation], 3);
               RequestBox->ShowModal();
             }
            // если ответ - выполнить операцию или выполняется блок "Авто"
            if (RequestBox->GetMakeOperation() || bExecuteAutoBlock)
             {
                MakeDipolOperation(pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation]);
             }
            break;
        case (otCommand):
            RequestBox->SetCurrentType(otCommand);
			RequestBox->SetText( this, pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation], 2);
            RequestBox->ShowModal();
            // если ответ - выполнить операцию
			if( RequestBox->GetMakeOperation() )
             {
				SendStringToPrisProtokol( "Операция 'Команда'", stStandart, true, true, false);
				// цикл по строкам текущей операции
				for( i = pnOperationStart[nCurrentOperation]; i <= pnOperationEnd[nCurrentOperation]; i++ )
                 { // текст операции
				   SendStringToPrisProtokol(pSectionStrings[i].Field[2], st_Blue, false, false, true);
				 }
				SendStringToPrisProtokol( "Конец операции 'Команда'", stStandart, true, true, false);
				// перейдем к след. операции
                bMade = true;
                nLastMadeOperation = nCurrentOperation;
             }
			break;
        case (otReport):
            RequestBox->SetCurrentType(otReport);
            RequestBox->SetText(this, pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation], 2);
            RequestBox->ShowModal();
            // если ответ - выполнить операцию
            if (RequestBox->GetMakeOperation())
             {
				SendStringToPrisProtokol( "Операция 'Доклад'", stStandart, true, true, false);
                // цикл по строкам текущей операции
                for (i = pnOperationStart[nCurrentOperation]; i <= pnOperationEnd[nCurrentOperation]; i++)
                {
//                    for (int j = 1; j < 10000; j++)
                        // текст операции
						SendStringToPrisProtokol(pSectionStrings[i].Field[2], st_Blue, false, false, true);
                }
				SendStringToPrisProtokol( "Конец операции 'Доклад'", stStandart, true, true, false );
                // перейдем к след. операции
               bMade = true;
               nLastMadeOperation = nCurrentOperation;
            }
            break;
        case (otMessage):
            RequestBox->SetCurrentType(otMessage);
            RequestBox->SetText(this, pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation], 2);
            RequestBox->ShowModal();
            // если ответ - выполнить операцию
            if (RequestBox->GetMakeOperation())
             {
				SendStringToPrisProtokol( "Операция 'Сообщение оператору'", stStandart, true, true, false);
                // цикл по строкам текущей операции
                for (i = pnOperationStart[nCurrentOperation]; i <= pnOperationEnd[nCurrentOperation]; i++)
                 { // текст операции
				   SendStringToPrisProtokol(pSectionStrings[i].Field[2], st_Blue, false, false, true);
				 }
				SendStringToPrisProtokol( "Конец операции 'Сообщение оператору'", stStandart, true, true, false);
                // перейдем к след. операции
                bMade = true;
                nLastMadeOperation = nCurrentOperation;
             }
            break;
        case (otISROperation):
            AnsiString OperationText = pSectionStrings[pnOperationStart[nCurrentOperation]].Field[3].Trim();
			if( OperationText == "СП" )
             {
                ISR_prot = 1; // будем ждать ответ
				if( SPBox )
					 SPBox->ShowModal();
				else
					 break;
                bMade = true;
                nLastMadeOperation = nCurrentOperation;
                if(ISR_prot == 0)
                          return;   // дали отмену СП
                // если выполняем блок "Авто"
                if (bExecuteAutoBlock)
              { // остановим выполнение
                   // текущая операция по-прежнему первая в блоке "Авто"
                   StopExecuteAutoBlock();
                   // }
                // если поступила заявка на остановку блока "Авто"
                if (bStopAutoRequest)
                 { // сбросим ее, типа обработали, но делать ниче не надо
                    bStopAutoRequest = false;
                 }
                bAutoBlockFromPris = false;
                if (nAutoLast == nCurrentOperation)
                 {
                    ClearSelection();
                 }
                else
                 {
                    nAutoFirst++;
                 }
               }  //  if (bExecuteAutoBlock)
              }
            else
             { // если не вып. блок "Авто"
               if (!bExecuteAutoBlock)
                {
                    RequestBox->SetCurrentType(otISROperation);
                    RequestBox->SetText(this, pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation], 3);
                    RequestBox->ShowModal();
                    nExecutesOperationNumber = nCurrentOperation;
                }
				// если ответ - выполнить операцию или выполняется блок "Авто"
				if( RequestBox->GetMakeOperation() || bExecuteAutoBlock )
                 {
                    bISRPartitionOperationProcess = MakeISROperation(pnOperationStart[nCurrentOperation], pnOperationEnd[nCurrentOperation]);
                    // если оп. не вып.
					if (!bISRPartitionOperationProcess)
					 {
					   if( bExecuteAutoBlock )        // если выполняем блок "Авто"
						{
													  // текущая операция по-прежнему первая в блоке "Авто"
							StopExecuteAutoBlock();   // остановим выполнение
						}
                       // если поступила заявка на остановку блока "Авто"
                       if (bStopAutoRequest)
                        { // сбросим ее, типа обработали, но делать ниче не надо
                          bStopAutoRequest = false;
                        }
                        bAutoBlockFromPris = false;
                     }
                    else
                     {
                        nExecutesOperationNumber = nCurrentOperation;
                     }
                 }
            }
            break;
    }
	if( bMade )
	 {
	   pbMadeOperation[nCurrentOperation] = true;    // флаг выполненной операции
	   GoNextOperation(true);                        // перейдем к след. операции
     }
    else
	 {
	   DrawGrid();                                   // перерисуем таблицу
     }
}
//------------------------------------------------------------------------------
// функция выполняет операцию диполя (КО)
// операция берется из полей 4 строк с nFirstString по nLastString
//------------------------------------------------------------------------------
void __fastcall TMDIChild::MakeDipolOperation( int nFirstString, int nLastString )
{
    AnsiString Operation, CurrentPart;
	int nOperationLength, i;
    // цикл по строкам текущей операции
	for( i = nFirstString; i <= nLastString; i++ )
     {
        CurrentPart = pSectionStrings[i].Field[3];
        // если не коментарий
		if (!IsComment(CurrentPart))
		 {
		   Operation += CurrentPart + " ";   // текст операции
         }
     }
    /*if(Operation==" ")
         {  // флаг выполненной операции
            pbMadeOperation[nCurrentOperation] = true;
            // перейдем к след. операции
            GoNextOperation(true);
            return;
         }*/
    // если есть пробелы
	Operation = Operation.Trim();
	nOperationInPrisCount++;
    aOperationInPris[nOperationInPrisCount - 1].nPartitionNumber = nPartitionNumber;
	aOperationInPris[nOperationInPrisCount - 1].nSectionNumber = nSectionNumber;
    // если операция "ВЫЗВАТЬ"
    if (Operation.SubString(1, ProgramCallStament.Length()).AnsiCompareIC(ProgramCallStament) == 0)
     {
        aOperationInPris[nOperationInPrisCount - 1].bIsProgramCallOperation = true;
     }
    else
     {
        aOperationInPris[nOperationInPrisCount - 1].bIsProgramCallOperation = false;
     }
    // если операция "СП"
    bool pr = false; // признак наличия '*' - сохр.текущ.прот.
	if( Operation.SubString( 1, SPStament.Length()).AnsiCompareIC(SPStament) == 0 )
     {
	   aOperationInPris[nOperationInPrisCount - 1].bIsSPOperation = true;
	   short ln = Operation.Length();
	   if( ln < 33 )                     // max имя протокола
		{
		  for( i = 0; i < ln - 2; i++ )
		   {
			 if( ( Operation.c_str() + 2 + i )[0] == ' ' )
						 continue;
			 if( ( Operation.c_str() + 2 + i )[0] == '*' )
				   {
					 pr = true;
					 continue;
				   }
			 pr = false;
		   }
		  try {
				if(pr){
						SPBox->FormActivate(0); // сохраняем
						SPBox->OKBtnClick(0);
						ISRMainForm->WorkSP->Enabled = true;
					  }
			   }
		  catch(...) {
						ShowMessage ("ИСР: Сбой при СП в структуре" );
						WriteToDebugFile( "ИСР: Сбой при СП в структуре" );
						return;
					  }
		}
	 }
    else
     {
        aOperationInPris[nOperationInPrisCount - 1].bIsProgramCallOperation = false;
     }
    CommentToCurrentOperation = GetCurrentPosition(nFirstString, nLastString);

    int nOperationNumber = 0;
    if(pr)
     {
       bSendInternalOperationToPris = false;
       externalOperation[totalOperationsInPris] = true;
       totalOperationsInPris++;
     }
    else nOperationNumber = SendKOToPris(Operation, true, CommentToCurrentOperation, bExecuteAutoBlock);
    // номер операции
    aOperationInPris[nOperationInPrisCount - 1].nOperationNumber = nOperationNumber;
    // начнем операцию
    StartOperation();
    DrawGrid();
}
//------------------------------------------------------------------------------
// выдать текущее положение в виде строки
//------------------------------------------------------------------------------
AnsiString __fastcall TMDIChild::GetCurrentPosition( int nFirstString, int nLastString )
{
  AnsiString Comment = "раздел \"" + GetPartitionTitle(nCurrentPartition) + "\", секция " + SectionType[nSectionNumber] + ", ";
  int i;
  // если секция НШС
  if( nSectionNumber == 1 )
   {
	 Comment += NShSBlockDescription(nCurrentOperation) + ", ";
   }
  if( pnVariantCount[nCurrentOperation] == 0 )                 // не ВАРИАНТ
	   Comment += "операция " + pSectionStrings[nFirstString].Field[0];
  else  // добавим номер самой операции ВАРИАНТ
	{
	  for( i = 1; i <= nFirstString; i++ )
		if( !pSectionStrings[nFirstString - i].Field[0].IsEmpty() )
		 {
		   Comment += "операция " + pSectionStrings[nFirstString - i].Field[0];
		   break;
		 }
	}
  return (Comment);
}
//------------------------------------------------------------------------------
// функция выполняет операцию ИСР
// операция берется из полей 4 строк с nFirstString по nLastString
// пока выполняется только операция "Запуск раздела"
// результат - выполнилась ли операция
//------------------------------------------------------------------------------
bool __fastcall TMDIChild::MakeISROperation( int nFirstString, int nLastString )
{
    bool bResult;
    int i;
    AnsiString Operation;
    AnsiString StringToProtokol;
    int nOperationLength;
    // цикл по строкам текущей операции
    for (i = nFirstString; i <= nLastString; i++)
     { // текст операции
        Operation += pSectionStrings[i].Field[3] + " ";
     }
    // длина операции
    nOperationLength = Operation.Length();
    // если на конце пробел
    if (Operation[nOperationLength] == ' ')
     { // обрежем
       Operation = Operation.SubString(1, nOperationLength - 1);
     }
    // найдем начало операции "Запуск раздела"
    AnsiString OperationParameters, OperationParametersWithSpaces;
    int nSymbolCountBeforeFirstSymbolAfterOperationName;
    char* pcStartISROperation = Operation.c_str();
    char* pcStartOperationName = strstr(pcStartISROperation, ISRStartDirName.c_str());
    char* pcFirstSymbolAfterOperationName;
    bool bGood = true;
    if (pcStartOperationName == NULL)
     {
		ErrorMessageBox( "Синтаксическая ошибка в директиве ИСР: неизвестное имя директивы ИСР" );
        bGood = false;
     }
	if (bGood)
     {
        pcFirstSymbolAfterOperationName = pcStartOperationName + ISRStartDirName.Length();
        nSymbolCountBeforeFirstSymbolAfterOperationName = pcFirstSymbolAfterOperationName - pcStartISROperation;
        OperationParametersWithSpaces = Operation.SubString(nSymbolCountBeforeFirstSymbolAfterOperationName + 1, Operation.Length() -nSymbolCountBeforeFirstSymbolAfterOperationName);
        OperationParameters = OperationParametersWithSpaces.Trim();
        // заименим подстроки из нескольких пробелов на одиночные пробелы
        for (i = 0; i < 2; i++)
         {
            ReplaceMultiSpaceToOneSpace(OperationParameters);
         }
        CommentToCurrentOperation = GetCurrentPosition(nFirstString, nLastString);
        // marker
		StringToProtokol = "Внутренняя директива (" + CommentToCurrentOperation + ")";
		SendStringToPrisProtokol( StringToProtokol, stStandart, true, true, false );
        // выполним
        bResult = ISRMainForm->RecievePartitionOperation(OperationParameters, false, Operation);
        if (bResult)
         { // запомним, что из данной секции вызван раздел
            LoadedPartitions[nPartitionNumber].nSectionFromISRPartitionOperationMake = nSectionNumber;
         }
		else
         {
			StringToProtokol = "Конец внутренней директивы (" + CommentToCurrentOperation + ")";
			SendStringToPrisProtokol(StringToProtokol, stStandart, true, true, false);
         }
     }
    else
     {
        bResult = false;
     }
    return (bResult);
}

void __fastcall TMDIChild::GridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    // если ПРСом выполняется операция из данной секции
    if (bPrisExecutesOperation)
     {
        return;
     }
    // если в данный момент обрабатывается операция ИСР "Загрузка раздела"
    if (bISRPartitionOperationProcess)
     {
        return;
     }
    // если операций нет
    if (nOperationCount == 0)
     {
        return;
     }
    int nGridTopRow;
    switch (Key)
     {
        case (13): // если нажат Enter
            MakeOperation();
            Key = 0;
            break;
        case (VK_DOWN): // если сейчас не выбран вариант
            if (!bSelectedVariant)
             {
                if (nCurrentOperation < nOperationCount - 1)
                 { // перейдем к след. операции
                   nCurrentOperation++;
                   // если конец новой операции ниже нижней строки сетки
                   if (pnOperationEnd[nCurrentOperation] > Grid->TopRow + nGridRowCount - 2)
                    { // кол-во строк снизу
                       Grid->TopRow = pnOperationEnd[nCurrentOperation] - nGridRowCount + 2;
                    }
                    // перерисуем таблицу
                    DrawGrid();
                 }
             }
            Key = 0;
            break;
        case (VK_UP): // если сейчас не выбран вариант
            if (!bSelectedVariant)
             { // если переход вверх разрешен или рабочее место
                if (bAllowBackCrossing || bDeveloperWorkPlace)
                 {
                    if (nCurrentOperation > 0)
                     { // перейдем к пред. операции
                       nCurrentOperation--;
                       // если начало новой операции за пределами верхней границы сетки
                       if (pnOperationStart[nCurrentOperation] < Grid->TopRow - 1)
                        { // начнем просмотр с первой строки новой операции
                          Grid->TopRow = pnOperationStart[nCurrentOperation] + 1;
                        }
                        // перерисуем таблицу
                        DrawGrid();
                     }
                 }
             }
            Key = 0;
            break;
        case (VK_PRIOR):
        case (VK_NEXT):
            Key = 0;
            break;
        case (VK_F7):
            ISRMainForm->KeyPressedInChildWindow(kpF7);
            Key = 0;
            break;
        case (VK_F9):
            ISRMainForm->KeyPressedInChildWindow(kpF9);
            Key = 0;
            break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMDIChild::FormDestroy(TObject *Sender)
{
    int i;
    // цикл пор операциям
    for (i = 0; i < nOperationCount; i++)
     { // если "Вариант"
       if (pOperationType[i] == otVariant)
        {
            delete[] ppnVariantStart[i];
            delete[] ppnVariantEnd[i];
        }
     }
    if (nOperationCount > 0)
     {
    	delete[] pnOperationStart;
    	delete[] pnOperationEnd;
    	delete[] pbMadeOperation;
    	delete[] pOperationType;
    	delete[] pnVariantCount;
        delete[] ppnVariantStart;
        delete[]ppnVariantEnd;
     }
    if (nSectionStringCount > 0)
     {
        delete[] pSectionStrings;
     }
}
//---------------------------------------------------------------------------

int __fastcall TMDIChild::GetSectionNumber()
{
    return (nSectionNumber);
}

//---------------------------------------------------------------------------
// функция перерисовывает таблицу
//---------------------------------------------------------------------------
void __fastcall TMDIChild::DrawGrid()
{
    // если текущая операция есть
	if( nCurrentOperation != -1 )
	 {
		// если начало новой операции выше верхней строки сетки
		if( pnOperationStart[nCurrentOperation] < Grid->TopRow - 1 )
         {
            Grid->TopRow = pnOperationStart[nCurrentOperation] + 1;
         }
        // если конец новой операции ниже нижней строки сетки
		if( pnOperationEnd[nCurrentOperation] > Grid->TopRow + nGridRowCount - 2 )
		 {
            Grid->TopRow = pnOperationEnd[nCurrentOperation] - nGridRowCount + 2;
		 }
     }
    Grid->Refresh();
    // если мы не на конструкторе
    if( !bIsConstructed )
      {
        ISRMainForm->ConfigureMenu();
      }
}

//---------------------------------------------------------------------------
// отмена текущего выбранного варианта
//---------------------------------------------------------------------------
void __fastcall TMDIChild::UndoChosenVariant()
{
	bSelectedVariant = false;
    DrawGrid();
    ISRMainForm->ConfigureMenu();
}
//----------------------------------------------------------------
// ответ от АПОРТа
//----------------------------------------------------------------
void __fastcall TMDIChild::AnswerForKO( int nAnswer )
{
    // если операция вызова программы
	if( aOperationInPris[nOperationInPrisCount - 1].bIsProgramCallOperation )
		ISRMainForm->SetMainWindowOnTop();             // сделаем окно активным
   // если операция выполнена
	if( nAnswer != 2 )
		nLastMadeOperation = nCurrentOperation;
	switch( nAnswer )
	 {
	   case(-1): // ошибка в файле структуры
				 bAutoBlockFromPris = false;
				 DrawGrid();
				 break;
	   case (1): // операция выполнена ненормально
				 SendStringToPrisProtokol( L"ИСР: Конец директивы (" + CommentToCurrentOperation + ")",
									 stStandart, false, true, false );
	   case (2): // БИНТ занят
				 if( nAnswer == 1 )
					ErrorMessageBox( L"Операция выполнена НЕНОРМАЛЬНО", L"Ответ от ПРИС" );
				 if( nAnswer == 2 )
					InformationMessageBox( L"Операция не может быть выполнена, так как идет обработка испытательной директивы", L"Ответ от ПРИС" );
				 // если был выбран вариант
				 if( bSelectedVariant )
					bSelectedVariant = false;
				 EndOperation();
				 nOperationInPrisCount--;          // кол-во операций в ПРИС
				 if( bExecuteAutoBlock )           // если выполняем блок "Авто"
					StopExecuteAutoBlock();        // остановим вып-е, текущ. оп-я по-прежнему 1-я в блоке "Авто"
				 if(  bStopAutoRequest )           // если поступила заявка на остановку блока "Авто"
					bStopAutoRequest = false;      // сбросим ее, как обработали, но делать ничего не надо
				 bAutoBlockFromPris = false;
				 DrawGrid();
				 break;

		case (0): // операция выполнена нормально
				  if( ISRMainForm->prer )          // была прервана принудительно
				   {
					  SendStringToPrisProtokol( "ИСР: Оператор прервал директиву (" + CommentToCurrentOperation + ")", stStandart, false, true, false);
				   }
				  else
					SendStringToPrisProtokol( "ИСР: Конец директивы (" + CommentToCurrentOperation + ")", stStandart, false, true, false);
				  if( !bExecuteAutoBlock )
					InformationMessageBox( L"Операция выполнена нормально", L"Ответ от ПРИС" );
				  nOperationInPrisCount--;         // кол-во операций в ПРИС
				  if( bSelectedVariant )           // если был выбран вариант
					   bSelectedVariant = false;
				  EndOperation();
				  pbMadeOperation[nCurrentOperation] = true;
				  GoNextOperation(false);
				  if( bExecuteAutoBlock )          // если выполняем блок "Авто"
					   ProcessAuto();
				  DrawGrid();
				  break;
    }
}
//------------------------------------------------------------------------------
// обрабтка блока "Авто" после выполнения операции
//------------------------------------------------------------------------------
void __fastcall TMDIChild::ProcessAuto()
{
    // если выполнили последнюю операцию
    if (nAutoFirst == nAutoLast)
     {
        StopExecuteAutoBlock();
        // снимем выделение блока
        ClearSelection();
        // если поступила заявка на остановку блока "Авто"
        if (bStopAutoRequest)
         { // сбросим ее, не нужна
           bStopAutoRequest = false;
         }
        // если блок "Авто" вып. по заявке из ПРИС и в блоке не было не-диполь операций
        if (bAutoBlockFromPris && !bExistsNonDipolOperation)
         { // окончим раздел
           SendMessage(ISRMainForm->Handle, MESSAGE_END_PARTITION, 0, 0);
         }
     }
    else
     { //перейдем к след. операции
       nAutoFirst++;
       // если поступила заявка на остановку блока "Авто"
       if (bStopAutoRequest)
        {
            bStopAutoRequest = false;
            StopExecuteAutoBlock();
        }
       else
        { // выполним очередную операцию
           ExecuteAutoBlockOperation();
        }
     }
 }

// остановить выполнение блока "Авто"
void __fastcall TMDIChild::StopExecuteAutoBlock()
{
    // флаг: выполняем блок "Авто"
    bExecuteAutoBlock = false;
    // восстановим заголовок дочернего окна
	Caption = WindowCaption;
    // если текущая операция - не диполь-операция и не ИСР-операция
    if (pOperationType[nCurrentOperation] != otDipolOperation && pOperationType[nCurrentOperation] != otISROperation)
     { // если мы не в процессе завершения всех разделов
        if (!bEndAllPartitions)
         {
            AttentionSoundSignal();
         }
     }
	SendStringToPrisProtokol( "Окончено выполнение блока 'Авто'", stStandart, true, true, false);
}
//------------------------------------------------------------------------------
// начать операцию и заблокировать перемещение
//------------------------------------------------------------------------------
void __fastcall TMDIChild::StartOperation()
{
    // установим флаг: ПРИС выполняет операцию
    bPrisExecutesOperation = true;
    // номер операции, которая обрабатывается ПРИСои
    nExecutesOperationNumber = nCurrentOperation;
}

// окончить операцию и разблокировать перемещение
void __fastcall TMDIChild::EndOperation()
{
    // сбросим флаг: ПРИС выполняет операцию
    bPrisExecutesOperation = false;
}
//-------------------------------------------------------------
void __fastcall TMDIChild::GoNextOperation(bool bRedrawGrid)
{
    // если мы не на последней операции
    if (nCurrentOperation < nOperationCount - 1)
     { // перейдем к след. операции
       nCurrentOperation++;
     }
    // перерисуем таблицу
    if (bRedrawGrid)
     {
        DrawGrid();
     }
}
//------------------------------------------------------------
// Начать/окончить отметку блока "Авто"
void __fastcall TMDIChild::BeginFinishBlock()
{
    int nLocalAutoFirst = nAutoFirst;
    int nLocalAutoLast = nAutoLast;
    bool bLocalBlockSelectionStart = bBlockSelectionStart;
    // если блок уже начат
    if (bBlockSelectionStart)
     { // если мы ниже последней операции
       if (nCurrentOperation > nAutoLast)
        { // конец - текущая операция
          nLocalAutoLast = nCurrentOperation;
        }
       else
        { // начало - текущая операция
           nLocalAutoFirst = nCurrentOperation;
        }
        bLocalBlockSelectionStart = false;
     }
	else
     {
        nLocalAutoFirst = nCurrentOperation;
        nLocalAutoLast = nCurrentOperation;
        bLocalBlockSelectionStart = true;
     }
    // если блок не корректен
	if (!TestCorrectAutoBlock( nLocalAutoFirst, nLocalAutoLast ) )
	 {
		WarningMessageBox( "В блок 'Авто' могут входить только операции типов: 'О' 'И'. Отметка блока невозможна" );
	 }
    else
	 {
        nAutoFirst = nLocalAutoFirst;
        nAutoLast = nLocalAutoLast;
        bBlockSelectionStart = bLocalBlockSelectionStart;
        // перерисуем таблицу
		DrawGrid();
     }
 }
//-----------------------------------------------------------------
// функция проверяет, корректен ли блок "Авто", т.е. входят ли в него
// только операции типа "О" и "И"
bool __fastcall TMDIChild::TestCorrectAutoBlock(int nAutoFirst, int nAutoLast)
{
    bool bResult = true;
    // цикл по всем операциям
    for (int i = nAutoFirst; i <= nAutoLast; i++)
     {
        if (pOperationType[i] != otDipolOperation && pOperationType[i] != otISROperation)
         {
			bResult = false;
            break;
         }
     }
    return (bResult);
}
//-------------------------------------------------------------
void __fastcall TMDIChild::ClearSelection(void)
{
    nAutoFirst = -1;
    bBlockSelectionStart = false;
    // перерисуем таблицу
    DrawGrid();
}
//--------------------------------------------------------------
// выполнить блок "Авто"
void __fastcall TMDIChild::ExecuteAutoBlock()
{
	// если мы не на последнем разделе
	if( nCurrentPartition < nPartitionCount - 1 )
     {
		WarningMessageBox( L"Блок 'Авто' не может быть выполнен, так как данный раздел не является текущим" );
        return;
     }
    // флаг: выполняем блок "Авто"
    bExecuteAutoBlock = true;
	SendStringToPrisProtokol( "Начато выполнение блока 'Авто'", stStandart, true, true, false);
	Caption = Caption + " - " + AutoMessage;
    // выполним операцию
    ExecuteAutoBlockOperation();
}
//---------------------------------------------------------------
void __fastcall TMDIChild::ExecuteAutoBlockOperation()
{
    nCurrentOperation = nAutoFirst;
    MakeCurrentOperation();
}
//----------------------------------------------------------------
void __fastcall TMDIChild::StopAutoBlock()
{
    // флаг: поступила заявка на остановку блока "Авто"
    bStopAutoRequest = true;
    bAutoBlockFromPris = false;
    ISRMainForm->ConfigureMenu();
}
//----------------------------------------------------------------
// функция выполняет в авт. режиме операции, начиная с текущей
// в секции 1 и 3 - до первой операции, которая не может входить
//     в блок "Авто" или до конца секции
// для секции 2 - если мы над блоками НШС, то до первого блока НШС
//     или до конца секции, если их нет,
//     если мы в блоке НШС - до конца блока
void __fastcall TMDIChild::WorkInAutoMode()
{
    int i, nFirstDipolOperation, nLastDipolOperation;
    // последняя строка блока НШС
    int nLastNShSString;
    // текущая строка (первая строка текущей операции)
    int nCurrentString = pnOperationStart[nCurrentOperation];
    int nCurrentBlock;
    bool bFind;
    // если секция НШС
    if (nSectionNumber == 1)
     { // если есть блоки НШС
       if (nNShSBlockCount > 0)
        { // если мы над первым блоком
          if (nCurrentString < aNShSBlock[0].nBlockStart)
           { // последняя строка в секции НШС - первая строка первого блока
              nLastNShSString = aNShSBlock[0].nBlockStart;
           }
          else
           {
             bFind = false;
             // цикл по блокам НШС
             for (nCurrentBlock = 0; nCurrentBlock < nNShSBlockCount; nCurrentBlock ++)
              { // если блок охватывает текущую строку
                if (nCurrentString > aNShSBlock[nCurrentBlock].nBlockStart && nCurrentString < aNShSBlock[nCurrentBlock].nBlockEnd)
                 {
                   bFind = true;
                   nLastNShSString = aNShSBlock[nCurrentBlock].nBlockEnd;
                 }
              }
             // если блок НШС не найден
             if (!bFind)
              {
                ErrorMessageBox("Поступила заявка на выполнение блока НШС в автоматическом режиме, однако сделать это невозможно, так как текущая операция не находится ни в одном из блоков", "Ошибка");
                return;
              }
           }
       }
      else // блоков НШС нет
       { // работаем во всей секции
         nLastNShSString = -1;
       }
   }
  bExistsNonDipolOperation = false;
  // цикл по операциям
  for (i = nCurrentOperation; i < nOperationCount; i++)
   { // если не диполь-операция
     if (pOperationType[i] != otDipolOperation)
      {
        bExistsNonDipolOperation = true;
        break;
      }
     // если секция НШС и есть нижняя строка НШС мы вышли из блока
     if (nSectionNumber == 1 && nLastNShSString != -1 && pnOperationStart[i] > nLastNShSString)
      {
        break;
      }
   }
 // если диполь-операции есть
 if (i > nCurrentOperation)
  {
    nFirstDipolOperation = nCurrentOperation;
    nLastDipolOperation = i - 1;
    // блок "Авто"
    nAutoFirst = nFirstDipolOperation;
    nAutoLast = nLastDipolOperation;
    bAutoBlockFromPris = true;
    // выполним его
    ExecuteAutoBlock();
  }
}
//---------------------------------------------------------------
// установить курсор на блок НШС
// в случае удачи возвращает 0
// -1 - если секция не НШС
// -2 - не найдена метка НШС
// -3 - в блоке НШС нет операций
int __fastcall TMDIChild::SetNshSBlock(int nBlockNumber)
{
    int nBlockCounter;
    int nOperationCounter;
    int nBlockStart, nBlockEnd;
    // если секция не НШС
    if (nSectionNumber != 1)
     {
       return (-1);
     }
    bool bFindBlock = false;
    bool bFindOperation = false;
    // цикл по блокам НШС
    for (nBlockCounter = 0; nBlockCounter < nNShSBlockCount; nBlockCounter++)
     {
        if (aNShSBlock[nBlockCounter].nBlockNumber == nBlockNumber)
         {
            bFindBlock = true;
            break;
         }
     }
    // если блок НШС не найден
    if (!bFindBlock)
     {
       return (-2);
     }
    nBlockStart = aNShSBlock[nBlockCounter].nBlockStart;
    nBlockEnd = aNShSBlock[nBlockCounter].nBlockEnd;
    // определим номер операции, с которой начинается блок НШС
    for (nOperationCounter = 0; nOperationCounter < nOperationCount; nOperationCounter++)
     {
       if (pnOperationStart[nOperationCounter] > nBlockStart && pnOperationEnd[nOperationCounter] < nBlockEnd)
        {
          bFindOperation = true;
          break;
        }
     }
    // если операция не найдена
    if (!bFindOperation)
     {
       return (-3);
     }
    // утановим нужную операцию
    nCurrentOperation = nOperationCounter;
    DrawGrid();
    return (0);
}
//-----------------------------------------------------------------
// обработчик сообщения "Конец вызванного раздела"
void __fastcall TMDIChild::OnEndCalledPartition(TMessage& Message)
{
    EndCalledPartition();
}
//---------------------------------------------------------------
void __fastcall TMDIChild::EndCalledPartition()
{
    // если выполняем блок "Авто"
    if (bExecuteAutoBlock)
     { // если мы в процессе завершения всех разделов
       if (bEndAllPartitions)
        {
          StopExecuteAutoBlock();
        }
       else
        {
          ProcessAuto();
        }
     }
    DrawGrid();
}
//-------------------------------------------------------------
AnsiString TMDIChild::NShSBlockDescription(int nOperationNumber)
{
    int nBlockNumber = NShSBlockNumber(nOperationNumber);
    AnsiString Comment;
    // если секция НШС
    switch (nBlockNumber)
     {
		case (-3):
			Comment += "Данный блок не найден";
            break;
        case (-1):
			Comment += "вне блоков НШС";
			break;
        case (-2):
			Comment += "сбой в " + AppTitle + ": не секция НШС";
            break;
        default:
			Comment += "Данный блок не найден " + IntToStr(nBlockNumber);
     }
    return (Comment);
}
//-------------------------------------------------------------------
// функция воззвращает номер блока НШС, в котором располагается операция nOperationNumber
// если мы над блоком, возвращается -1
// если секция не НШС, возвращается -2
// если в блоках эта операция не найдена, возвращается -3
int TMDIChild::NShSBlockNumber(int nOperationNumber)
{
    // если секция НШС
    if (nSectionNumber != 1 )
     {
       return (-2);
     }
    int nCurrentString = pnOperationStart[nOperationNumber];
    bool bFind = false;
    int nBlockNumber;
    int nCurrentBlock;
    // если блоков нет
    if (nNShSBlockCount == 0)
     { // операция вне блока
       nBlockNumber = -1;
     }
    // блоки есть
    else
     { // если мы над первым блоком
       if (nCurrentString < aNShSBlock[0].nBlockStart)
        { // операция вне блока
          nBlockNumber = -1;
          bFind = true;
        }
       // мы в блоке
       else
        { // цикл по блокам НШС
          for (nCurrentBlock = 0; nCurrentBlock < nNShSBlockCount; nCurrentBlock ++)
           { // если блок охватывает текущую строку
             if (nCurrentString > aNShSBlock[nCurrentBlock].nBlockStart && nCurrentString < aNShSBlock[nCurrentBlock].nBlockEnd)
              {
                bFind = true;
                break;
              }
           }
           // если блок найден
           if (bFind)
            {
              nBlockNumber = aNShSBlock[nCurrentBlock].nBlockNumber;
            }
         }
     }
    // если блок не найден (это ошибка)
    if (bFind)
     {
        return (nBlockNumber);
     }
    else
     {
        return (-3);
     }
 }


