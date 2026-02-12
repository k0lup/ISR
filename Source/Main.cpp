//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
//#include <Vcl.OleCtrls.hpp>
#pragma hdrstop
#include <algorithm>
#include <Filectrl.hpp>
#include "Main.h"
#include "Variant.h"
#include "Parnas.h"
#include "pfks_p.h"
#include "Title.h"
#include "List.h"
#include "Stencil.h"
#include "End.h"
#include "SP.h"
#include "Request.h"
#include "CheckSPO.h"
#include "about.h"
#include "Find.h"
#include "Setting.h"
#include "Completing.h"
#include "PartisionRep1.h"
#include "SelectionRep1.h"
#include "Common.h"
//---------------------------------------------------------------------------
#pragma link "Word_2K_SRVR"
#pragma link "Word_XP_srvr"
//#pragma link "WordXP"
#pragma resource "*.dfm"
TISRMainForm *ISRMainForm;
extern AnsiString ProtokolName;
extern AnsiString ParameterNShSArticle_REP;
extern int size;
extern AnsiString ParameterDamageArticle_REP;
extern AnsiString  ParameterGoInitialArticle_REP;
short ISR_prot;
//---------------------------------------------------------------------------
__fastcall TISRMainForm::TISRMainForm( TComponent *Owner )
	: TForm(Owner)
{
  nR = 0;                      // тип последней операции
  BR = 0;                      // пр.завершения оп-и НАЧАЛО РАЗДЕЛА
  ISR_prot = 0;                // признак ожидания ответа на СП
  TM = new TTimer(this);
  TM->OnTimer = On_TM;
  TM->Interval = 0;
  TM->Enabled = false;
  prer = 0;
  KS = 0;
  //
  net_PRIS = new NET_CL_PRIS( ISRMainForm, 0x4020 );
  net_PROT = new NET_CL_PRIS( ISRMainForm, 0x4024 );
  net_PRIS->Open();
  net_PROT->Open();
  Visible = true;
  SetFocus();
}
//-----------------------------------------------
void __fastcall TISRMainForm::On_TM( TObject *Sender )
{
  if( BR == 0 )
      return;
  BR = -1;
  TM->Enabled = false;
  TM->Interval = 0;
  ErrorMessageBox( L"Нет ответа от ПРИС на операцию НАЧАЛО РАЗДЕЛА" );
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::FormActivate( TObject *Sender )
{
  static int nCount = 0;
  // если первый вызов
	if( nCount++ == 0 )
	 {
		Prepare();
	 }
}
//---------------------------------------------------------------------------
 void __fastcall TISRMainForm::FormDestroy(TObject *Sender)
{  
  // если в системе есть разделы
  try {
		DeletePartitionsMemory();
	  }
  catch(...)
	 {
		WriteToDebugFile( "ИСР: Ошибка при удалении разделов по КС" );
	 }
  //  если были разделы, загружаемые только по НАЧРАЗД
  if( HideAbbrList->Count )
	for( int i = 0; i < HideAbbrList->Count; i++ )
          delete[] HideAbbrList->Items[i];
  delete HideAbbrList;
  // если есть спецсредства
  if( SpecialFacilitiesPartitionCount > 0 )
    {
	  SpecialFacilitiesPartitionCount = 0;
	  try {
			delete[] SpecialFacilitiesPartitions;
		  }
	  catch(...)
		  {
			WriteToDebugFile( "ИСР: Ошибка при удалении спецсредств по КС" );
		  }
    }
  //	delete Por;
  net_PRIS->Active = false;
  net_PROT->Active = false;
  delete net_PRIS;
  delete net_PROT;
  if( TM )
   {
	 TM->Interval = 0;
	 TM->Enabled = false;
	 delete TM;
   }
  WriteToDebugFile( "Конец работы ИСР" );
  Sleep(1000);
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::CreateMDIChild( String Name )
{
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::OnSysCommand( TMessage& Message )
{
	DefaultHandler( &Message );
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::FormCloseQuery( TObject *Sender, bool &CanClose )
{
    // если не пришла порция на закрытие и не мое рабочее место
    if (!bRecievePortionForClose && !bDeveloperWorkPlace)
	{
		ShowMessage( AppTitle + " не может быть закрыт. Если Вам он сейчас не нужен, Вы можете свернуть его на панель задач" );
		CanClose = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::ProcessInPortion( char *Portion, int l )
{
   int Rod, Sort, AtomType;
   char *Atom;
   bool bProcess = false;                                       // флаг: порция обработана
   AnsiString DebugString = "";
   //
   if( prer )                                                   // прервали операцию
	 {
		prer = 0;
		bPrisProcessEndPartition = 0;
		goto OO;
	 }
   Rod = ((POR*)Portion)->KindofWork;
   Sort = ((POR*)Portion)->SortofWork;
   AtomType = ((POR*)Portion)->AtomType;
   Atom =  Portion + sizeof(POR);
   DebugString = "Получена порция. Род: " + IntToStr(Rod) + ", сорт: " + IntToStr(Sort) + ", тип атома: " + IntToStr(AtomType);
   WriteToDebugFile( DebugString );
   // если ответ на КО
   if( Rod == 1 && Sort == 2 )
	{
	  if( ISR_prot )
			 ISR_prot = 0;                                      // ожидали ответ на СП
	  if( TitleBox->Visible )
				  TitleBox->BringToFront();
	  if( ( AtomType != 0 ) && ( nR == 4 || nR == 3 ) )         // не прошла ком-да загр.раздел или ЗАГОЛОВ
		{
		   TM->Interval = 0;
		   TM->Enabled = false;
		   EndCurrentPartition(0);                              // заканчиваем
		   ShowMessage( "ИСР не может выполнить операцию. ПРИС занят" );
		   nR = 0;
		   return;
		}
		//
	  if( totalOperationsInPris != 0 )
			AnswerForKO( AtomType );                            // была операция в ПРИС из радела
	  else
	   if( ( AtomType != 0 ) && ( nR == 5 || bSaveProtokol ) )  // завершить раздел
		 {
		   if( bSaveProtokol )
			 {
			   ErrorMessageBox("Не прошло сохранение протокола ");
			   bSaveProtokol = 0;
			   BR = 0;
			   bProcess = true;
			   return;
			 }
		   goto MM;                                             // пришла ошибка от ПРИС
		 }
		bProcess = true;
	}
   // если сигнал на завершение работы
   if( Rod == 1 && Sort == 3 && AtomType == 0 )
	  {
		bRecievePortionForClose = true;
		Close();
		bProcess = true;
		nR = 0;
		KS = 1;
		return;
	  }
	// если директива "Раздел"
	if( Rod == 1 && Sort == 5 && AtomType == 0 )
	  {
		PreProcessPartitionOperation( AnsiString(Atom) );
		bProcess = true;
	  }
	// если ответ на "конец раздела"
	if( Rod == 1 && Sort == 6 )
	 {
MM:     bPrisProcessEndPartition = false;                       // сбросим флаг
		if( AtomType == 0 )                                     // если раздел можно завершать
			   EndCurrentPartition(0);
		else
			   RefusalEndPartition();
		bProcess = true;
	 }
	// если начало работы
	if( Rod == 1 && Sort == 7 && AtomType == 0 )
	 {
		PRISLoaded();
		bProcess = true;

	 }
	// если начало раздела
	if( Rod == 1 && Sort == 4 && AtomType == 0 )
	 {
	   TM->Interval = 0;
	   TM->Enabled = false;
	   if( BR == 10 )
		{
		  BR = 0;                                             // уст.признак норм. загрузки раздела
		  PartitionOperation( "pppp", "", false, false, false );
		}
	   else                                                   // до отв. на ЗАГОЛОВ гасим меню ПОДГОТОВКА
		{
		  BR = 7;
		  if( !pr_vosst )                                     // если не восстанавливаемся
			SendWindowTitleToPris();                          // пошлем заголовок окна в ПРИС
		}
	   bProcess = true;
	   PrepareHeadline->Enabled = true;
	 }
	// если ЗАГОЛОВ
	if( Rod == 1 && Sort == 10 )
	 {
	   if( BR == 11 )                               // был конец раздела или всех разделов
		{
			EndCurrentPartition(1);                  // идем на продолжение
			BR = 0;
			bProcess = true;
			return;
		}
	   if( AtomType == 1 )
		{
		  SendStringToPrisProtokol( "Не прошла команда ЗАГОЛОВ! ЗАВЕРШИТЕ РАЗДЕЛ И ПОВТОРИТЕ ОПЕРАЦИЮ", stError, true, true, false);
		  ErrorMessageBox( "Не прошла команда ЗАГОЛОВ\\nЗАВЕРШИТЕ РАЗДЕЛ И ПОВТОРИТЕ ОПЕРАЦИЮ" );
		  BR = 0;
		  bProcess = true;
		  return;
		}
	   else
		{
		   Title( "pppp" );                         //  спец.строкa для возврата в ф-ю Title(str)
		   // пишем заголовок в протокол ПРИС
		   AnsiString ps = GetProtokolName();
		   if( ps.Length() > 33 )
			{
			  SendStringToPrisProtokol( "Не прошла команда ЗАГОЛОВ: Превышена допустимая длина ф. протокола", stError, true, true, false);
			  ErrorMessageBox( "Не прошла команда ЗАГОЛОВ. Превышена допустимая длина имени ф.протокола. ЗАВЕРШИТЕ РАЗДЕЛ И ПОВТОРИТЕ ОПЕРАЦИЮ" );
			  BR = 0;
			  bProcess = true;
			  return;
			}
		   SendStringToPrisProtokol( "Заголовок раздела: " + TitleBox->TitleEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя протокола: " + ps,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя РЭП: " + TitleBox->RAPEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя оператора ПУ АИС: " + TitleBox->OperatorPUAISEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя оператора АИС: " + TitleBox->OperatorAISEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя оператора ТКЦК: " + TitleBox->OperatorTKCKEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя оператора НТК СИ: " + TitleBox->OperatorNTKSIEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Имя оператора БС: " + TitleBox->OperatorBSEdit->Text,
										st_Blue, false, false, true);
		   SendStringToPrisProtokol( "Конец операции 'Заголовок раздела'", stStandart, true, true, false );
		 }

		BR = 0;                                     // сбросим для входа в меню ЗАГР.СТРУКТУРЫ
		bProcess = true;
	 }
    // если текущий каталог
	if( Rod == 2 && Sort == 1 && AtomType == 6 )
     {
        bProcess = true;
		Title(Atom);
	 }
    // если директива "ЭД"
	if (Rod == 1 && Sort == 8 && AtomType == 0 )
     {
	   bProcess = true;
	   ProcessEDDir( Atom );
	 }
	// если директива "НА_СР"
	if( Rod == 1 && Sort == 9 && AtomType == 0 )
	 {
        bProcess = true;
		ProcessNASRDir( Atom );
	 }
	// если порция не обработана
	if( !bProcess )
     {
		AnsiString ErrorMessageToScreen = "Пришла некорректная порция. Инфомация о ней сохранена в файле \"" + ErrFileName + "\".";
		AnsiString ErrorMessageToFile = "Пришла некорректная порция. Род: " + IntToStr(Rod) + ", сорт: "
										+ IntToStr(Sort) + ", тип атома: " + IntToStr(AtomType)
										+ ", тело атома: \"" + Atom + "\".";
		ErrorMessageBox( ErrorMessageToScreen );
        WriteToErrorFile(ErrorMessageToFile);
	 }
OO:
	ConfigureMenu();
	nR = 0;
    // закончим восстановление
	if( pr_vosst )
		 pr_vosst = false;
}
//------------------------------------------------------------------------------
// получен ответ на КО
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::AnswerForKO(int AtomType)
{
	if( totalOperationsInPris == 0 )
		return;
	if( !externalOperation[--totalOperationsInPris] )
      {
		AnsiString StringToProtokol = "ИСР: Конец директивы";
//        SendStringToPrisProtokol(StringToProtokol, stStandart, false, true, false);
        return;
      }
    // переключимся к активной секции
    GoToActiveSection();
    int nPartitionNumber = aOperationInPris[nOperationInPrisCount - 1].nPartitionNumber;
    int nSectionNumber = aOperationInPris[nOperationInPrisCount - 1].nSectionNumber;
    TMDIChild* ch = LoadedPartitions[nPartitionNumber].pWindow[nSectionNumber];
	// передадим окну
	ch->AnswerForKO( AtomType );
}
//---------------------------------------------------------------------------
// функция загружает структуру текущего раздела в ИСР
//---------------------------------------------------------------------------
// nTopSection - номер секции, которая должна лечь сверху
bool TISRMainForm::LoadStructure(bool bFromPartitionOperation, AnsiString& StrToProtokol, AnsiString& ErrorMessage, int nTopSection)
{
	int i, j;
	// длина строки, считанной из файла
	int nStringLength;
    // дескриптор файла структуры
    int nStructureFileHandler;
	// метка начала и конца раздела
	AnsiString StructureBeginEndLabel;
	// номера строк начала и конца секции
	int anFirstString[3], anLastString[3];
	// кол-во строк в секции
	int anStringCount[3];
	// флаг, показывающий, что файл читается нормально
	bool bGoodRead;
	// флаг, показывающий, что мы при проходе по секции
	// мы внутри секции
	bool bInSection;
	// флаг, показывающий, что мы при проходе по секции
	// мы внутри заголовка секции
	bool bInSectionCaption;
	// флаг, что мы на первой строке заголовка секции
	bool bOnFirstStringOfSectionCaption;
	// номер текущей строки
	int nStringNumber;
	// переменная для прохода по секциям
	int nCurSection;
	// массив заголовков секций
	AnsiString aSectionCaption[3];
	// кол-во разделителей в строке
	int nDelimiterCount;
	// позиция первогоразделителя
	int nFirstDelimiterPos;
	// текущая часть заголовка секции
	AnsiString PartOfSectionCaption;
	// флаг: встретилась первая строка формата 4
	bool bFirstFormat4String;
	// массив указателей строк структуры
	StructureString* apSectionStrings[3];
	// кол-во строк в файле
	int nTotalStringCount;
    // буфер, в который читается строка
    char acReadString[MaxStringLength + 1];
    // AnsiString версия буфера
    AnsiString ReadString;
    AnsiString LocalErrorMessage;
    int nErrorCode;
	AnsiString ProcessTitle = "Загрузка файла структуры";
    bool bGood = true;
    bool bOutLocalErrorMessage = false;
    bool bFileOpened = false;
    bool bMemoryAllocated = false;
	char pcKS[9];

	// имя структуры
    AnsiString StructureFileName = GetStructureFileName(nCurrentPartition);

    if (!FileExists(StructureFileName))
     {
		StrToProtokol = "Не найден на диске файл структуры раздела \"" + StructureFileName + "\"";
        ErrorMessage = StrToProtokol;
		LocalErrorMessage = "Не найден на диске файл структуры раздела \"" + StructureFileName + "\".";
        bOutLocalErrorMessage = true;
		// открытие документа - неудачно
		bGood = false;
     }
    // если имеет смысл продолжать
    if (bGood)
     {
        int nChoice;
        // подсчет КС
        int nCount = KSCount(StructureFileName.c_str(), 3, pcKS);
        // если КС не совпадает
        if (nCount == 0)
         {
			AnsiString Message = "Ошибка в контрольной сумме в файле структуры \"" + StructureFileName + "\". Загрузить структуру?";
			nChoice = QuestionMessageBox( Message, ProcessTitle, MB_YESNO + MB_ICONWARNING );
            if (nChoice == IDNO)
             {
				StrToProtokol = "у файла структуры раздела не верна контрольная сумма. Оператор отказался от раздела";
				ErrorMessage = "у файла структуры раздела не верна контрольная сумма. Оператор отказался от раздела";
				bGood = false;
			 }
         }
	 }

    // если имеет смысл продолжать
 if (bGood)
  { // откроем файл структуры
    nStructureFileHandler = FileOpen(StructureFileName, fmOpenRead + fmShareDenyWrite);
    // если неудачно
    if (nStructureFileHandler == -1)
     {
	   StrToProtokol = "ошибка при попытке открытия файла структуры раздела \"" + StructureFileName + "\"";
       ErrorMessage = StrToProtokol;
	   LocalErrorMessage = "ошибка при попытке открытия файла структуры раздела \"" + StructureFileName + "\".";
       bOutLocalErrorMessage = true;
       // открытие документа - неудачно
       bGood = false;
	 }
    // файл открыт
    else {
            bFileOpened = true;
         }
  }

    // если имеет смысл продолжать
 if (bGood)
  { // первая секция - штатная работа
    nCurSection = 0;
    // номер текущей строки
    nStringNumber = 0;
    // цикл по трем секциям
     while (1)
      { // определим метку начала и конца секции
        switch(nCurSection)
         { // ШТАТ
           case (0): StructureBeginEndLabel = AnsiString(cSectionBeginEndSymbol) + "1";
                     break;
           // НШС
           case (1): StructureBeginEndLabel = AnsiString(cSectionBeginEndSymbol) + "2";
                     break;
           // ПРИ
           case (2): StructureBeginEndLabel = AnsiString(cSectionBeginEndSymbol) + "3";
         }

        // внутри секции
        bInSection = false;
        // встретилась первая строка формата 4
        bFirstFormat4String = true;
        // цикл по чтению строк
        while (1)
            { // считаем строку
              nStringLength = ReadStringFromFile(nStructureFileHandler, acReadString, nErrorCode);
              // если ошибка при чтении
              if (nStringLength < 0)
                { // сообщение о неудаче
				  LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName +
									  "\" в строке " + IntToStr(nStringNumber + 1)
									  + " (" + acFileReadErrorMessage[nErrorCode] + ").";
				  StrToProtokol = "Нарушена структура файла \"" + StructureFileName + "\" в строке "
								   + IntToStr(nStringNumber + 1) + " ("
								   + acFileReadErrorMessage[nErrorCode] + ")";
                  ErrorMessage = StrToProtokol;
                  // чтение файла - неудачно
				  bGood = false;
                  // выйдем из цикла
                  break;
                }
              // создадим AnsiString-версию строки, считанной из файла
              ReadString = acReadString;
              // если метка начала или конца секции
              if(ReadString.SubString(1, 2) == StructureBeginEndLabel)
                {  // если мы еще не внутри секции
                   if (!bInSection)
                    { // установим флаг, что мы внутри секции
                      bInSection = true;
                      // установим флаг, что мы внутри заголовка секции
                      bInSectionCaption = true;
                      // установим флаг, что мы на первой строке заголовка секции
                      bOnFirstStringOfSectionCaption = true;
                    }
                    // мы внутри секции
                   else
                    { // запомним конец содержания секции
                      anLastString[nCurSection] = nStringNumber - 1;
                      // установим флаг, что мы не внутри секции
                      bInSection = false;
                      // учтем текущую строку
                      nStringNumber++;
                      // выйдем из чтения
                      break;
                    }
                }
                // если мы внутри секции
                if (bInSection)
                {  // подсчитаем кол-во разделителей
                   nDelimiterCount = 0;
                   for (i = 0; i < nStringLength; i++)
                    {
                      if (ReadString[i + 1] == cFieldDelimiter)
                         {
                            nDelimiterCount++;
                         }
                    }
                    // перебор по кол-ву разделителей
                    switch (nDelimiterCount)
                    {
                      case (2):
                            // если мы внутри заголовка секции
                            if (bInSectionCaption)
                            { // найдем первый разделитель
                              nFirstDelimiterPos = ReadString.Pos(cFieldDelimiter);
                              // если разделитель не там
                              if ((bOnFirstStringOfSectionCaption && nFirstDelimiterPos != 3) || (!bOnFirstStringOfSectionCaption && nFirstDelimiterPos != 1))
								{
								  // сообщение о неудаче
								  LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName
													  + "\" в строке " + IntToStr(nStringNumber + 1);
								  StrToProtokol = "Нарушена структура файла \"" + StructureFileName
												  + "\" в строке " + IntToStr(nStringNumber + 1);
                                  ErrorMessage = StrToProtokol;
                                  // чтение файла - неудачно
                                  bGood = false;
                                  // выйдем из switch
                                  break;
								}
                              // если второй разделитель не там
                              if (ReadString[nStringLength] != cFieldDelimiter)
								{
								  // сообщение о неудаче
								  LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName
													  + "\" в строке " + IntToStr(nStringNumber + 1);
								  StrToProtokol = "Нарушена структура файла \"" + StructureFileName
												  + "\" в строке " + IntToStr(nStringNumber + 1);
                                  ErrorMessage = StrToProtokol;
                                  // чтение файла - неудачно
                                  bGood = false;
                                  // выйдем из switch
                                  break;
                                }
							  // вырежем часть заголовка секции
                              PartOfSectionCaption = ReadString.SubString(nFirstDelimiterPos + 1, nStringLength - nFirstDelimiterPos - 1);
                              // вырежем пробелы
                              PartOfSectionCaption = PartOfSectionCaption.Trim();
                              // формируем заголовок секции
                              aSectionCaption[nCurSection] += (PartOfSectionCaption + " ");
                              // ушли с первой строки заголовка
                              bOnFirstStringOfSectionCaption = false;
                            }
                            // строка формата 5 встретилась вне заголовка секции
							else
							 {
                                // сообщение о неудаче
								LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName
													+ "\" в строке " + IntToStr(nStringNumber + 1)
													+ ". Строка формата 5 находися на недопустимой позиции.";
                                StrToProtokol = LocalErrorMessage;
                                ErrorMessage = StrToProtokol;
								// чтение файла - неудачно
                                bGood = false;
								bOutLocalErrorMessage = 1;
                                // выйдем из switch
                                break;
							 }
                            break;
                        case (4):
                            // если встретили 1-ю строку формата 4
							if (bFirstFormat4String)
							 {
                                // из заголовка вышли
                                bInSectionCaption = false;
                                bFirstFormat4String = false;
                                // запомним начало секции
                                anFirstString[nCurSection] = nStringNumber;
                            }
                            break;
                        default:
							// сообщение о неудаче
							LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName
												+ "\" в строке " + IntToStr(nStringNumber + 1);
							StrToProtokol = "Нарушена структура файла \"" + StructureFileName
											+ "\" в строке " + IntToStr(nStringNumber + 1);
                            ErrorMessage = StrToProtokol;
                            // чтение файла - неудачно
                            bGood = false;

                    }
                }
			  // если при чтении файла возникли ошибки
			  if (!bGood)
				 {
                    // выйдем из цикла по строкам
                    break;
				 }
			  nStringNumber++;
			}
            // если при чтении файла возникли ошибки
            if (!bGood)
            {
                // выйдем из цикла по секциям
                break;
            }
            // количество строк секции
            if (bFirstFormat4String)
            {
                anStringCount[nCurSection] = 0;
                anFirstString[nCurSection] = -1;
                anLastString[nCurSection] = -1;
            }
            else
            {
                anStringCount[nCurSection] = anLastString[nCurSection] - anFirstString[nCurSection] + 1;
            }
            // если мы прошли секцию 3
            if (nCurSection == 2)
            {
                // окончим проход по секциям
                break;
            }
            // перейдем к след. секции
            nCurSection++;
        }
    }

    // если при чтении файла не возникли ошибки
    if (bGood)
    {
        // попытка ваделить память
        try
        {
            // создадим 3 массива строк
            for (i = 0; i < 3; i++)
            {
                if (anStringCount[i] > 0)
                    apSectionStrings[i] = new StructureString[anStringCount[i]];
            }
            bMemoryAllocated = true;
        }
		// попытка запроса памяти неудачна
		catch (std::bad_alloc&)
		{
			ErrorMessageBox( "Невозможно открыть секцию файла структуры из-за нехватки памяти", ProcessTitle );
            // чтение файла - неудачно
            bGood = false;
            // очистим уже выделенную память
            for (j = 0; j < i; j++)
			{
                delete[] apSectionStrings[j];
            }
            bMemoryAllocated = false;
        }
	}

    // если при чтении файла не возникли ошибки
    if (bGood)
    {
        // номер символа, с которого начинается поле в строке
        int nFieldStart;
        // номер текущего поля
        int nCurrentField;
        // номер текущей строки в документе
        int nDocumentStringNumber;
        // мы в теле секции
        bool bInSectionBody;
        AnsiString Field;

        // запомним кол-во строк в файле
        nTotalStringCount = nStringNumber;
        // номер текущей строки
        nStringNumber = 0;

        // перейдем к началу файла
        FileSeek(nStructureFileHandler, 0, 0);
        // цикл по чтению строк
        while (1)
        { // считаем строку
          nStringLength = ReadStringFromFile(nStructureFileHandler, acReadString, nErrorCode);
          // если ошибка при чтении
          if (nStringLength < 0)
		   {
			  // сообщение о неудаче
			   LocalErrorMessage = "Нарушена структура файла \"" + StructureFileName
								   + "\" в строке " + IntToStr(nStringNumber + 1)
								   + " (" + acFileReadErrorMessage[nErrorCode] + ").";
			   StrToProtokol = LocalErrorMessage;
               ErrorMessage = StrToProtokol;
               // чтение файла - неудачно
               bGood = false;
               // выйдем из цикла
               break;
            }
            // пока считаем, что мы внутри какой-либо секции
			bInSectionBody = true;
            // если мы внутри тела секции ШТАТ
            if (nStringNumber >= anFirstString[0] && nStringNumber <= anLastString[0])
            {
                nCurSection = 0;
            }
            else
            { // если мы внутри тела секции НШС
                if (nStringNumber >= anFirstString[1] && nStringNumber <= anLastString[1])
                {
                    nCurSection = 1;
                }
                else
                {  // если мы внутри тела секции ПРИ
                   if (nStringNumber >= anFirstString[2] && nStringNumber <= anLastString[2])
                   {
                        nCurSection = 2;
                   }
                    // мы не в теле секции
                    else
                    {
                        bInSectionBody = false;
                    }
                }
            }
            // если мы в теле секции
            if (bInSectionBody)
            {  // создадим AnsiString-версию строки, считанной из файла
                ReadString = acReadString;
                // номер строки в документе
                nDocumentStringNumber = nStringNumber - anFirstString[nCurSection];
                // первое поле начинается с нулевого символа
                nFieldStart = 1;
                // начнем с первого поля
                nCurrentField = 0;
                // цикл по символам строки
                for(i = 1; i <= ReadString.Length(); i++)
                { // если разделитель полей
                    if (ReadString[i] == cFieldDelimiter)
                    {  // вырежем поле
                        Field = ReadString.SubString(nFieldStart, i - nFieldStart);
                        // запишем поле в документ
                        // Если пусто в поле операции после номера
                        if((nCurrentField==1)&&Field.IsEmpty()&&
                           !apSectionStrings[nCurSection][nDocumentStringNumber].Field[nCurrentField-1].IsEmpty()
                           )
                          { // Ошибка - нет кода операции
                            Field = apSectionStrings[nCurSection][nDocumentStringNumber].Field[nCurrentField-1];
                            ErrorMessageBox("Ошибка в операции " +
                               Field + " файла структуры раздела. Нет кода операции");
                            Field = " ";
                            ErrorMessage = ": ошибка в файле структуры";
                            bGood = false;
                            break;
                          }
                        apSectionStrings[nCurSection][nDocumentStringNumber].Field[nCurrentField] = Field;
                        //else
                        //   if(nDocumentStringNumber)
                        //      apSectionStrings[nCurSection][nDocumentStringNumber].Field[nCurrentField] = apSectionStrings[nCurSection][nDocumentStringNumber-1].Field[nCurrentField];
                        // начало следующего поля
                        nFieldStart = i + 1;
                        // номер поля
                        nCurrentField++;
                    }
                }
            }
            // если проанализировали все строки
            if (nStringNumber == nTotalStringCount)
             { // окончим цикл
               break;
             }
            nStringNumber++;

        }
    }

    // если при чтении файла не возникли ошибки
    if (bGood)
     {
        TMDIChild *Child;
        // создадим дочерние окна
        for (i = 2; i >= 0; i--)
        {
          if (i != nTopSection)
           {
                Child = new TMDIChild(Application, i, apSectionStrings[i], anStringCount[i], aSectionCaption[i], StructureFileName, nCurrentPartition);
                LoadedPartitions[nCurrentPartition].pWindow[i] = Child;
           }
        }
        Child = new TMDIChild(Application, nTopSection, apSectionStrings[nTopSection], anStringCount[nTopSection], aSectionCaption[nTopSection], StructureFileName, nCurrentPartition);
        LoadedPartitions[nCurrentPartition].pWindow[nTopSection] = Child;
    }
    else
    {
        if (bMemoryAllocated)
        { // очистим выделенную память
            for (i = 0; i < 3; i++)
            {
                if (anStringCount[i] > 0)
                    delete[] apSectionStrings[i];
            }
        }
        // если обрабатывается не операция загрузки раздела,
        // а непосредственная загрузка раздела
        if (!bFromPartitionOperation)
        { // если выводить сообщение об ошибке
            if (bOutLocalErrorMessage)
            { // выведем
              ErrorMessageBox(LocalErrorMessage, ProcessTitle);
            }
        }
	 }
    // если файл открыт
    if (bFileOpened)
     {
        FileClose(nStructureFileHandler);
     }

    if (bGood)
	 {
        AnsiString MessageToProt;
		MessageToProt = "Загружена структура: \"" + StructureFileName + "\", " + "КС = " + pcKS;
		SendStringToPrisProtokol( MessageToProt, stStandart, true, true, false );
	 }
    // вернем результат
    return (bGood);
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::OnWindowPosChanging(TMessage& Message)
{
    DefaultHandler(&Message);
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareLoadStructureClick( TObject *Sender )
{
	if( BR == 7 )
	 {
	   ShowMessage( "ЖДЕМ ОТВЕТ НА ОПЕРАЦИЮ ЗАГОЛОВ" );
	   return;
	 }
    // аббревиатура раздела
    AnsiString PartitionAbbr = LoadedPartitions[0].Abbr;
    AnsiString Temp1, Temp2;
    if (LoadStructure(false, Temp1, Temp2))
     { // флаг: структура головного раздела загружена
        bMainPartitionStructureLoaded = true;
        anCurrentSection[0] = 0;
        ConfigureMenu();
     }
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::SectionsAnyMenuItemClick(TObject *Sender)
{
    // пункт меню, вызвавший событие
    TMenuItem* SenderItem = (TMenuItem*)Sender;
    // номер выбранной секции
    int nSectionNumber;
    // определим номер выбранной секции
    if (SenderItem == SectionsNormal)
     {
        nSectionNumber = 0;
     }
    else {
           if (SenderItem == SectionsNShS)
             {
               nSectionNumber = 1;
             }
           else {
                  nSectionNumber = 2;
                }
         }
    // установим активную секцию
    SetTopSection(nSectionNumber);
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::OperationsEndPartitionClick(TObject *Sender)
{
    int i;
    // цикл по всем child-окнам
	for (i = 0; i < MDIChildCount; i++)
        {
          MDIChildren[i]->Close();
        }
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::WorkUndoChosenVariantClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->UndoChosenVariant();
}
//---------------------------------------------------------------------------
// обработчик пунктов меню "ЭД"
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::ArticleAnyItemClick(TObject *Sender)
{
    // пункт меню, вызвавший событие
    TMenuItem* SenderItem = (TMenuItem*)Sender;
    // тип ЭД
    EDType CurrentType;
 if (SenderItem == ArticleNShS)
        CurrentType = etArticleNShS;  // НШС изд.
 else
   if (SenderItem == ArticleDamage)
         CurrentType = etArticleDamage;  // АВАР изд.
   else
    if (SenderItem == ArticleGoInitial)
           CurrentType = etArticleGoInitial;  // ПРИ изд.
    else
      if (SenderItem == ArticleNShS_REP)
           CurrentType = etArticleNShS_REP; // НШС РЭП
      else
        if (SenderItem == ArticleGoInitial_REP)
              CurrentType = etArticleGoInitial_REP;  // ПРИ РЭП
        else
              CurrentType =  etArticleDamage_REP;  // АВАР РЭП

    ShowED(CurrentType, "", false);
}
//---------------------------------------------------------------------------
// показать файл ЭД
// результат:
// 0 - файл показан
// -1 - файл не найден на диске
// -2 - файл не додключен к ИСР
// -3 - метка не найдена
//#include "PartisionRep1.h"
int __fastcall TISRMainForm::ShowED(EDType CurrentType, AnsiString Label, bool bFromPris)
{
    // имя файла ЭД
	/*Ansi*/String EDFileName;
	/*Ansi*/String FileName, ss;
    int nResult = 0;
    char pr_REP = 0;  // признак загрузки разделов РЭП
// определим номер выбранного раздела ЭД
   switch(CurrentType)
   {
     case etArticleNShS:     //НШС
                               EDFileName = ParameterNShSArticle;
                               break;
     case etArticleDamage:   // АВАР
                               EDFileName = ParameterDamageArticle;
                               break;
     case etArticleGoInitial: //ПРИ
                               EDFileName = ParameterGoInitialArticle;
                               break;
     case etArticleNShS_REP:     //НШС_РЭП
                               EDFileName = ParameterNShSArticle_REP;
                               break;
     case etArticleDamage_REP: // АВАР_РЭП
                               EDFileName = ParameterDamageArticle_REP;
                               break;
   case etArticleGoInitial_REP: //ПРИ_РЭП
                               EDFileName = ParameterGoInitialArticle_REP;
                               break;
     case etPart_REP1:        // первый раздел РЭП
                               pr_REP = 1;
                               goto F;
     case etPart_REP2:         // второй раздел РЭП
                               pr_REP = 2;
                               goto F;
     case etPart_REP3:         // третий раздел РЭП
                               pr_REP = 3;
  F:                           FileName =  PartisionRep->rzd;
                               FileName = ParameterSPOLocation + "\\"
                                    + FileName + "\\" + FileName + ".rtf";
                               EDFileName = FileName;
                               break;
   }
if (EDFileName.IsEmpty())
	{
        if (!bFromPris)
			ErrorMessageBox( "Файл ЭД не подключен к ИСР", "Просмотр ЭД" );
        return (-2);
    }
// имя файла НШС_ИЗД
  if( !pr_REP ) // не будем загружать файлы РЭП
      FileName = ParameterFilesLocation + "\\" + EDFileName + EDFilesExtention;

 // если файл есть
   if (FileExists(FileName))
	{
	   AnsiString S = "";
	   bool mm = ISRMainForm->WordApplication->get_Visible();
       if( mm != 0 ) // если уже есть загруженный документ
           PartisionRep->Close();
        // установим связь
       try { WordApplication->Connect(); }
       catch(...)
		{
		  WriteToDebugFile( "Ошибка Connect с WORD" );
		  ErrorMessageBox( "Ошибка Connect с WORD " );
          return -1;
        }
        // проверим, не осталось ли открытых разделов РЭП
	  if( pr_REP )
		 for( int j = 1; j <= WordApplication->Documents->Count; j++ )
		   {
			 OleVariant ItemIndex = j;
             ss = WordApplication->Documents->Item(ItemIndex)->Name;
			 ss.SetLength( ss.Length() - 4 );
             switch(pr_REP)
			  {
				 case 1: if( ss == Second_Rep || ss == Third_Rep)
                         WordApplication->Documents->Item(ItemIndex)->Close();
                         break;
                 case 2: if( ss == Partision_Rep || ss == Third_Rep)
						 WordApplication->Documents->Item(ItemIndex)->Close();
						 break;
				 case 3: if( ss == Second_Rep || ss == Partision_Rep)
                         WordApplication->Documents->Item(ItemIndex)->Close();
                         break;
              }
           }
		// сделаем видимым
		WordApplication->Visible = true;
		TVariant VarFileName = FileName;
		TVariant ReadOnty = true;

        // откроем файл
		try { WordApplication->Documents->Open( &VarFileName, EmptyParam(), &ReadOnty, EmptyParam(),
											   EmptyParam(), EmptyParam(), EmptyParam(), EmptyParam(),
											   EmptyParam(), EmptyParam() );
			}
		catch(...)
		{
		  ErrorMessageBox(("Ошибка открытия файла " + FileName).c_str());
		  WriteToDebugFile(("Ошибка открытия файла " + FileName).c_str());
		  return -1;
		}
	 /*   enum class WdWindowState
		  {  wdWindowStateNormal = 0,
			 wdWindowStateMaximize = 1,
			 wdWindowStateMinimize = 2
		  };  */

		WordApplication->WindowState = (WdWindowState)0; // wdWindowStateNormal;

        //AnsiString WindowTitle = EDFileName + EDFilesExtention + " (только чтение) - Microsoft Word";
        //HWND hw = WordApplication->ActiveDocument->get_ActiveWindow();
         //ss = WordApplication->Get_CaptionLabels ();
        //bool h = WordApplication->ActiveDocument->ReadOnly ;
        // найдем окно
//        AnsiString WindowTitle = EDFileName + EDFilesExtention + " (только чтение) - Microsoft Word";
        //HWND hwndWord = FindWindow("OpusApp", WindowTitle.c_str());
        // его размер - по ИСР
//        SetWindowPos(hwndWord, HWND_BOTTOM, Left, Top, Width, Height, SWP_FRAMECHANGED);

	 // если есть метка
	if (!Label.IsEmpty())
		{
			TVariant What = -1; //wdGoToBookmark;
            TVariant Name = Label;
            TVariant Ans = WordApplication->ActiveDocument->Bookmarks->Exists(Name);
            if (Ans) {
				WordApplication->Selection->GoTo( &What, (TVariant*)0/*EmptyParam*/,
													 (TVariant*)0/*EmptyParam*/, &Name);
            }
            else {
                   nResult = -3;
                 }
        }
    }
	else
	 {
		if (!bFromPris)
		 {
			ErrorMessageBox( "Отсутствует файл \"" + FileName + "\".", "Просмотр ЭД" );
		 }
		nResult = -1;
	 }
    return (nResult);
}
//------------------------------------------------------------------------------
// директива "ЭД"
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::ProcessEDDir(AnsiString DirText)
{
    int nResult = 0;
    int nDelPos = DirText.LastDelimiter("|");
    int nStringLength = DirText.Length();
	AnsiString StrToProtokol;
    AnsiString EDName;
    AnsiString Label;

    // если разделителя нету
	if (nDelPos == 0)
	 {
        nResult = 1;
		StrToProtokol = "Отсутствует разделитель между параметрами директивы";
	 }
	else
	 {
        // тип ЭД
        EDType CurrentType;
        bool bGoodEDName = true;
        // выделим название ЭД
        EDName = DirText.SubString(1, nDelPos - 1);
        // выделим метку
        Label = DirText.SubString(nDelPos + 1, nStringLength -nDelPos);
		if (EDName == EDNshSArticleName)
		 {
            CurrentType = etArticleNShS;
		 }
		else
		 {
			if (EDName == EDDamageArticleName)
			 {
                CurrentType = etArticleDamage;
			 }
			else
			 {
				if (EDName == EDGoInitialArticleName)
				 {
                    CurrentType = etArticleGoInitial;
				 }
				else
				 {
                    nResult = 1;
					StrToProtokol = "Неверный вид ЭД";
                    bGoodEDName = false;
				 }
			 }
		 }
		if (bGoodEDName)
		 {
            // покажем ЭД
			nResult = ShowED( CurrentType, Label, true );
			if( nResult == -1 )
			 {
				StrToProtokol = "Файл ЭД на диске не найден";
			 }
			else
			 {
				if( nResult == -2 )
				 {
					StrToProtokol = "Файл ЭД не подключен к ИСР";
				 }
				else
				 {
					if( nResult == -3 )
					 {
						StrToProtokol = "Закладка \"" + Label + "\" в файле ЭД отсутствует";
					 }
				 }
			 }
		 }
	 }
	// если директива вып. нормально или не показ. закладка
	if( nResult == 0 || nResult == -3 )
	 {
		SetMainWindowOnTop();
        AnsiString Message;
		Message = "В Word загружен документ \"" +  EDName + "\".";
		InformationMessageBox( Message );
		if (nResult == -3)
		 {
			Message = "Закладка \"" + Label + "\" в файле ЭД отсутствует";
			ErrorMessageBox( Message, "Ошибка при записи в файл" );
		 }
	 }
    // если директива вып. нормально
	if (nResult == 0)
	 {
        // шлем ответ - директива выполнена
        SendEDDirAnswerToPris(1);
	 }
	else
	 {
        // шлем ответ - директива не выполнена
        SendEDDirAnswerToPris(0);
		SendStringToPrisProtokol( StrToProtokol, stError, true, true, false );
	 }
}
//------------------------------------------------------------------------------
// директива "НА_СР"
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::ProcessNASRDir(AnsiString DirText)
{
    int nResult = 0;
    int nDelPos = DirText.LastDelimiter("|");
    int nStringLength = DirText.Length();
    AnsiString StrToProtokol;

	// если разделов нет
	if( nPartitionCount == 0 )
	 {
		nResult = 1;
		StrToProtokol = "Нельзя выполнить переход внутри раздела: структура не загружена";
	 }
	else
	 { // если структура не загружена
	   if( !bMainPartitionStructureLoaded )
        {
		  nResult = 1;
		  StrToProtokol = "Нельзя выполнить переход внутри раздела: структура не загружена";
        }
       else
		{ // если разделителя нету
		  if( nDelPos == 0 )
           {
			  nResult = 1;
			  StrToProtokol = "Отсутствует разделитель между параметрами директивы";
           }
          else
		   {
			  int nSectionNumber;
              bool bGoodSectionName = true;
              // выделим название секции
			  AnsiString SectionName = DirText.SubString( 1, nDelPos - 1 );
              // выделим метку
			  AnsiString Label = DirText.SubString( nDelPos + 1, nStringLength - nDelPos );
			  if( SectionName == NormalWorkLabel )
               {
                 nSectionNumber = 0;
               }
              else
               {
				 if( SectionName == NShSWorkLabel )
                  {
                    nSectionNumber = 1;
                  }
                 else
                  {
					if( SectionName == GoInitialLabel )
                     {
                       nSectionNumber = 2;
                     }
                    else
                     {
                       nResult = 1;
					   StrToProtokol = "Неверный вид секции";
                       bGoodSectionName = false;
                     }
                   }
                }
			 if( bGoodSectionName )
			  {
				// сделаем секцию текущей
				SetTopSection( nSectionNumber );
				// если есть метка
                if (!Label.IsEmpty())
				 {
				   if (nSectionNumber != 1)
					{
					  nResult = 1;
					  StrToProtokol = "Метки допустимы только в секции НШС";
					}
				   else
					{
					  TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
					  int nCurrentNShSBlock = StrToInt( Label.SubString( 2, Label.Length() - NShSLabel.Length() - 1));
					  // перейдем к ней
					  int nRes = ch->SetNshSBlock(nCurrentNShSBlock);
					   // если ошибка
					  if( nRes < 0 )
					   {
						 switch (nRes)
						  { // не найдена метка НШС
							case (-2): StrToProtokol = "Блок \"" + Label + "\" не найден";
									   break;
							 // в блоке НШС нет операций
							case (-3): StrToProtokol = "В блоке \"" + Label + "\" не найдено ни одной операции. Невозможно встать на данный блок";
                                    break;
						  }
                      nResult = 1;
                    }
                  }
                 }
                }
            }
        }
    }
    // если директива вып. нормально
    if (nResult == 0)
	 { // шлем ответ - директива выполнена
       SendNASRDirAnswerToPris(1);
     }
    else
     { // шлем ответ - директива не выполнена
       SendNASRDirAnswerToPris(0);
	   SendStringToPrisProtokol(StrToProtokol, stError, true, true, false);
     }
}
//---------------------------------------------------------------------
// обработчик события "Выход из Word"
void __fastcall TISRMainForm::WordApplicationQuit(TObject *Sender)
{
    PartisionRep->Close();
    // разорвем связь
    WordApplication->Disconnect();
}
//---------------------------------------------------------------------------
// конфигурирование главного меню
void __fastcall TISRMainForm::ConfigureMenu()
{
  int i;
  TMDIChild* ch;
  bool q = 0;
   //
  if( pr_save && nPartitionCount && !pr_vosst )       // сохраняем КТ
	  {
		  if( !save_loc.IsEmpty() )
			  {
				save_KT( save_loc );
				WriteToDebugFile( "Локальное сохранение" );
			  }
		  if( !save_rem.IsEmpty() )
			  {
				save_KT( save_rem );
				WriteToDebugFile( "Удаленное сохранение" );
			  }
	  }
    // флаг: есть текущая секция
    bool bExistsSections = false;
    // флаг: выполняется блок "Авто"
    bool bAutoBlockExecuted = IsBlockAutoExecuted();
	// если секция одна и у нее загружена структура или разделов не один
	if( (nPartitionCount == 1 && bMainPartitionStructureLoaded ) || nPartitionCount > 1 )
	  {
		bExistsSections = true;                          // флаг: есть текущая секция
        // указатель на нее
        ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
      }
    // если раздел не выбран
	if( ISRCurrentState == isNonPartitionSelected )
	 {
	   if( bPrisLoaded )                                  // если ПРИС загружен
	   {
		 Caption = AppTitle + " - [раздел не выбран]";    // заголовок главной формы
	   }
		else
		 {
			Caption = AppTitle + " - [ждем загрузки ПРИС...]";
         }
	 }
	else
	   {
		 if( ISRCurrentState == isPartitionSelected )     // если выбран раздел
           {
			 Caption = AppTitle + " - [выбран раздел: " + Partitions[nSelectedPartitionNumber].Abbr + "]";
           }
		  // раздел загружен
		   else
		   { // если есть разделы и текущий раздел - головной и он без файла структуры
			 if (nPartitionCount > 0 && nCurrentPartition == 0 && !bMainPartitionStructureExists)
			 {
				  Caption = AppTitle + " - [" + LoadedPartitions[nCurrentPartition].Abbr + ", файл структуры штатно отсутствует]";
			 }
			 // раздел с файлом структуры
			 else
			 { // если структура загружена
			   if (nCurrentPartition == 0 && bMainPartitionStructureLoaded || nCurrentPartition > 0)
				{
                  Caption = AppTitle;
				}
               // структура не загружена
               else
				{
					Caption = AppTitle + " - [" + LoadedPartitions[nCurrentPartition].Abbr + ", файл структуры не загружен]";
                }
             }
           }
       }
// меню "Подготовка"
 if( bPrisProcessEndPartition )                       // если ждем от ПРИС ответ на конец раздела
  {
	PreparePartitionsList->Enabled = false;
	PrepareHeadline->Enabled = false;
	PrepareCheckSPO->Enabled = false;
	PreparePFKS->Enabled = false;
	PrepareLoadStructure->Enabled = false;
	PrepareDebugPartition->Enabled = false;
	PrepareAddPartitions->Enabled = false;
	PrepareRebuildList->Enabled = false;
	PrepareStencil->Enabled = false;
  }
 else
  {
	if( bPrisLoaded || bDeveloperWorkPlace )          // если ПРИС к этому моменту загружен
	 {
	   // "Список разделов"
	   PreparePartitionsList->Enabled = ( !bPartitionDebug ) && ( !bAutoBlockExecuted );
	   // "Отладка раздела"
	   PrepareDebugPartition->Enabled = (ISRCurrentState != isPartitionCarringOut) && (!bAutoBlockExecuted);
	 }

	else
	 {                                                 // ПРИС еще не загружен
	   PreparePartitionsList->Enabled = false;         // список вызвать нельзя
	   PrepareDebugPartition->Enabled = false;
	 }
	switch( ISRCurrentState )
	 {
	   case (isNonPartitionSelected):  // если ни один раздел не выбран
									   PrepareHeadline->Enabled = false;
									   PrepareCheckSPO->Enabled = false;
									   PreparePFKS->Enabled = false;
									   PrepareLoadStructure->Enabled = false;
									   break;

	   case (isPartitionSelected):     // выбран раздел
									   PrepareHeadline->Enabled = true;
									   PrepareCheckSPO->Enabled = false;
									   PreparePFKS->Enabled = false;
									   PrepareLoadStructure->Enabled = false;
									   break;
      // раздел проводится
      case (isPartitionCarringOut):
                PrepareHeadline->Enabled = false;
                // если один раздел и у него не загружена структура
                if (nPartitionCount == 1 && !bMainPartitionStructureLoaded)
                {
                    PrepareCheckSPO->Enabled = true;
                    PreparePFKS->Enabled = true;
                }
                else {
                       PrepareCheckSPO->Enabled = false;
                       PreparePFKS->Enabled = false;
                     }
                PrepareLoadStructure->Enabled = (nPartitionCount == 1) && (bMainPartitionStructureExists && !bMainPartitionStructureLoaded);
                break;
    }  // switch
  // "Добавить разделы в список разделов..."
  PrepareAddPartitions->Enabled = (ISRCurrentState != isPartitionCarringOut) && (!bPartitionDebug) && (!bAutoBlockExecuted);
  // "Перестроить список разделов"
  PrepareRebuildList->Enabled = (ISRCurrentState != isPartitionCarringOut) && (!bPartitionDebug) && (!bAutoBlockExecuted);
  // "Трафарет..."
  PrepareStencil->Enabled = (ISRCurrentState != isPartitionCarringOut) && (!bAutoBlockExecuted);
 }  // else ПРИС загружен

// меню "Разделы"
// если разделов нету
if (nPartitionCount == 0)
 {
   try{ PartitionMenuItems[0]->Visible = true;}
   catch(...)
     { return;}
   PartitionMenuItems[0]->Caption = "< Разделов нет >";
   PartitionMenuItems[0]->Enabled = true;
   PartitionMenuItems[0]->Checked = false;
   PartitionMenuItems[1]->Visible = false;
   PartitionMenuItems[2]->Visible = false;
 }
else
 { // цикл по разделам
   for( i = 0; i < nPartitionCount; i++ )
    {
      PartitionMenuItems[i]->Visible = true;
      // заголовок пункта меню - имя раздела
      PartitionMenuItems[i]->Caption = LoadedPartitions[i].Abbr;
      // если текущий раздел - выделим
	  if( i == nCurrentPartition )
       {
         PartitionMenuItems[i]->Checked = true;
       }
      else
       {
         PartitionMenuItems[i]->Checked = false;
       }
      // доступен, если не ждем ответ на конец раздела
      PartitionMenuItems[i]->Enabled = !bPrisProcessEndPartition;
    }
   // цикл по оставшимся пунктам меню
   for( i = nPartitionCount; i < MaxPartitionCount; i++ )
     {
       PartitionMenuItems[i]->Visible = false;
     }
  }
// меню "Секции"
// если загруженных разделов нет или есть, но мы на головном разделе и структура не загружена
 if( ( nPartitionCount == 0 ) || ( nPartitionCount > 0 && nCurrentPartition == 0 &&
												 bMainPartitionStructureLoaded == false ) )
  {  // делаем пункты секций недоступными и без отметки
     SectionsNormal->Enabled = false;
     SectionsNShS->Enabled = false;
     SectionsGoInitial->Enabled = false;
     SectionsNormal->Checked = false;
     SectionsNShS->Checked = false;
     SectionsGoInitial->Checked = false;
  }
 // загруженные разделы есть и структура загружена
 else {
        SectionsNormal->Enabled = !bPrisProcessEndPartition;
        SectionsNShS->Enabled = !bPrisProcessEndPartition;
        SectionsGoInitial->Enabled = !bPrisProcessEndPartition;
        // установим флажек на активной секции
        switch (anCurrentSection[nCurrentPartition])
         {
            case (0): SectionsNormal->Checked = true;
                      SectionsNShS->Checked = false;
                      SectionsGoInitial->Checked = false;
                      break;
            case (1): SectionsNormal->Checked = false;
                      SectionsNShS->Checked = true;
                      SectionsGoInitial->Checked = false;
                      break;
            case (2): SectionsNormal->Checked = false;
                      SectionsNShS->Checked = false;
                      SectionsGoInitial->Checked = true;
                      break;
         }
      }
// Меню "Работа"
// если ждем от ПРИС ответ на конец раздела
if (bPrisProcessEndPartition)
  {
     WorkMakeOperation->Enabled = false;
     WorkGoToActiveSection->Enabled = false;
     WorkSP->Enabled = false;
     WorkEndAllPartitions->Enabled = false;
     WorkEndPartition->Enabled = false;
     WorkUndoChosenVariant->Enabled = false;
     WorkPartitionLength->Enabled = false;
     WorkOptions->Enabled = false;
	 WorkImitAnswer->Enabled = false;
  }
else
  { // пункт "Имитация ответа ПРИС на КО"
    WorkImitAnswer->Enabled = (nOperationInPrisCount > 0);
    // пункт "Выполнить операцию"
    // доступен, если есть секция и в ней есть операции
    if (bExistsSections)
     {
	   q = (ch->GetOperationCount() > 0);
	   q = q && (!ch->GetPrisExecutesOperation());
	   q = q && (!bAutoBlockExecuted);
	   /////WorkMakeOperation->Enabled = (ch->GetOperationCount() > 0) && (!ch->GetPrisExecutesOperation()) && (!bAutoBlockExecuted);
	   WorkMakeOperation->Enabled = q;
	 }
    else {
            WorkMakeOperation->Enabled = false;
         }
    // пункт "Перейти к активной секции"
    WorkGoToActiveSection->Enabled = nOperationInPrisCount > 0;
    // пункт "СП"
	// доступен, если есть разделы
	q = (nPartitionCount > 0);
	q = q && (!bAutoBlockExecuted);
	q = q && (totalOperationsInPris == 0);
	/////WorkSP->Enabled = (nPartitionCount > 0) && (!bAutoBlockExecuted) && (totalOperationsInPris == 0);
	WorkSP->Enabled = q;
	// пункт "Выход из разделов"
	// доступен, если есть разделы
	q = (nPartitionCount > 0);
	q = q && (!bAutoBlockExecuted);
	////WorkEndAllPartitions->Enabled = (nPartitionCount > 0) && (!bAutoBlockExecuted);
	WorkEndAllPartitions->Enabled = q;
    // пункт "Конец раздела"
	// доступен, если есть разделы и мы на последнем разделе
	q = (nPartitionCount > 0);
	q = q && (nCurrentPartition == nPartitionCount - 1);
	q = q && (!bAutoBlockExecuted);
	////WorkEndPartition->Enabled = (nPartitionCount > 0) && (nCurrentPartition == nPartitionCount - 1) && (!bAutoBlockExecuted);
	WorkEndPartition->Enabled = q;
	// пункт "Отменить вариант"
    // если есть секции
    if (bExistsSections)
     { // в текущ.секции есть выделенн.вариант и он не выполняется
	   q = ch->GetExistsSelectedVariant();
	   q = q && !ch->GetPrisExecutesOperation();
	   /////WorkUndoChosenVariant->Enabled = (ch->GetExistsSelectedVariant() && !ch->GetPrisExecutesOperation());
	   WorkUndoChosenVariant->Enabled = q;
	 }
    else {
            WorkUndoChosenVariant->Enabled = false;
         }
	// пункт "Продолжительность раздела"
    WorkPartitionLength->Enabled = nPartitionCount > 0;
    // пункт "Настройка"
    WorkOptions->Enabled = !bAutoBlockExecuted;
  }
// меню "Авто"
 if (bExistsSections)
  { // пункт "Начать/окончить отметку блока "Авто"
    // доступен, если есть окна с секциями
    AutoBeginFinishBlock->Enabled = !ch->GetExecuteAutoBlock();
    // пункт "Снять отметку блока "Авто"
    // доступен, если есть блок "Авто"
    AutoClearSelection->Enabled = (ch->GetAutoFirst() != -1) && (!ch->GetExecuteAutoBlock());
    // пункт "Выполнить блок "Авто"
    // доступен, если есть блок "Авто" и какой-либо блок "Авто" не вып.
    AutoMakeAuto->Enabled = (ch->GetAutoFirst() != -1) && (!IsBlockAutoExecuted());
    // пункт "Остановить выполнение блока "Авто"
    // доступен, блок "Авто" выполняется и
    //не поступало заявки на его остановку
	AutoStopAutoBlock->Enabled = ch->GetExecuteAutoBlock() && !(ch->GetStopAutoRequest());
  }
 else
  { // пункт "Начать/окончить отметку блока "Авто"
    AutoBeginFinishBlock->Enabled = false;
    // пункт "Снять отметку блока "Авто"
    AutoClearSelection->Enabled = false;
    // пункт "Выполнить блок "Авто"
    AutoMakeAuto->Enabled = false;
    // пункт "Остановить выполнение блока "Авто"
    AutoStopAutoBlock->Enabled = false;
  }
// меню "Изделие"
// пункты доступны, если есть строки в файле настройки о соотв. файлах ЭД и строка "Файлы"
ArticleNShS->Enabled = !ParameterNShSArticle.IsEmpty() && !ParameterFilesLocation.IsEmpty();
ArticleDamage->Enabled = !ParameterDamageArticle.IsEmpty() && !ParameterFilesLocation.IsEmpty();
ArticleGoInitial->Enabled = !ParameterGoInitialArticle.IsEmpty() && !ParameterFilesLocation.IsEmpty();
 // для РЭП
 ArticleNShS_REP->Enabled = !ParameterNShSArticle_REP.IsEmpty() && !ParameterFilesLocation.IsEmpty();
ArticleDamage_REP->Enabled = !ParameterDamageArticle_REP.IsEmpty() && !ParameterFilesLocation.IsEmpty();
ArticleGoInitial_REP->Enabled = !ParameterGoInitialArticle_REP.IsEmpty() && !ParameterFilesLocation.IsEmpty();
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PartitionsAnyItemClick( TObject *Sender )
{
  int nPartitionNumber;                             // номер выбранного раздела
  if( nPartitionCount == 0 )                        // если разделов нет
			 return;
  TMenuItem* SenderItem = (TMenuItem*)Sender;       // пункт меню, вызвавший событие
  // определим номер выбранного раздела
  if( SenderItem == PartitionMenuItems[0] )
			 nPartitionNumber = 0;
  else
   {
	 if( SenderItem == PartitionMenuItems[1] )
		   nPartitionNumber = 1;
	 else
		   nPartitionNumber = 2;
   }
	// если щелкнули на текущем разделе
    if (nPartitionNumber == nCurrentPartition) return;
    // если головной раздел и в нем нет структуры
	if(nPartitionNumber == 0 && !bMainPartitionStructureLoaded)
     {
		AnsiString Message = "В разделе \"" + GetPartitionTitle(0) + "\" не загружен файл структуры. Переключится к данному разделу невозможно";
		WarningMessageBox(Message);
        return;
     }
 // установим номер текущего раздела
	nCurrentPartition = nPartitionNumber;
	// меню
    ConfigureMenu();
    // установка активной секции
	SetTopSection(anCurrentSection[nCurrentPartition]);
}
//---------------------------------------------------------------------------
// активизация определенной секции определенного раздела
// если nPartitionNumber == -1, то активизируется секция текущего раздела
void __fastcall TISRMainForm::SetTopSection(int nSectionNumber, int nPartitionNumber)
{
    TMDIChild* ch;
    int nLocalPartitionNumber;
    // если надо установить секцию текущего раздела
    if (nPartitionNumber == -1)
      {
        nLocalPartitionNumber = nCurrentPartition;
      }
    else {
            nLocalPartitionNumber = nPartitionNumber;
         }
    ch = LoadedPartitions[nLocalPartitionNumber].pWindow[nSectionNumber];
    // сделаем окна активным
    ch->Show();
    // глобальная переменная: номер текущей секции
    anCurrentSection[nLocalPartitionNumber] = nSectionNumber;
    if (nPartitionNumber != -1)
     { // сменим текущий раздел
        nCurrentPartition = nPartitionNumber;
	 }
    ConfigureMenu();
    return;
}
//-----------------------------------------------------------------------------
// Выбрали п. меню "Заголовок раздела"
//-----------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareHeadlineClick( TObject *Sender )
{
	// если трафарета нет
	if( !bStencilExists )
     {
	   WarningMessageBox( "В системе отсутствует трафарет. Задание заголовка невозможно" );
       return;
     }
    // если параметр "ПРОТОКОЛ=" есть
	if( bParameterProtokolExistsAndGood )
	 {
	   Title( ParameterProtokol );           // заголовок
     }
	else
	 {
	   SendRequestCurrentPartitionToPris();  // запрос текущего каталога
	 }
  //ISRMainForm->PrepareHeadline->Enabled = false;
}
//-------------------------------------------------------------------
// функция выполняет работы по вводу заголовка и началу раздела
//-------------------------------------------------------------------
void __fastcall TISRMainForm::Title( AnsiString ProtokolDir )
{
	char* sp;
	if( ProtokolDir == "pppp" )
		  goto PP;
	TitleBox->SetProtokolDir( ProtokolDir );
	TitleBox->ShowModal();
	// если заголовок раздела введен
	if( TitleBox->IsOK() )
	 {
	   sp = strpbrk( TBPartitionTitle.c_str(), " \\*?:/|" );
	   if(sp)
		{
		  ErrorMessageBox( "Недопустимый символ в имени протокола" );
		  return;
		}
	   Sleep(500);
	   if( r_vrem )
		{
		  WriteToDebugFile( "Посылаем РВРЕМЯ" );
		  SendKOToPris( "РВРЕМЯ",false );
		}
	   // установим переменные
		MainPartitionTitle = TBPartitionTitle;
        // кол-во разделов
        nPartitionCount = 1;
		WriteToDebugFile( "Начат раздел " + TBPartitionTitle );
        // номер текущего раздела
        nCurrentPartition = 0;
        // номер раздела в списке
        LoadedPartitions[0] = Partitions[nSelectedPartitionNumber];
        // номер резки протокола
        nBisect = 1;
        PartitionStartTickCount = GetTickCount();
		// раздел проводится
        ISRCurrentState = isPartitionCarringOut;
       //SnapFoto("В Title() после ISRCurrentState = isPartitionCarringOut");
        // флаг: головной раздел загружен из ИСР
        bMainPartitionLoadedFromISR = true;
     // пошлем директории в ПРИС
	 if( !pr_vosst ) // если не восстанавливаемся
      {
		SendBeginPartitionToPris( MainPartitionTitle );  // пошлем начало раздела в ПРИС
		SendDirectoriesToPris();
		TM->Interval = 5000;
		TM->Enabled = true;
		return;                                          // уходим; вернуться после получения ответа
        // пошлем заголовок окна в ПРИС
        //SendWindowTitleToPris();
   PP:  if( bPartitionDebug )
         {
			AnsiString KatalogOperation = "КАТАЛОГ " + DebugDir + "\\";
            SendKOToPris(KatalogOperation, false);
         }
        else
        {
		   if( LoadedPartitions[0].nDirectioriesCount > 0 )
            {
			   AnsiString Katalog = ParameterSPOLocation + "\\" + LoadedPartitions[0].Abbr + "\\" + LoadedPartitions[0].Directories[0];
			   if (DirectoryExists(Katalog))
                {
					AnsiString KatalogOperation = "КАТАЛОГ " + Katalog + "\\";
					SendKOToPris( KatalogOperation,false );
                }
            }
        }
       // выведем КС настройки в протокол
       PrintAdjstmentKSToProt();
//      ISRCurrentState = isPartitionSelected;
//      ISRCurrentState = isNonPartitionSelected;
        // кол-во разделов
//        nPartitionCount = 0;
        // перестроим меню
        //SnapFoto("В Title() перед ConfigureMenu()");

        ConfigureMenu();
      } // if(pr_vosst)
    }
}
#include <fcntl.h>
#include <sys/stat.h>
#include <winbase.h>
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::Prepare()
{
	aHeader[0][0] = "№";
	aHeader[0][1] = "Т";
	aHeader[0][2] = "Команды, доклады, пояснения";
	aHeader[0][3] = "Операции по управлению и контролю";
	aHeader[1][0] = "№";
	aHeader[1][1] = "Т";
	aHeader[1][2] = "Диагностика НШС";
	aHeader[1][3] = "Операции по управлению и контролю";
	aHeader[2][0] = "№";
	aHeader[2][1] = "Т";
	aHeader[2][2] = "Команды, доклады, пояснения";
	aHeader[2][3] = "Операции по управлению и контролю";

	SectionType[0] = "ШТАТ";
	SectionType[1] = "НШС";
	SectionType[2] = "ПРИ";

	AutoMessage = "Автоматическое выполнение";
	PartitionStartDirName = "ПУСКРАЗД";
	ISRStartDirName = "НАЧРАЗД";
	GoInitialLabel = "ПРИ";
	NormalWorkLabel = "ШТАТ";
	NShSWorkLabel = "НШС";
	NShSLabel = "НШС";
	NShSEndLabel = "К_";
	ReportName = "Доклад";
	CommandName = "Команда";
	MessageName = "Указание оператору";
	DipolOperationName = "Директива для ПРИС";
	ISROperationName = "Директива ИСР";
	AnsiString FileReadErrorMessage0 = "отсутсвует символ новой строки после символа возврата каретки";
	AnsiString FileReadErrorMessage1 = "длина строки превышает максимально допустимую";
	strcpy( acFileReadErrorMessage[0], FileReadErrorMessage0.c_str() );
	strcpy( acFileReadErrorMessage[1], FileReadErrorMessage1.c_str() );
	AutoStartPartitionLabel = "АВТ";
	StepStartPartitionLabel = "ШАГ";
	ProgramCallStament = "Вызвать";
	SPStament = "СП";
	EDNshSArticleName = "НШС_ИЗД";
	EDDamageArticleName = "АВАР_ИЗД";
	EDGoInitialArticleName = "ПРИ_ИЗД";
    NASRDirName = "НА_СР";

	AnsiString sDIPOL_OPERATION_SYMBOL = "О";
	AnsiString sISR_OPERATION_SYMBOL = "И";
	AnsiString sVARIANT_SYMBOL = "В";
	AnsiString sCOMMAND_SYMBOL = "К";
	AnsiString sREPORT_SYMBOL = "Д";
	AnsiString sMESSAGE_SYMBOL = "П";

    DIPOL_OPERATION_SYMBOL = sDIPOL_OPERATION_SYMBOL[1];
	ISR_OPERATION_SYMBOL = sISR_OPERATION_SYMBOL[1];
    VARIANT_SYMBOL = sVARIANT_SYMBOL[1];
    COMMAND_SYMBOL = sCOMMAND_SYMBOL[1];
	REPORT_SYMBOL = sREPORT_SYMBOL[1];
	MESSAGE_SYMBOL = sMESSAGE_SYMBOL[1];

	AnsiString sHandPartition = "Р";
	AnsiString sCommentString = "К";
	AnsiString sReferenceString = "П";
	AnsiString sSpecialFacilities = "С";

	HandPartition = sHandPartition[1];
    CommentString = sCommentString[1];
	ReferenceString = sReferenceString[1];
    SpecialFacilities = sSpecialFacilities[1];

    // запомним название приложения
	AppTitle = "ИСР";
    Application->Title = AppTitle;
    // текущий каталог
    AnsiString sPath = ParamStr(0);
	ProgramDir = sPath.SubString(1, sPath.LastDelimiter("\\") - 1);
    // имя файла с ошибками
    ErrFileName = ProgramDir + "\\ISR Errors.txt";
  	Application->OnException = OnException;
    // имя файла отладки
    DebugFileName = ProgramDir + "\\ISR Debug.txt";
 ///////////////////////////////////////////////////////////
try{
	if( FileExists( DebugFileName ) )
	  {
		 int i;
		 chmod( DebugFileName.c_str(), S_IREAD | S_IWRITE );
		 int hnd = open( DebugFileName.c_str(), O_RDWR | O_BINARY );
		 if( hnd != -1 )
			{
			  long lng = filelength(hnd);
              close(hnd);
			  if( lng > 2000000 )
				 {
				   AnsiString new_name;
				   for( i = 1; i++; i <= 100 )
					 {
					   new_name = ProgramDir + "\\ISR Debug" + IntToStr(i-1) + ".txt";
					   if( FileExists(new_name) )
							continue;
                       else break;
                     }
				   if( i == 100 )
					  remove( new_name.c_str() );
				   rename( DebugFileName.c_str(), new_name.c_str() );
                 }
            }
      }
	}
	catch(...){;}

//////////////////////////////////////////////////////////////
    // ShowMessage("ИСР: создадим именованный мьютекс");
    // создадим именованный мьютекс
	HANDLE hTestMutex = ::CreateMutex( NULL, TRUE, L"ISR" );
    // если он не создался, т.к. уже существует
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	 {
	   if( hwndPrevInstance == NULL )
		{
		  ErrorMessageBox( "В памяти осталась неработоспособная копия ИСР.\n Необходимо завершить работу ПРИС и выгрузить\n неработоспособный ИСР через диспетчер.");
		  WriteToDebugFile( "В памяти неработоспособная копия ИСР" );
		  try{ Application->Terminate(); }
          catch(...)
			{
			  ShowMessage( "ИСР: Ошибка при останове программы" );
			}
//        exit(EXIT_FAILURE);
		  ReleaseMutex( hTestMutex );
          return;
        }  
		  try { KillApplication( hwndPrevInstance ); }
          catch(...)
		  {
		    ShowMessage( "ИСР: Ошибка при удалении старой копии" );
            Application->Terminate();
            return;
          }
	 }
  int i;
    // коэфициент, учитывающий размер системного шрифта
    dScr = (double)PixelsPerInch / 96.0;
    // откорректируем высоту ToolBar
    ToolBar->Height = PrepareBtn->Height + 4;
    // получим ширину экрана в пикселах
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	if( nScreenWidth == 640 || nScreenWidth == 800 )
	 {
	  if (dScr > 1)
		  nGridRowCount = 14;
	  else
		  nGridRowCount = 15;
     }
	if( nScreenWidth >= 1024 )
	 {
	   if( dScr > 1 )
		  nGridRowCount = 23;
	   else
		  nGridRowCount = 24;
     }
    nGridHeight = (nGridRowCount + 1)* nGridRowHeight + nGridRowCount + 4;
    nMainWindowHeight = nGridHeight + ToolBar->Height + 51;

    // информация о версии
    OSVERSIONINFO osCurrentVersion;
    osCurrentVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&osCurrentVersion) == 0)
     {
		ErrorMessageBox( "Критическая ошибка", "Ошибка" );
		exit(EXIT_FAILURE);
        return;
	 }
    // для Win 2000
	if( osCurrentVersion.dwPlatformId == VER_PLATFORM_WIN32_NT && osCurrentVersion.dwMajorVersion > 4 )
     {
		nMainWindowHeight += 1;
     }
	if( dScr > 1 )
     {
		nMainWindowTop += 6;
        nGridWidth += 4;
        nMainWindowHeight += 5;
		nMainWindowWidth += 4;
     }
    // положение окна
	Left = nMainWindowLeft;
	Top = nMainWindowTop + 50;
    Width = nMainWindowWidth;
    Height = nMainWindowHeight;

     // пункты меню "Раздел"
	PartitionMenuItems[0] = PartitionsFirst;
    PartitionMenuItems[1] = PartitionsSecond;
    PartitionMenuItems[2] = PartitionsThird;

    //  пока нет окон для загруженных разделов
	 for( i = 0; i < nPartitionCount; i++ )
	   LoadedPartitions[i].pWindow[i] = 0;

	bDeveloperWorkPlace = FileExists( "Developer.txt" );
  // отладочное меню
   TestBtn->Visible = 0; //bDeveloperWorkPlace;
  // если кол-во параметров КС == 0
	if( ParamCount() == 0 )
     {
		ErrorMessageBox( "Неверный формат командной строки. Программа будет закрыта", "Ошибка" );
        Application->Terminate();
		return;
     }
    OnFileName = ParamStr(1);
	CfgFileName = ParamStr(2);
    // если параметров КС меньше 2
    // ShowMessage(IntToStr(ParamCount()));
	if( ParamCount() < 2 )
     {
		ErrorMessageBox( "Неверный формат командной строки. Программа будет закрыта", "Ошибка" );
		Application->Terminate();
		return;
     }
    // если файла настройки пользователя не существует
	if( !FileExists(OnFileName) )
	 {
		ErrorMessageBox( "Отсутствует файл \"" + OnFileName + "\". Программа будет закрыта", "Ошибка" );
        Application->Terminate();
		return;
     }
    // если системного файла настройки не существует
	if( !FileExists(CfgFileName) )
	 {
		ErrorMessageBox( "Отсутствует файл \"" + CfgFileName + "\". Программа будет закрыта", "Ошибка" );
        Application->Terminate();
        return;
     }

    char buffer[MAXP];
    // попытка открыть парнас
	if( openparn( OnFileName.c_str(), NULL ) != 0 )
     {
		ErrorMessageBox( "Ошибка при попытке доступа к пользовательскому файлу настройки ПРИСа. Программа будет закрыта", "Ошибка" );
        Application->Terminate();
        return;
	 }
	AnsiString sArticleAbnormal = "НШС_ИЗД";
	AnsiString sArticleBreakdown = "АВАР_ИЗД";
	AnsiString sArticleGoInitialState = "ПРИ_ИЗД";
	AnsiString sPFKS = "ПФКС";
	AnsiString sProtocol = "ПРОТОКОЛ";
	AnsiString sIsd = "ИЗДЕЛИЕ";
	AnsiString sFilesLocation = "РАЗМЕЩЕНИЕ_ФАЙЛОВ";
	AnsiString sAdjustmentsLocation = "РАЗМЕЩЕНИЕ_РАЗДЕЛОВ";
	AnsiString sSPOLocation = "РАЗМЕЩЕНИЕ_СПО";
	AnsiString sAdjustments = "РАЗДЕЛЫ";
    // Для РЭП
	 if( getparn("НШС_РЭП", buffer, MAXP) != 0 )
				  ParameterNShSArticle_REP = "";
     else
           ParameterNShSArticle_REP = buffer;
	 if( getparn( "АВАР_РЭП", buffer, MAXP) != 0 )
        ParameterDamageArticle_REP = "";
     else
           ParameterDamageArticle_REP = buffer;
	 if( getparn( "ПРИ_РЭП", buffer, MAXP ) != 0 )
          ParameterGoInitialArticle_REP = "";
     else  
           ParameterGoInitialArticle_REP = buffer;
    // Проверим
     /*if( ParameterNShSArticle_REP.IsEmpty() &&
           ParameterDamageArticle_REP.IsEmpty() &&
               ParameterGoInitialArticle_REP.IsEmpty() )*/
    ///////////////////////////////////////////////////////
    // запросим параметр
	if( getparn( sArticleAbnormal.c_str(), buffer, MAXP ) != 0 )
     {  // НШС_ИЗД
        ParameterNShSArticle = "";
     }
    else { // запомним параметр
           ParameterNShSArticle = buffer;
         }
    // запросим параметр
	if( getparn( sArticleBreakdown.c_str(), buffer, MAXP ) != 0 )
      { // АВАР_ИЗД
        ParameterDamageArticle = "";
	  }
    else { // запомним параметр
           ParameterDamageArticle = buffer;
         }
    // запросим параметр
	if( getparn( sArticleGoInitialState.c_str(), buffer, MAXP ) != 0 )
       {  // ПРИ_ИЗД
          ParameterGoInitialArticle = "";
       }
    else { // запомним параметр
           ParameterGoInitialArticle = buffer;
         }
    // запросим параметр
	if( getparn( sFilesLocation.c_str(), buffer, MAXP ) != 0 )
       { // РАЗМЕЩЕНИЕ_ФАЙЛОВ
          ParameterFilesLocation = "";
       }
    else { // запомним параметр
           ParameterFilesLocation = buffer;
         }
    // запросим параметр
	if( getparn( sAdjustmentsLocation.c_str(), buffer, MAXP ) != 0 )
       {  // РАЗМЕЩЕНИЕ_РАЗДЕЛОВ
          ParameterAdjustmentsLocation = "";
       }
    else { // запомним параметр
           ParameterAdjustmentsLocation = buffer;
         }
    // запросим параметр
	if( getparn( sSPOLocation.c_str(), buffer, MAXP ) != 0 )
         { // РАЗМЕЩЕНИЕ_СПО
           ParameterSPOLocation = "";
         }
    else { // запомним параметр
           char* ss = strchr(buffer,';');
           if(ss)
			 {
			   ss[0] = 0;
			   WriteToDebugFile( "****************В ф.настройки *.on более 1 строки РАЗМЕЩЕНИЕ_СПО" );
             }
           ParameterSPOLocation = buffer;
         }
    // запросим параметр
	if( getparn( sAdjustments.c_str(), buffer, MAXP) != 0 )
       { // РАЗДЕЛЫ (берутся из ф.настр.".on" строки "РАЗДЕЛЫ=")
           ParameterAdjustments = "";
       }
    else { // запомним параметр
           ParameterAdjustments = buffer;
         }
    // запросим параметр
	if( getparn( sProtocol.c_str(), buffer, MAXP ) != 0 )
      { // ПРОТОКОЛ
        ParameterProtokol = "";
        bParameterProtokolExistsAndGood = false;
      }
    else
     { // запомним параметр
        ParameterProtokol = buffer;
		bParameterProtokolExistsAndGood = DirectoryExists( ParameterProtokol );
     }
	bWorkWithAllPartitionsFromSPO = ParameterAdjustments.IsEmpty();
    // закроем парнас
	if( closeparn() != 0 )
     {
		ErrorMessageBox( "Ошибка при попытке доступа к пользовательскому файлу настройки ПРИСа. Программа будет закрыта", "Ошибка" );
        Application->Terminate();
        return;
	 }
    // попытка открыть парнас
	if( openparn(CfgFileName.c_str(), NULL) != 0 )
	 {
		ErrorMessageBox( "Ошибка при попытке доступа к системному файлу настройки ПРИСа. Программа будет закрыта", "Ошибка" );
        Application->Terminate();
        return;
     }
	getparn( sPFKS.c_str(), buffer, MAXP );
    // запомним параметр
	PFKSFullFileName = buffer;
	size = 12;
// Для ТРАФАРЕТa
	if( getparn( "ТРАФАРЕТ", buffer, MAXP ) == 0 )
	  {
		 size = StrToInt(buffer);
		 if( size == 3 )
		  {
			mask = L"AAA-0000;1;.";
			StencilLength = 8;
		  }
		 if( size == 4 )
		  {
			StencilLength = 9;
			mask = L"AAAA-0000;1;.";
		  }
	   }
	if( size > 4 )
		  {
			 StencilLength = size + 5;
			 for( i = 0; i < size; i++ )
			 mask += L"!<>c";
			 mask += L"-0000;1;.";
		  }
	/* else {
			 ErrorMessageBox( "Неверный параметр трафарета в " + CfgFileName );
			 size = 4;
			 mask = L"AAAA-0000;1;.";
			 StencilLength = 9;
		  }*/
//////////////////////////////////////////////////////////////
// Проверяем, будет ли работа с КТ
	if( ParamCount() == 3 )
     { // работаем с КТ
       // определяем пути для файла сохранения
	   if( getparn("ЛОКАЛЬНОЕ_СОХРАНЕНИЕ", buffer, MAXP ) != 0 );
                                //save_loc = 0;
       else
        { // запомним параметр
          save_loc = AnsiString(buffer) + "\\isr.svg";
        }
	   if( getparn("УДАЛЕННОЕ_СОХРАНЕНИЕ", buffer, MAXP) != 0 );
                                //save_rem = 0;
       else
        { // запомним параметр
          save_rem = AnsiString(buffer) + "\\isr.svg";
        }
	   if( getparn( "ВОССТАНОВЛЕНИЕ", buffer, MAXP ) != 0 )
                                vosst = 0;
       else
        { // запомним параметр
          vosst = AnsiString(buffer) + "\\isr.svg";
        }
	  if( !CompareText( ParamStr(3), "/О" ) || !CompareText( ParamStr(3), "/O" ) ||
		  !CompareText( ParamStr(3), "/С" ) || !CompareText( ParamStr(3), "/C" ) )
        { // будем сохранять КТ
          pr_save = true;
		  WriteToDebugFile( "Сохраняем КТ с параметром " + ParamStr(3) );
        }
      else pr_save = false;
      if( !CompareText(ParamStr(3),"/O")||
            !CompareText(ParamStr(3),"/О")||
                 !CompareText(ParamStr(3),"/В")||
                    !CompareText(ParamStr(3),"/B"))
         { // нужно восстановиться
           WriteToDebugFile("Восстанавливаемся с параметром "+ParamStr(3));
           if(vosst.Length())
                 pr_vosst = true;
            else
			{
			  ErrorMessageBox("Не задан путь для восстановления в ф.конфигурации");
              pr_vosst = false;
            }
		 }
    }

////////////////////////////////////////////////////////////
//             Есть ли синхронизация времени через ИСР
/////////////////////////////////////////////////////////////
if ( getparn("ИСР_РВРЕМЯ", buffer, MAXP) == 0 )
							{
							   r_vrem = true;
                               WriteToDebugFile("Будем выдавать РВРЕМЯ по началу каждого раздела" );
                            }
else
    r_vrem = false;
// закроем парнас
	if( closeparn() != 0 )
	 {
		ErrorMessageBox( "Ошибка при попытке доступа к системному файлу настройки ПРИСа. Программа будет закрыта", "Ошибка" );
		Application->Terminate();
        return;
     }
    SectionsNormal->Checked = true;
    CurrentAdjustmentsList = ParameterAdjustments;
    // обработаем файлы настройки разделов
	ProcessAdjustments();
    // начальный тип работы - шаговый
	CurrentWorkMode = wmStep;
    // имя трафарета
    AnsiString ReadStencil;
    FullStencilFileName = ProgramDir + "\\" + StencilFileName;
    // прочитаем трафарет
	int nRes = ReadStencilFromFile( FullStencilFileName, ReadStencil );
    // перебор по варианту ответа
	switch (nRes)
	 {
	   case (0): bStencilExists = true;      // трафарет прочитан нормально
				 Stencil = ReadStencil;
                 break;

	   case (-1): bStencilExists = false;    // ошибка при открытии файла
				  Stencil = "";
				  break;

	   case (-2): bStencilExists = false;    // ошибка при чтении файла
				  ErrorMessageBox( "Ошибка при чтении трафарета из файла \"" + FullStencilFileName + "\"." );
				  Stencil = "";
				  break;
	 }
    // ни один раздел пока не выбран
    nSelectedPartitionNumber = -1;
	// структура не загружена
    bMainPartitionStructureLoaded = false;
    // раздел не проводится
    ISRCurrentState = isNonPartitionSelected;
    // меню
	ConfigureMenu();
   //	WriteToDebugFile( "1" );
   //	SendDirectoriesToPris();
	for( i = 0; i < MaxPartitionCount; i++ )
     {
        LoadedPartitions[i].nSectionFromISRPartitionOperationMake = -1;
     }
// работа с КТ
		if( pr_vosst )                       // восстанавливаемся
            {
			   if( vosst_KT() == -1 )
				 {
				   ErrorMessageBox( "Не прошло восстановление" );
				   SendStringToPrisProtokol("Не прошло восстановление", stError, true, true, false );
				 }
               else {
						 SendStringToPrisProtokol( "Прошло восстановление из файла " + vosst, stError, true, true, false);
						 WriteToDebugFile( "Восстановились" );
                         PRISLoaded();
                         ConfigureMenu();
                    }
               pr_vosst = 0;
            }
}
//-----------------------------------------------------------------------------
//    Выбрали п. меню "Список разделов"
//-----------------------------------------------------------------------------
void __fastcall TISRMainForm::PreparePartitionsListClick( TObject *Sender )
{
 if( BR == 11 )
   {
	  WarningMessageBox( "Ждем ответ на оп-ю посылки заголовка в ССБВС", "Загрузка раздела" );
	  return;
   }
 // если мы не в последнем разделе
	if( nCurrentPartition < nPartitionCount - 1 )
	 {
		WarningMessageBox( "Вы не можете загрузить раздел, так как Вы находитесь не в текущем разделе", "Загрузка раздела" );
        return;
	 }
	// список разделов
    PartitionListBox->ShowModal();
    // если "Выбрать" раздел
	if ( PartitionListBox->GetPartitionListBoxResult() == prChoose )
     {  // номер выбранного раздела
        nSelectedPartitionNumber = PartitionListBox->GetSelectedPartitionNumber();
        // состояние - раздел выбран
        ISRCurrentState = isPartitionSelected;
        // флаг: у головного раздела есть стуктура
        bMainPartitionStructureExists = !(Partitions[nSelectedPartitionNumber].StructureName.IsEmpty());
        bMainPartitionStructureLoaded = false;
        // перестроим меню
        ConfigureMenu();
    }
    // если "Загрузить" раздел
	if ( PartitionListBox->GetPartitionListBoxResult() == prLoad )
     {  // номер выбранного раздела
        nSelectedPartitionNumber = PartitionListBox->GetSelectedPartitionNumber();
        //bool bb =
        PartitionOperation(Partitions[nSelectedPartitionNumber].Abbr, "", false, false, false);
     }
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::TestTest1Click(TObject *Sender)
{
	 for( int i = 0; i < 10; i++ )
    try {
		  SendStringToPrisProtokol( "Строка в протокол", stStandart, true, true, false );
        }
    catch(...)
	{
	   WriteToDebugFile("Ошибка при посылке строки протокола");}
	   return;
    /////////////////
    /*Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = BintID;
    Por->AddAtomA(1, 5, 0)->Append(AnsiString("PRIMER|2:НШС|АВТ"));
    // пошлем порцию
    Por->Send();   */
 }
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::WorkEndPartitionClick(TObject *Sender)
{
    int i, j;
    int nCurrentPartition_old = -1;  // для запоминания текущего номера раздела при его изменении
    TMDIChild* ch;
    // изначально протокол сохранять не надо
    bSaveProtokol = false;
    // флаг: оканчиваем все разделы
    bool bEndAllPartition = (TMenuItem*)Sender == WorkEndAllPartitions;
	if (bEndAllPartition)
      {
        ////////////////////////////////////
        if( nPartitionCount > 1 ) // если > одного раздела
                      {  // сохраним номер текущего раздела на случай, если нельзя закрыть все разделы по какой-то причине
                         nCurrentPartition_old = nCurrentPartition;
                         nCurrentPartition = nPartitionCount - 1;  // текущим делаем последний
                      }
        ///////////////////////////////////////
        SnapFoto("В начале WorkEndPartitionClick() - все разделы");
      }
    else
      {
        SnapFoto("В начале WorkEndPartitionClick() - один раздел");
      }
    // аббревиатура раздела
    AnsiString PartitionAbbr = LoadedPartitions[nCurrentPartition].Abbr;
    // флаг: загружена структура
    bool bStructureLoaded = !(nCurrentPartition == 0 && !bMainPartitionStructureLoaded);
    // флаг: ПРИС выполняет операцию
    bool bPrisExecutesOperation = false;
    bool bGood = true;
    // флаг: надо ли показывать окно "Конец раздела"
    bool bShowEndBox;
    // если выполняется блок "Авто" в тек. разделе
	if( IsBlockAutoExecuted() )
	 {
		AnsiString sMessage = "Вы не можете загрузить раздел, так как Вы находитесь не в текущем разделе \"" + GetPartitionTitle(nCurrentPartition)
					  + "\". Нельзя завершить раздел";
		WarningMessageBox( sMessage, "Завершение раздела" );
		if( bEndAllPartition )
			if( nCurrentPartition_old != -1)
				  nCurrentPartition = nCurrentPartition_old;    // восстановим номер текущего раздела
		bGood = false;
	 }
	// если имеет смысл продолжать
	if( bGood )                                                 // если оканчиваем все разделы
	 {
	   if( bEndAllPartition )
		{
		  if( nOperationInPrisCount > 0 )
		   {
			  //WarningMessageBox( "Нельзя завершить разделы так как ПРИС обрабатывает операцию (операции) из раздела (разделов)", Stringconst_sPartitionClose );
			  if( bEndAllPartition )
				  if( nCurrentPartition_old != -1)
					 nCurrentPartition = nCurrentPartition_old; // восстановим номер текущего раздела
				bGood = false;
		   }
		}
	   else {
			  if( bStructureLoaded )                            // если загружена структура
			   {
				 for( int i = 0; i < 3; i++ )                    // цикл по секциям раздела
				 {
					ch = LoadedPartitions[nCurrentPartition].pWindow[i];
                    // если в секции есть команда, ушедшая в ПРИС
					if( ch->GetPrisExecutesOperation() )
					{
					  bPrisExecutesOperation = true;
                      break;
                    }
                 }
                // если ПРИС выполняет операцию из данного раздела
				if( bPrisExecutesOperation )
                 {
				   if( bEndAllPartition )
						if( nCurrentPartition_old != -1 )
                                nCurrentPartition = nCurrentPartition_old;  // восстановим номер текущего раздела
					   bGood = false;
				 }
               }
             }
	}
 // если имеет смысл продолжать
   if( bGood )
	{
	  if( bEndAllPartition )
	   {
		  bShowEndBox = bMainPartitionLoadedFromISR;
	   }
	  else
	   { // если мы закрываем головной раздел и он загружен из ИСР
			bShowEndBox = nCurrentPartition == 0 && bMainPartitionLoadedFromISR;
	   }
		// если не пришло сообщние "Конец раздела"
	  if (!bEndPartitionMessage)
	   { // если надо показать окно "Конец раздела"
		   if( bShowEndBox )
			{
			  EndBox->SetMode(bEndAllPartition);
			  EndBox->ShowModal();                                    // покажем окно "Конец раздела"
			  if( EndBox->Result() == erCancel )                      // если "Отмена"
					bGood = false;
			  else
			   {
				   if( EndBox->Result() == erEndWithSaveProtokol )    // если надо сохранять протокол
					{
					  bSaveProtokol = true;
					  ProtokolName = EndBox->GetProtokolName();
					  char* sp = strpbrk( ProtokolName.c_str(), " \\*?:/|" );
					  if(sp)
					   {
						 ErrorMessageBox( "Недопустимый символ в имени файла" );
						 bSaveProtokol = false;
						 bGood = false;
					   }
					}
			   }
			}
		else
		 {
		   // запрос на завершение раздела
		   int nAnswer = QuestionMessageBox( "Завершить раздел \"" + GetPartitionTitle(nCurrentPartition) + "\"?");
		   if( nAnswer == IDCANCEL )                                // если "Отмена"
			  bGood = false;
		 }
	   }
	}
	// если имеет смысл продолжать
   if( bGood )
	{
	  SendEndPartitionToPris();              // посылаем конец раздела в ПРИС
	  ConfigureMenu();
	  // "Выход из разделов"
	  if( bEndAllPartition )
			bEndAllPartitions = true;
	}
   SnapFoto( "В конце WorkEndPartitionClick()" );
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::TestTest2Click( TObject *Sender )
{
	SendDirectoriesToPris();
}
//---------------------------------------------------------------------------
// обработка пришедшей директивы на запуск раздела
// результат - загружен ли раздел
//---------------------------------------------------------------------------
bool __fastcall TISRMainForm::PartitionOperation( AnsiString PartitionAbbr, AnsiString Label, bool bAutoMode, bool bFromPris, bool bISRStructureOperation, AnsiString OperationText)
{
    int i;
	bool bLocalAutoMode = bAutoMode;
    AnsiString ErrorMessage, LocalErrorMessage;
    AnsiString StrToProtokol, StrToProtokol2;
    bool bGood = true;
    int PartitionNumber;
    // разбор метки
    int nCurrentSection = 0, nCurrentNShSBlock = 0;
    int nLabelLength = Label.Length();
    bool bGoodLabel = false;
    bool bLabelExists = false;
	if( PartitionAbbr == "pppp" )
				 goto P;
    // если кол-во рагруженных разделов больше или равно максимально
    // возможному кол-ву
	if ( nPartitionCount >= MaxPartitionCount )
     {
		ErrorMessage = "количество загруженных разделов равно максимально возможному";
        StrToProtokol = ErrorMessage;
        bGood = false;
     }
    // если имеет смысл продолжать
    if (bGood)
	{
		bool bPartitionLoaded = false;
        // цикл по загруженным разделам
		for( int i = 0; i < nPartitionCount; i++ )
		 {
			// если раздел с данной аббревиатурой уже загружен
			if( PartitionAbbr == LoadedPartitions[i].Abbr )
			 {
				bPartitionLoaded = true;
                break;
             }
         }
		// если раздел уже загружен
		if( bPartitionLoaded )
		 {
			ErrorMessage = "раздел уже загружен";
            StrToProtokol = ErrorMessage;
            bGood = false;
		 }
    }
    // если имеет смысл продолжать
	if( bGood )
      { // номер раздела
		PartitionNumber = GetPartitionNumber( PartitionAbbr );
        if (PartitionNumber == -1)
         {
			ErrorMessage = "раздел отсутвует в списке разделов";
            StrToProtokol = ErrorMessage;
            bGood = false;
         }
      }
    // если имеет смысл продолжать
	if( bGood )
	 {
	   // если у раздела нет структуры
	   if( Partitions[PartitionNumber].StructureName.IsEmpty() )
        {
			ErrorMessage = "у раздела штатно отсутствует файл структуры";
            StrToProtokol = ErrorMessage;
            bGood = false;
        }
    }
    // если имеет смысл продолжать
	if( bGood )
	 {
	   if( Label != "" )
		{
			// если есть метка
            bLabelExists = true;
            //если метка штатного проведения раздела
			if( Label == NormalWorkLabel )
				   {
					  nCurrentSection = 0;
                      bGoodLabel = true;
                   }
            else
              { // если метка секции 3 (приведение в исходное)
				if( Label == GoInitialLabel )
				   {
					 nCurrentSection = 2;
					 bGoodLabel = true;
				   }
				else
				{ // тогда наверное метка "НШС"
					if( Label == NShSWorkLabel )
					 {  // если метка секции 2 НШС
						nCurrentSection = 1;
						bGoodLabel = true;
					 }
					else
					 { // наверно метка блока НШС
						// если конец метки правильный
						if (Label.SubString(nLabelLength - 2, NShSLabel.Length()) == NShSLabel) {
							// если начало метки - правильное
							if (Label[1] == cNShSLabelSymbol)
							 {
							   nCurrentSection = 1;
							   nCurrentNShSBlock = StrToInt(Label.SubString(2, nLabelLength - NShSLabel.Length() - 1));
							   bGoodLabel = true;
							 }
					   }
					 }
				}
        }
            // если некорректная метка
			if( !bGoodLabel )
			 {
				LocalErrorMessage = "Пришла директива \"" + PartitionStartDirName + " "
								   + PartitionAbbr + "\" с некорректной меткой - \"" + Label + "\".";
				ErrorMessageBox( LocalErrorMessage );
                nCurrentSection = 0;
                bLocalAutoMode = false;
			 }
    }
		nPartitionCount++;
		WriteToDebugFile( "Начат раздел " + PartitionAbbr );
        nCurrentPartition = nPartitionCount - 1;
        LoadedPartitions[nPartitionCount - 1] = Partitions[PartitionNumber];

        // уберем диалоговые окна, если есть
		for( i = 0; i < DialogBoxCount; i++ )
		 {
            try {
				 SendMessage( apDialogBox[i], WM_CLOSE, 0, 0 );
                }
            catch(...){;}
		 }
        // если загрузили головной раздел
		if( nPartitionCount == 1 )
		 {
            bMainPartitionLoadedFromISR = !bFromPris;
		 }
        // загрузим раздел
		if( !LoadStructure( true, StrToProtokol, ErrorMessage, nCurrentSection ) )
		 {
            bGood = false;
            nPartitionCount--;
            nCurrentPartition = nPartitionCount - 1;
		 }
    }

    // если все хорошо
    if (bGood)
      { // если головной раздел
		if ( nCurrentPartition == 0 )
                   PartitionStartTickCount = GetTickCount();
        if (bFromPris)
           { // ответ ПРИСу - начать раздел
			 SendAnswerToPartitionOperationToPris( 1, PartitionAbbr );
           }
        else
           { // начало раздела - в ПРИС
			 SendBeginPartitionToPris(PartitionAbbr);
             if(!pr_vosst)  // если не восстанавливаемся
			 {
				TM->Interval = 5000; // ждем  ответ
                TM->Enabled = true;
				BR = 10; // загружаемся без заголовка
                return (true);
             }
             // если операция из структуры
  P:        if( bISRStructureOperation )
             {  // пошлем в протокол операцию ИСР
				SendStringToPrisProtokol( OperationText, stStandart, false, true, false );
             }
           }
        // если загрузили головной раздел
		if( nPartitionCount == 1)
         { // флаги
            bMainPartitionStructureExists = true;
            bMainPartitionStructureLoaded = true;
            // состояние ИСР
            ISRCurrentState = isPartitionCarringOut;
            MainPartitionTitle = PartitionAbbr;
			// пошлем заголовк окна в ПРИС
			if( !pr_vosst ) // если не восстанавливаемся
                    SendWindowTitleToPris();
         }
		// пошлем директории в ПРИС
		SendDirectoriesToPris();
        // выведем КС настройки в протокол
        PrintAdjstmentKSToProt();
        anCurrentSection[nCurrentPartition] = nCurrentSection;
        TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[nCurrentSection];
        // если секция НШС и есть метка
		if( bLabelExists && nCurrentSection == 1 && nCurrentNShSBlock != 0)
		 {
			// перейдем к ней
			int nResult = ch->SetNshSBlock(nCurrentNShSBlock);
			// если ошибка
			if (nResult < 0) {
				switch (nResult) {
					// не найдена метка НШС
					case (-2):
						LocalErrorMessage = "Пришла директива \"" + PartitionStartDirName + " "
											+ PartitionAbbr + "\" с переходом в секцию НШС в блок - \""
											+ Label + "\". Данный блок не найден";
						break;
					// в блоке НШС нет операций
					case (-3):
						LocalErrorMessage = "Пришла директива \"" + PartitionStartDirName + " "
											+ PartitionAbbr + "\" с переходом в секцию НШС в блок - \""
											+ Label + "\". В данном блоке не найдено ни одной операции. Невозможно встать на данный блок";
						break;
				}
				ErrorMessageBox(LocalErrorMessage);
				bLocalAutoMode = false;
			}
		 }
      ConfigureMenu();
		// если работаем в автоматическом режиме
		if( bLocalAutoMode )
		  {
			ch->WorkInAutoMode();         // запустим режим "Авто"
          }
        return (true);
    }
	// что-то не так
	else
	  {
		// ErrorMessage и StrToProtokol приходят без точки
		StrToProtokol2 = "Раздел не загружен. Причина: " + StrToProtokol + ".";
		// если операция из ПРИСа
		if( bFromPris )
		 {
		   // ответ ПРИСу - не начинать раздел
		   SendAnswerToPartitionOperationToPris(0);
           // пошлем в протокол текст ошибки
		   SendStringToPrisProtokol( StrToProtokol2, stError, true, true, false );
		   ErrorMessage = "В ИСР из ПРИС пришла директива \""+ OperationText + "\". Раздел не загружен. Причина: " + ErrorMessage + ".";
         }
        // внутренняя операция ИСР
		else
		 { // если операция из структуры
			if( bISRStructureOperation )
             { // пошлем в протокол операцию ИСР
				SendStringToPrisProtokol( OperationText, stError, false, true, false );
                // пошлем в протокол текст ошибки
				SendStringToPrisProtokol( StrToProtokol2, stError, false, false, true );
				ErrorMessage = "В данный момент интерпретируется внутренняя директива ИСР \""+ OperationText + "\". Раздел не загружен. Причина: " + ErrorMessage + ".";
             }
			// загрузить через меню
            else
			  {
				ErrorMessage = "Раздел не загружен. Причина: " + ErrorMessage + ".";
              }
         }
		ErrorMessageBox( ErrorMessage, "Загрузка раздела" );
        return (false);
      }
}

void __fastcall TISRMainForm::TestTest3Click(TObject *Sender)
{
   /* Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = BintID;
    Por->AddAtomA(1, 5, 0)->AppendBuf("ЗО_КИС_КА1", 10);
    // пошлем порцию
	Por->Send();  */
}

void __fastcall TISRMainForm::PrepareStencilClick(TObject *Sender)
{
	StencilBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::TestTest5Click(TObject *Sender)
{
    SendWindowTitleToPris();
}
//---------------------------------------------------------------------------
// "Работа" - "Переключиться к активной секции"
void __fastcall TISRMainForm::WorkGoToActiveSectionClick(TObject *Sender)
{
    GoToActiveSection();
}
// Переключиться к активной секции
void __fastcall TISRMainForm::GoToActiveSection()
{
    int nPartitionNumber = aOperationInPris[nOperationInPrisCount - 1].nPartitionNumber;
    int nSectionNumber = aOperationInPris[nOperationInPrisCount - 1].nSectionNumber;
    SetTopSection(nSectionNumber, nPartitionNumber);
}
// "Вернуть" окно на место
void __fastcall TISRMainForm::WorkReturnWindowClick(TObject *Sender)
{
    // положение окна
    Left = nMainWindowLeft;
    Top = nMainWindowTop;
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::TestTest4Click(TObject *Sender)
{
   /*/ Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = BintID;
    Por->AddAtomA(1, 5, 0)->AppendBuf("ОС_ВКЛ_СОСБ1", 12);
    // пошлем порцию
	Por->Send();  */
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::OnException( TObject *Sender, Exception *E )
{
	if( E->Message == "Out of memory" )
	 {
		ErrorMessageBox( "Произошла критическая ошибка (нехватка памяти). Программа будет закрыта", "Ошибка" );
		exit(EXIT_FAILURE);
     }
    // если некорректный ввод значения в TMaskEdit
	if( E->Message == "Invalid input value.  Use escape key to abandon changes" )
	 {
		WarningMessageBox( "Введенное значение некорректно. За информацией обращайтесь к эксплуатационной документации", "Ошибка" );
		return;
     }
	// сообщение
	AnsiString Message = "В программе произошла внутренняя ошибка. Информация о ней будет сохранена в файле \"" + ErrFileName + "\".";
	// выведем сообщение в файл "Error.txt"
    WriteToErrorFile(E->Message);
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::TestTest6Click(TObject *Sender)
{
    // ответ от ПРИС на КО
   /* Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = AportID;
    Por->AddAtomA(1, 2, 0);
	Por->Send(); */
}
void __fastcall TISRMainForm::WorkSPClick(TObject *Sender)
{
    nR = 8;  // СП
    SPBox->ShowModal();
}
//---------------------------------------------------------------------------
// Начать/окончить отметку блока "Авто"
void __fastcall TISRMainForm::AutoBeginFinishBlockClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->BeginFinishBlock();
}
//---------------------------------------------------------------------------


void __fastcall TISRMainForm::AutoClearSelectionClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->ClearSelection();
}
//---------------------------------------------------------------------------
// нажата клавиша в child-окне
void __fastcall TISRMainForm::KeyPressedInChildWindow(KeysPressed Key)
{
    switch (Key) {
        case (kpF7):
            AutoBeginFinishBlockClick(NULL);
            break;
        case (kpF9):
            AutoClearSelectionClick(NULL);
            break;
    }
}



void __fastcall TISRMainForm::AutoMakeAutoClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->ExecuteAutoBlock();
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::AutoStopAutoBlockClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->StopAutoBlock();
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::TestTest7Click(TObject *Sender)
{
    // ответ от ПРИС на кразд
  /*  Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = AportID;
    Por->AddAtomA(1, 6, 0);
	Por->Send();  */
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::TestTest8Click(TObject *Sender)
{
    // ответ от ПРИС на КО
  /*  Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = AportID;
    Por->AddAtomA(1, 7, 0);
	Por->Send();   */
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::TestTest10Click(TObject *Sender)
{
    // ответ от ПРИС на КО
  /*	Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = AportID;
    Por->AddAtomA(1, 2, 1);
    Por->Send(); */
}
//---------------------------------------------------------------------------
// "Перестроить список разделов"
void __fastcall TISRMainForm::PrepareRebuildListClick(TObject *Sender)
{
    // восстановим список файлов настройки РЭП
    CurrentAdjustmentsList = ParameterAdjustments;
    // перестроим разделы
    RebuildPartitionsList();
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::RebuildPartitionsList()
{
    // обработаем настройки
    ProcessAdjustments();
//             Формирование списка разделов РЭП
/////////////////////////////////////////////////////////////
  FormPartition_REP();
  if( PartisionRep->ListBox1->Items->Count == 0)
    { // разделы РЭП не заданы, погасим пункты меню
       //N13->Enabled = false ;
       ToolButton7->Enabled = false;
       ToolButton3->Enabled = false;
	   //N6->Enabled = false;
    }
////////////////////////////////////////////////////////////
    // директории в ПРИС
	SendDirectoriesToPris();
}
//------------------------------------------------------------------------------
// "Добавить разделы в список разделов..."
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareAddPartitionsClick(TObject *Sender)
{
	if( bWorkWithAllPartitionsFromSPO )
	 {
		WarningMessageBox( "Список разделов построен из ВСЕХ разделов, СПО которых находится в каталоге СПО. Поэтому добавить раздел в список разделов невозможно. Если вы добавили СПО раздела в каталог СПО и хотите, чтобы этот раздел оказался в списке разделов, выберите пункт 'Перестроить список разделов'" );
		return;
	 }
	AnsiString Index;
	// настроим начальную директорию
	if( ParameterAdjustmentsLocation != "" )
	 {
        OpenDialog->InitialDir = ParameterAdjustmentsLocation;
		if( OpenDialog->Execute() )
		 {
			AnsiString currentPath = ExtractFilePath( OpenDialog->FileName );
            AnsiString setFilePath;
			if( ParameterAdjustmentsLocation[ParameterAdjustmentsLocation.Length()] == '\\' )
			 {
                setFilePath = ParameterAdjustmentsLocation;
			 }
			else
			 {
                setFilePath = ParameterAdjustmentsLocation + "\\";
			 }
			if( currentPath.AnsiCompareIC(setFilePath) != 0 )
			 {
				ErrorMessageBox( "Файл настройки разделов должен располагаться в каталоге, определенном параметром 'РАЗМЕЩЕНИЕ_РАЗДЕЛОВ' файла настройки пользователя" );
                return;
			 }
            // получим индекс файла настройки
			Index = ExtractFileName( OpenDialog->FileName );
			Index = Index.SubString( 1, Index.Length() - 4 );
            CurrentAdjustmentsList += ";" + Index;
		 }
	}
  RebuildPartitionsList();
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::WorkPartitionLengthClick(TObject *Sender)
{
    // текущее кол-во тиков
    unsigned int CurrentTickCount = GetTickCount();
    // тиков в разделе
    unsigned int PartitionTickCount;
    if (CurrentTickCount > PartitionStartTickCount)
     {
	   PartitionTickCount = CurrentTickCount - PartitionStartTickCount;
	 }
	else
	 {
		PartitionTickCount = (0xFFFFFFFF - PartitionStartTickCount) + CurrentTickCount;
	 }
    // кол-во целых секунд
    int nSeconds = PartitionTickCount / 1000;
    // кол-во целых минут
    int nMinutes = nSeconds / 60;
    // остаток секунд
	int nShowSeconds = nSeconds % 60;
    // кол-во часов
    int Hours = nMinutes / 60;
	// остаток минут
    int nShowMinutes = nMinutes % 60;
	AnsiString TimeMessage = "Продолжительность раздела - " + StringWithLeadZero2(Hours) + ":" + StringWithLeadZero2(nShowMinutes) + ":" + StringWithLeadZero2(nShowSeconds);
    if(Sender==0) // печатаем по концу раздела в протокол ПРИС
	   SendStringToPrisProtokol(TimeMessage, stStandart, true, true, false);
    else  // выдаем в окно по запросу из меню
       InformationMessageBox(TimeMessage);
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::WorkMakeOperationClick(TObject *Sender)
{
    TMDIChild* ch = LoadedPartitions[nCurrentPartition].pWindow[anCurrentSection[nCurrentPartition]];
    ch->MakeOperation();
}
//---------------------------------------------------------------------------
// получена операция начала раздела
void __fastcall TISRMainForm::PreProcessPartitionOperation(AnsiString Parameters)
{
    AnsiString InternalParameters = Parameters;
    // заменим разделители на пробелы
	for (int i = 1; i <= InternalParameters.Length(); i++)
     {
       if (InternalParameters[i] == cPartitionDirFieldDelimiter)
         {
           InternalParameters[i] = ' ';
         }
    }
    InternalParameters = InternalParameters.Trim();
    SetMainWindowOnTop();
    RecievePartitionOperation(InternalParameters, true, PartitionStartDirName + " " + InternalParameters);
}
//---------------------------------------------------------------------------
// получена операция начала раздела
// принимает строку параметров операции "Запуск раздела"
// разделитель параметров - один пробел
// результат - загружен ли раздел
bool __fastcall TISRMainForm::RecievePartitionOperation(AnsiString Parameters, bool bFromPris, AnsiString OperationText)
{
    // позиции разделителей
    int anDelimiterPos[2];
    int nDelimiterCount = 0;
    int nParametersLength = Parameters.Length();
    int i;
	bool bGood = true;
    AnsiString ErrorMessage;
    AnsiString PartitionAbbr, Label, Mode;
    AnsiString StrToProtokol, StrToProtokol2;
    bool bAuto;
    bool bPrintCommonError = true;

  // если отладка раздела
  if (bPartitionDebug)
   {
     bGood = false;
	 ErrorMessage = "Сейчас идет отладка раздела.Во время отладки раздела нельзя загружать вложенные разделы. Если вы хотите, чтобы загружался вложенный раздел, не используйте отладку раздела. Настройте ПРИС так, чтобы в списке разделов ИСР оказались и головной, и вложенный разделы";
	 if( bFromPris )
	  {
		ErrorMessage = "В ИСР из ПРИС пришла директива \"" + OperationText +"\". " + ErrorMessage;
      }
     else
	  {
		ErrorMessage = "В данный момент интерпретируется внутренняя директива ИСР \"" + OperationText +"\". " + ErrorMessage;
      }
	 StrToProtokol = "Нельзя загружать вложенные разделы во время отладки раздела";
     bPrintCommonError = false;
   }
  // если имеет смысл продолжать
  if( bGood )
   { // если открыт диалог открытия файла
	 if( bOpenFileDialogOpen )
      {
        bGood = false;
		bPrintCommonError = false;
		if( bFromPris )
		 {
		   ErrorMessage = "В ИСР из ПРИС пришла директива \"" + OperationText + "\". В данный момент загрузить раздел нельзя, так как открыт диалог выбора файла";
         }
		else
         {
		   ErrorMessage = "В данный момент интерпретируется внутренняя директива ИСР \"" + OperationText + "\". Загрузить раздел нельзя, так как открыт диалог выбора файла";
         }
		StrToProtokol = AppTitle + " не может выполнить директиву, так как открыт диалог выбора файла";
      }
   }
 // если имеет смысл продолжать
 if( bGood )
  {
	// если длина строки равна 0
	if( nParametersLength == 0 )
     {
       bGood = false;
	   ErrorMessage = "отсутствуют параметры директивы";
       StrToProtokol = ErrorMessage;
     }
  }
 // если имеет смысл продолжать
  if( bGood )
   {
	 // если разделов нет
	 if( nPartitionCount == 0 )
      {
        bGood = false;
		ErrorMessage = "нельзя загрузить головной раздел из ПРИС. Головной раздел можно загрузить только из " + AppTitle;
        StrToProtokol = ErrorMessage;
      }
   }
 // если имеет смысл продолжать
  if( bGood )
   {
	 // цикл по строке
	 for( i = 1; i <= nParametersLength; i++ )
	  {
		// если символ подходит
		if( !IsGoodSymbolForPartitionOperationParameters(Parameters[i]) )
          {
            bGood = false;
			ErrorMessage = "недопустимый символ в строке параметров директивы. Номер символа - " + IntToStr(i) + " (начиная с первого символа строки параметров)";
			StrToProtokol = ErrorMessage;
            break;
          }
      }
   }
  // если имеет смысл продолжать
  if( bGood )
   {
	 // флаг: мы сейчас в лексеме
	 bool bInLexeme = false;
     // кол-во лексем
     int nLexemeCount = 0;
     // цикл по строке
	 for( i = 1; i <= nParametersLength; i++ )
	  {
		// если не пробел
		if( Parameters[i] != ' ' )
		 {
		   // если мы не в лексеме
		   if( !bInLexeme )
			{
			  bInLexeme = true;              // установим, что мы в лексеме
			  nLexemeCount++;
            }
         }
        // пробел
        else
		 { // если мы в лексеме
		   if( bInLexeme )
			 {
			   bInLexeme = false;            // установим, что мы не в лексеме
             }
         }
      }
     // если параметров нет
	 if( nLexemeCount == 0 )
      {
        bGood = false;
		ErrorMessage = "отсутствуют параметры директивы";
        StrToProtokol = ErrorMessage;
      }
     else
      { // если параметров слишком много
		if( nLexemeCount > 3 )
         {
           bGood = false;
		   ErrorMessage = "у директивы слишком много параметров: " + IntToStr(nLexemeCount);
           StrToProtokol = ErrorMessage;
         }
      }
  }
 // если имеет смысл продолжать
  if( bGood )
	{ // цикл по строке
	  for( i = 1; i <= nParametersLength; i++ )
       { // если разделитель
         if (Parameters[i] == ' ')
		  {
			nDelimiterCount++;
			if (nDelimiterCount > 2)
			 {
			   bGood = false;
			   ErrorMessage = "разделителей параметров больше 2-х";
			   StrToProtokol = ErrorMessage;
			   break;
			 }
			anDelimiterPos[nDelimiterCount - 1] = i;
		  }
       }
    }
 // если имеет смысл продолжать
 if( bGood )
    {
      AnsiString SingleParameter;
	  switch( nDelimiterCount )
       {
         case (0): PartitionAbbr = Parameters;
                   break;
		 case (1): PartitionAbbr = Parameters.SubString( 1, anDelimiterPos[0] - 1 );
				   SingleParameter = Parameters.SubString( anDelimiterPos[0] + 1, nParametersLength - anDelimiterPos[0]);
				   if( SingleParameter == AutoStartPartitionLabel || SingleParameter == StepStartPartitionLabel )
                    {
                      Mode = SingleParameter;
                    }
                   else {
                           Label = SingleParameter;
                        }
                   break;
         case (2):
                PartitionAbbr = Parameters.SubString(1, anDelimiterPos[0] - 1);
                Label = Parameters.SubString(anDelimiterPos[0] + 1, anDelimiterPos[1] - anDelimiterPos[0] - 1);
                Mode = Parameters.SubString(anDelimiterPos[1] + 1, nParametersLength - anDelimiterPos[1]);
                break;
       }
	if( Mode.AnsiCompare(StepStartPartitionLabel) == 0 || Mode.IsEmpty() )
      {
        bAuto = false;
      }
    else
     {
	   if( Mode.AnsiCompare( AutoStartPartitionLabel ) == 0 )
         {
           bAuto = true;
         }
       else
         {
           bGood = false;
		   ErrorMessage = "третий параметр не \"" + AutoStartPartitionLabel + "\" и не" + " \"" + StepStartPartitionLabel + "\"";
           StrToProtokol = ErrorMessage;
         }
	 }
 }
 // если ошибок нет
  if( bGood )
   {
	 return ( PartitionOperation( PartitionAbbr, Label, bAuto, bFromPris, true, OperationText ) );
   }
  else
   {  // ErrorMessage и StrToProtokol приходят без точки
	  StrToProtokol2 = "Раздел не загружен. Причина: " + StrToProtokol + ".";
	  // если операция из ПРИСа
	  if( bFromPris )
		{
		  // ответ ПРИСу - не начинать раздел
          SendAnswerToPartitionOperationToPris(0);
		  // пошлем в протокол текст ошибки
		  SendStringToPrisProtokol( StrToProtokol2, stError, true, true, false );
        }
      // внутренняя операция ИСР
      else
		{
		  // пошлем в протокол операцию ИСР
		  SendStringToPrisProtokol( OperationText, stError, false, true, false );
          // пошлем в протокол текст ошибки
		  SendStringToPrisProtokol( StrToProtokol2, stError, false, false, true );
        }
     AnsiString Mess;
	 if( bPrintCommonError )
      {
		if( bFromPris )
         {
		   Mess = "В ИСР из ПРИС пришла директива \"" + OperationText + "\". В ней обнаружена ошибка: "  + ErrorMessage + ".";
         }
		else
         {
		   Mess = "В данный момент интерпретируется внутренняя директива ИСР \"" + OperationText + "\". В ней обнаружена ошибка: "  + ErrorMessage + ".";
         }
	  }
	 else {
               Mess = ErrorMessage + ".";
          }
	ErrorMessageBox( Mess, "Запуск раздела" );
	return (false);
  }
}
//------------------------------------------------------------------------------
// обработчик сообщения "Конец раздела"
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::OnEndPartition(TMessage& Message)
{
    // взведем флаг
    bEndPartitionMessage = true;
    WorkEndPartitionClick(WorkEndPartition);
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PreparePFKSClick(TObject *Sender)
{
	String Abbr = LoadedPartitions[0].Abbr;
	String AdjustmentName = LoadedPartitions[0].AdjustmentName;
	String CommandLine = PFKSFullFileName + L" /ISR " + Abbr + L" " + AdjustmentName + L" " + ParameterSPOLocation;
	// запуск дочернего процесса
	STARTUPINFO StartUpInfo;
	memset( &StartUpInfo, 0, sizeof(STARTUPINFO));
	StartUpInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION ProcessInfo;
	if (::CreateProcess( NULL, CommandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &StartUpInfo, &ProcessInfo) == 0)
	 {
	   String s = "";
	   if( PFKSFullFileName.IsEmpty() )
		  s = "ИСР: Нет ПФКС в файле *.cfg";
		ErrorMessageBox( "Ошибка при вызове ПФКС " + s );
	 }
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareDebugPartitionClick(TObject *Sender)
{
    bOpenFileDialogOpen = true;
    // запросим имя файла
    bool bResult = OpenDialog->Execute();
    bOpenFileDialogOpen = false;
	if( bResult )
	 {
        // настроим параметры
        bPartitionDebug = true;
        DebugAdjustment = OpenDialog->FileName;
        DebugDir = ExtractFilePath(DebugAdjustment);
		DebugDir = DebugDir.SubString( 1, DebugDir.Length() - 1 );
        // перестроим список разделов
		ProcessAdjustments();
        // если раздел не один
		if( nTotalPartitionCount != 1 )
		 {
			ErrorMessageBox( "Отладка с данным файлом настройки не может быть выполнена, т.к. количество разделов в нем не равно 1" );
            bPartitionDebug = false;
            // перестроим список разделов
            ProcessAdjustments();
            return;
		 }
        // номер выбранного раздела
        nSelectedPartitionNumber = 0;
        // состояние - раздел выбран
        ISRCurrentState = isPartitionSelected;
        // флаг: у головного раздела есть стуктура
        bMainPartitionStructureExists = !(Partitions[nSelectedPartitionNumber].StructureName.IsEmpty());
        bMainPartitionStructureLoaded = false;
        // перестроим меню
        ConfigureMenu();
    }
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareCheckSPOClick(TObject *Sender)
{
    CheckSPOBox->Prepend();
    CheckSPOBox->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::WorkOptionsClick(TObject *Sender)
{
    SettingBox->ShowModal();
}
//---------------------------------------------------------------------------
// функция заканчивает текущий раздел
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::EndCurrentPartition(bool u)
{
    int i;
    TMDIChild* ch;
    AnsiString StringToProtokol;
    // флаг: загружена структура
    bool bStructureLoaded = !(nCurrentPartition == 0 && !bMainPartitionStructureLoaded);
	if(u)
	   goto PPP;
    // если надо закрывать секции
	if( bStructureLoaded )
	 {
	   // закроем секции
	   for( i = 0; i < 3; i++ )
		{
			ch = LoadedPartitions[nCurrentPartition].pWindow[i];
            ch->Close();
            //  пока нет окон для загруженных разделов
            LoadedPartitions[nCurrentPartition].pWindow[i] = 0;
        }
	 }
	WriteToDebugFile( "Окончен раздел " + GetPartitionTitle(nCurrentPartition) );
    // кол-во разделов
	if( nPartitionCount )
		   nPartitionCount--;
    // текущий раздел
	if( nCurrentPartition )
		   nCurrentPartition--;
	// если разделов больше нет
	if( nPartitionCount == 0 )
	 {
	   //WorkPartitionLengthClick(0);    // печатаем продолжительность раздела
       // режим работы
       ISRCurrentState = isNonPartitionSelected;
	   if( nR != -1 )
		 {
			BR = 11;
			SendWindowTitleToPris();
			return;       // уходим, чтобы вернуться после получения ответа на ЗАГОЛОВ
		 }
	 }
    // разделы есть
    else
	 {
		// текущий раздел
        LoadedPartitionRecord* pCurrentPartition;
		if( nCurrentPartition >= 0 )
			  pCurrentPartition = &LoadedPartitions[nCurrentPartition];
		else
			  return;
        int nSectionFromISRPartitionOperationMake = pCurrentPartition->nSectionFromISRPartitionOperationMake;
        // если из текущего раздела был вызван закрывшийся раздел по команде ИСР
		if( nSectionFromISRPartitionOperationMake != -1 )
		 {
            ch = pCurrentPartition->pWindow[nSectionFromISRPartitionOperationMake];
            // сбросим флаг того, что из секции был вызван раздел по команде ИСР
            ch->SetISRPartitionOperationProcess(false);
			// операция выполнена
            ch->SetMadeOperation();
            // сообщение в протокол
			StringToProtokol = "Конец внутренней директивы (" + ch->GetCommentToCurrentOperation() + ")";
			SendStringToPrisProtokol( StringToProtokol, stStandart, true, true, false );
            // запомним ее номер
            ch->SetLastMadeOperationOnCurrentOperation();
			// перейдем к следующей операции
            ch->GoNextOperation(true);
            // если был выбран вариант
			if( ch->GetSelectedVariant() )
              {
                ch->SetSelectedVariant(false);
              }
            // если мы в процессе завершения всех разделов
			if( bEndAllPartitions )
			 {
			   ch->EndCalledPartition();                                // сообщим немедля дочернему окну
			 }
			else
			{
			   PostMessage( ch->Handle, MESSAGE_END_CALLED_PARTITION, 0, 0 );   // сообщим потом
            }
            // из текущего раздела не вызвано разделов по команде ИСР
            pCurrentPartition->nSectionFromISRPartitionOperationMake = -1;
		 }
        // если есть секции
		if( !( nPartitionCount == 1 && !bMainPartitionStructureLoaded ) )
         {
            SetTopSection(anCurrentSection[nCurrentPartition]);
         }
    }
 PPP: // метка для продолжение после возврата КЗ на ЗАГОЛОВ
	if( !bPartitionDebug )  // если не отладка
		   if( !pr_vosst )  // и не восстанавливаемся
				 SendDirectoriesToPris();
    // если отладка
	if( bPartitionDebug )
	  {
		bPartitionDebug = false;
		RebuildPartitionsList();     // перестроим список
	  }

    bEndPartitionMessage = false;
    ConfigureMenu();
	// если сохранять протокол и разделов больше нет
	if( bSaveProtokol && nPartitionCount == 0 )
     {
		SendKOToPris( "СП " + ProtokolName, false );
     }
     // если идет процесс завершения всех разделов
	if( bEndAllPartitions )
	 {
	   if( nPartitionCount == 0 )             // если разделов больше нет
                     bEndAllPartitions = false;

	   else
		   {
			  SendEndPartitionToPris();       // посылаем конец раздела в ПРИС
			  ConfigureMenu();
           }
	 }
	if( nPartitionCount == 0 )  // если разделов больше нет, удаляем файл с КТ
	   if( pr_save )
		   {
			  if( !save_loc.IsEmpty() )
				  {
					WriteToDebugFile( "Удаляем ф.лок.сохранения" );
					remove( save_loc.c_str() );
				  }
			  if( !save_rem.IsEmpty() )
				  {
					WriteToDebugFile( "Удаляем ф.удал.сохранения" );
					remove( save_rem.c_str() );
				  }
		   }
}
//------------------------------------------------------------------------------
// Отказ от завершения всех разделов
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::RefusalEndPartition()
{
	AnsiString ErrorMessage = "ПРИС ответил отказом на запрос о завершении раздела\""
							  + LoadedPartitions[nCurrentPartition].Abbr
							  + "\". Возможно, в разделе есть незавершенные циклограммы, или ПРИС сейчас не может принимать команды от ИСР. Раздел не завершен";
	ErrorMessageBox( ErrorMessage, "Завершение раздела" );
    // процесс завершения всех разделов кончаем
    bEndAllPartitions = false;
	ConfigureMenu();
}
//------------------------------------------------------------------------------
// Функция вызывается после того, как ПРИС загрузился
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::PRISLoaded()
{
    static int nCount = 0;
    nCount++;
    // если не первый вызов
	if( nCount > 1 )
      {
        return;
      }
	if( bDeveloperWorkPlace )
      {
		HWND hwndPRIS = FindWindow( L"TMainWindow", L"ПРИС. Раздел:    Протокол: ");
		SetWindowLong( hwndPRIS, GWL_STYLE, GetWindowLong(hwndPRIS, GWL_STYLE) | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		SetWindowPos( hwndPRIS, HWND_BOTTOM, 0, 0, 1, 1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
      }
    bPrisLoaded = true;
    ConfigureMenu();
}
//------------------------------------------------------------------------------
void __fastcall TISRMainForm::WorkImitAnswerClick(TObject *Sender)
{
	// ответ от ПРИС на КО
  /*  Por->Clear();
	Por->AddrTo = nISRID;
	Por->AddrFrom = AportID;
	Por->AddAtomA(1, 2, 1);
	Por->Send();   */
}
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::PrepareSupernumeraryExitClick( TObject *Sender )
{
	AnsiString sWarning = "Осторожно!";
	int nChoice = QuestionMessageBox( "Закрыть " + AppTitle + " ?", sWarning.c_str(), MB_YESNO + MB_ICONWARNING);
	if( nChoice == IDYES )
     {
		Application->Terminate();
     }
}
//---------------------------------------------------------------------------
// поместить главное окно ИСР сверху
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::SetMainWindowOnTop()
{
    HWND hCurrWnd;
    int iMyTID;
    int iCurrTID;

    hCurrWnd = ::GetForegroundWindow();
    iMyTID   = GetCurrentThreadId();
    iCurrTID = GetWindowThreadProcessId( hCurrWnd, 0 );

    AttachThreadInput(iMyTID, iCurrTID, TRUE);

    // hWnd - дескриптор окна.
    SetForegroundWindow(Handle);

    AttachThreadInput(iMyTID, iCurrTID, FALSE);
}
//---------------------------------------------------------------------------
// вывод КС частного файла настройки в протокол
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::PrintAdjstmentKSToProt()
{
	if( !LoadedPartitions[nPartitionCount - 1].bIsHEADAdjustment )
	 {
		char pcKS[9];
        AnsiString AdjustmentName = LoadedPartitions[nPartitionCount - 1].AdjustmentName;
		KSCount( AdjustmentName.c_str(), 3, pcKS );
		AnsiString MessageToProt = "Контрольная сумма файла настройки \"" + AdjustmentName + "\" = " + pcKS;

		SendStringToPrisProtokol( MessageToProt, stStandart, true, true, false );
	 }
}
//---------------------------------------------------------------------------
// Функции работы с КТ
// ф-я восстановления с КТ
//---------------------------------------------------------------------------
short  __fastcall TISRMainForm::vosst_KT()
{
  FILE* f;
  char obl[1000]; // область для чтения сохраненных данных
  unsigned short i, j, kol = 0, k = 0;
  char* p = &obl[0];
  AnsiString Abr;
  memset(obl,0,1000);
//////////////////////////////
if(FileExists(vosst))
 {
   if ((f = fopen(vosst.c_str(), "rb"))== NULL)
    {
      ErrorMessageBox(" Не могу открыть файл "+vosst+ " для восстановления");
      return -1;
	}
   WriteToDebugFile("Восстанавливаемся из файла " + vosst);
   rewind(f); // встали в начало файла
   i = filelength(_fileno(f)); // длина файла
   if(i == 0 || i > 1000 )
     { ErrorMessageBox("Ошибка длины файла сохранения isr.cvg");
       goto PP;
	 }
   if( fread( obl, i, 1, f ) < (size_t)i )
      {
        ErrorMessageBox("Не прочитался файл сохранения isr.cvg");
   PP:  fclose(f);
        return -1;
      }
   else
    { fclose(f);
      // восстанавливаем инф-ю
      bool zagr[3]; // массив признаков загрузки окон
      for(i=0;i<3;i++) zagr[i] = p[i];
      p = p+3;
      // текущее состояние ИСР
      ISRCurrentState = (_ISRState)p[0];
      p++;
      // признак наличия структуры у головного раздела
      bMainPartitionStructureExists = p[0];
      p++;
      // флаг: головной раздел загружен из ИСР
      bMainPartitionLoadedFromISR = p[0];
      p++;
      // кол-во загруженных разделов
      kol = ((int*)p)[0];
      p = p+4;
      // номер выбранного раздела
      nSelectedPartitionNumber = ((int*)p)[0];
      p = p+4;
      // восстанавливаем загруженные разделы
      for(i=0;i<kol;i++)
       { //аббревиатура загруж.разделов
         k = StrEnd(p) - p; // длина аббревиатуры
         Abr = AnsiString(p);
         if(zagr[i])LoadedPartitions[i].Abbr = Abr;
         // Заголовок окна
         if(i==0)MainPartitionTitle = Abr; // головной раздел
         // загружаем раздел, если были ли загружены окна
         if(!zagr[i])
           if(PartitionOperation(Abr,"",false,false,false))
                 WriteToDebugFile("Загрузили раздел "+Abr);
           else {
                 WriteToDebugFile("Ошибка при загрузке раздела");
                 return -1;
                }
         else nPartitionCount++; // добавится раздел без загр.окон
         p = p+k+1;
       }
   // номера текущих секций выбранных разделов
      for(i=0;i<3;i++)
       {
           anCurrentSection[i] = ((int*)p)[i];
           // установим текущую секцию загруженных разделов
          if(!zagr[i])
           if(anCurrentSection[i])
              SetTopSection(anCurrentSection[i]);
       }
      p = p+12;
      // восстановим текущие операции
      for(i=0;i<nPartitionCount;i++)
       {
        if(!zagr[i])  // если были окна
         for(j=0;j<3;j++)
         { TMDIChild* pW = LoadedPartitions[i].pWindow[j];
           if(pW)
           {  pW->nCurrentOperation=((int*)p)[0];
              if(pW->nCurrentOperation)
                pW->nLastMadeOperation=((int*)p)[0]-1;
              p = p+4;
              // восстановить массив выполненных операций
              k = ((int*)p)[0]; // размерность массива
              p=p+4;
              if(k>0)
               {
                  if(k>pW->nOperationCount) k = pW->nOperationCount;
                  try { memcpy(pW->pbMadeOperation,p,k);}
                  catch(...)
                  {
                    WriteToDebugFile("Ошибка при пересылке pbMadeOperation");
                    return -1;
                  }
               }
              p=p+k;
              TGridDrawState State;
              TRect rec;
              try { pW->GridDrawCell(0,0,1,rec,State);}
              catch(...)
               {  WriteToDebugFile("Ошибка при перерисовке окна секции (GridDrawCell)");
                  return -1;
               }
             // признак выполнения операции ПРИСом
             pW->bPrisExecutesOperation = p[0];
             p++;
             // номер выполняемой операции
             pW->nExecutesOperationNumber = ((int*)p)[0];
             p = p+4;
             // комментарий к текущей операции
             k = StrEnd(p) - p; // длина имени комментария
             pW->CommentToCurrentOperation = AnsiString(p);
             p= p+k+1;
             // операции блока Auto
             pW->nAutoFirst = ((int*)p)[0];  // первая
             p = p+4;
             pW->nAutoLast = ((int*)p)[0];  //последняя
             p = p+4;
             // признак выполнения блока Auto
             pW->bExecuteAutoBlock = p[0];
             p++;
             // комментарий к текущей операции
             //pW->CommentToCurrentOperation = pW->GetCurrentPosition(pW->pnOperationStart[pW->nCurrentOperation], pW->pnOperationEnd[pW->nCurrentOperation]);
             // признак выбора варианта
             pW->bSelectedVariant = p[0];
             p++;
             // номер операции "Вариант"
             pW->nVariantOperationNumber = ((int*)p)[0];
             p = p+4;
            // номер выбранного варианта
             pW->nChosenVariantNumber = ((int*)p)[0];
             p = p+4;
           } // if pW
         } //for j
        else  // восстановим имя файла стр-ры
           {  // если нет окон
             k = StrEnd(p)-p;
             LoadedPartitions[i].StructureName = AnsiString(p);
             p = p+k+1;
           }
       }  // for i
   // номер операции, отправленной в ПРИС (на нее ждем ответ)
      totalOperationsInPris = ((int*)p)[0];
      p = p+4;
      // массив отправленных в ПРИС операций
      try{ memcpy(&externalOperation[0],p,100);}
      catch(...)
         { WriteToDebugFile("Ошибка при пересылке externalOperation");
           return -1;
         }
      p = p+100;
      // кол-во операций в ПРИС
      nOperationInPrisCount = ((int*)p)[0];
      p = p+4;
      // стек операций для ПРИСа по секциям и pWindow( (14*3)*3)
      try{ memcpy(&aOperationInPris[0],p,14*3*3);}
      catch(...)
         { WriteToDebugFile("Ошибка при пересылке aOperationInPris");
           return -1;
         }
      p = p+14*3*3;
      // номер резки файла протокола
      nBisect = ((int*)p)[0];
      p = p+4;
      // файл протокола
      k = StrEnd(p) - p; // длина имени протокола
      ProtokolName = AnsiString(p);
      TBPartitionTitle = Abr;
      char* ps = AnsiStrScan(p,'.');
      if(ps) ps[0] = 0;
      TBProtokolNamePart1 = AnsiString(p);
      if(ps)TBProtokolNamePart2 = AnsiString(ps+1);
    }
 } // если ф.сохр. существует
 return 0;
}
//////////////////////////////////////////////////////////////////
// ф-я сохранения КТ
//---------------------------------------------------------------------------
// Переменные ИСР для сохранения по КТ
// кол-во загруженных разделов - int PartitionCount
// номер текущего раздела      - int nSelectedPartitionNumber
// номера текущ.секций выбр.разделов - int anCurrentSection[3]
// состояние ИСР - ISRState (0 - раздел не выбран,
//                           1 - выбран, 2 - раздел выполняется)
// режим работы ИСР - WorkMode (0 - автоматический, 1 - шаговый)
// выполнение блока Авто - bool bExecuteAutoBlock (true - идет вып-е)
// имя текущего протокола - AnsiString ProtokolName;
short __fastcall TISRMainForm::save_KT(AnsiString p_save)
{
   FILE* f;
   AnsiString prot;
   char obl[1000]; // область для записи сохраняемых данных
   int kz = 0;
   short i,j, k=0;
   char*p = &obl[0];
   static bool busy=0;
   if(busy) return 0;
/////////////////////////////
    busy = true;
   if ((f = fopen(p_save.c_str(), "w+b"))== NULL)
    {
      ErrorMessageBox(" Не могу открыть файл "+p_save);
      busy=0;
      return -1;
    }
   WriteToDebugFile("Cоздали файл "+p_save);
   rewind(f); // встали в начало файла
   // сохраняем области (описание см. в конце файла)
   p = p+3; // сместимся на массив пр.загрузки окон
   // текущее состояние ИСР
   p[0] =  ISRCurrentState;
   p++;
   // признак наличия структуры у головного раздела
   p[0] =  bMainPartitionStructureExists;
   p++;
   // флаг: головной раздел загружен из ИСР
   p[0] =  bMainPartitionLoadedFromISR;
   p++;
   // кол-во загруженных разделов
   ((int*)p)[0] = nPartitionCount;
   p = p+4;
   // номер выбранного раздела
   ((int*)p)[0] = nSelectedPartitionNumber;
   p = p+4;
 try
  { for(i=0;i<nPartitionCount;i++)
     {
        k = LoadedPartitions[i].Abbr.Length();
        memcpy(p,(LoadedPartitions[i].Abbr).c_str(),k);
        (p+k)[0] = 0;
        p = p+k+1;
     }
    // номера текущих секций выбранных разделов
   for(i=0;i<3;i++)
              ((int*)p)[i] = anCurrentSection[i];
   p = p+12;
    // сохраним текущие операции
    for(i=0;i<nPartitionCount;i++)
     {
      for(j=0;j<3;j++)
        if(LoadedPartitions[i].pWindow[j])
         { obl[i] = 0;
           // если окна открыты
           TMDIChild* pW = LoadedPartitions[i].pWindow[j];
          ((int*)p)[0] = pW->nCurrentOperation;
          p = p+4;
          // сохранить массив выполненных операций
          k = pW->nOperationCount;
          ((int*)p)[0] = k; // размерность массива
          p = p+4;
          memcpy(p,pW->pbMadeOperation,k);
          p= p+k;
          // признак выполнения операции ПРИСом
          p[0] = pW->bPrisExecutesOperation;
          p++;
          // номер выполняемой операции
          ((int*)p)[0] = pW->nExecutesOperationNumber;
          p = p+4;
          // комментарий к текущей операции
          k = pW->CommentToCurrentOperation.Length();
          memcpy(p,pW->CommentToCurrentOperation.c_str(),k);
          p= p+k;
          p[0] = 0;
          p++;
          // операции блока Auto
          ((int*)p)[0] = pW->nAutoFirst;  // первая
          p = p+4;
          ((int*)p)[0] = pW->nAutoLast;  //последняя
          p = p+4;
          // признак выполнения блока Auto
          p[0] = pW->bExecuteAutoBlock;
          p++;
          // признак выбора варианта
          p[0] = pW->bSelectedVariant;
          p++;
          // номер операции "Вариант"
          ((int*)p)[0] = pW->nVariantOperationNumber;
          p = p+4;
          // номер выбранного варианта
          ((int*)p)[0] = pW->nChosenVariantNumber;
          p = p+4;
         }
       else // окон нет, установим спец.признак
           { obl[i] = 1;
             // сохранить имя файла структуры
             if(j==0) // запоминаем только один раз
              { k = LoadedPartitions[i].StructureName.Length();
                memcpy(p,LoadedPartitions[i].StructureName.c_str(),k);
                p = p+k;
                p[0] = 0;
                p++;
              }
           }
     }
    // номер операции, отправленной в ПРИС (на нее ждем ответ)
    ((int*)p)[0] = totalOperationsInPris;
    p = p+4;
    // массив отправленных в ПРИС операций
    memcpy(p,&externalOperation[0],100);
    p = p+100;
    // кол-во операций в ПРИС
    ((int*)p)[0] = nOperationInPrisCount;
     p = p+4;
    // стек операций для ПРИСа по секциям и pWindow( (14*3)*3)
    memcpy(p,&aOperationInPris[0],14*3*3);
    p = p+14*3*3;
    // номер резки файла протокола
    ((int*)p)[0] = nBisect;
    p = p+4;
    // имя файла протокола
    prot = GetProtokolName();
    k = prot.Length();
    memcpy(p,prot.c_str(),k);
  }
 catch(...) { kz = -1;
              WriteToDebugFile("Memcpy error");
              goto E;
            }
   (p+k)[0] = 0;
   p = p+k+1;
   k = p - obl; // кол-во сохраняемых байтов
   try { fwrite(obl,k,1,f);}
   catch(...) { kz = -1;
                WriteToDebugFile("Write error");
                goto E;
              }
   try{ fclose(f);}
   catch(...) {WriteToDebugFile("Сlose error");}
E: if(kz == -1) ErrorMessageBox(" Ошибка записи в файл "+p_save);
   busy=0;
   return 0;
}

//---------------------------------------------------------------------------
// Подпрограмма отображения первого загруженного раздела РЭП
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::N7Click(TObject *Sender)
{
   AnsiString s;
   if( !Partision_Rep.IsEmpty() )
      {
        PartisionRep->rzd = Partision_Rep;
        ShowED(etPart_REP1, "", false);
      }
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
//             Формирование списка разделов РЭП
/////////////////////////////////////////////////////////////
// ParameterAdjustments - список (через ";" с \0 в конце) разделов
// из файла настройки "*.on" - все строки "РАЗДЕЛЫ"
// ParameterAdjustmentsLocation - полный путь к файлам настройки
// разделов РЭП из списка  ParameterAdjustments
// ParameterSPOLocation - размешение СПО (каталоги разделов)
#include <stdio.h>
#include <io.h>
 void  TISRMainForm::FormPartition_REP()
{
  FILE* f;
  int l = 0;
  char* p, *p1;
  AnsiString s = ParameterAdjustmentsLocation;
  AnsiString s1;
 // очистим старый список разделов РЭП
 if( PartisionRep )
		PartisionRep->ListBox1->Items->Clear();
 if( !ParameterAdjustments.IsEmpty() )                           // есть РАЗДЕЛЫ
  {
	// обработка имён разделов
	p1 = ParameterAdjustments.c_str();
	l =  ParameterAdjustments.Length();
	while ( ( p = StrScan( p1, ';' ) ) || ( p = StrScan( p1, '\0' ) ) )
	 {
	   if( p1 > ( ParameterAdjustments.c_str() + l ) )
					  return;
	   s1 =  AnsiString( p1, p - p1 );                           // имя файла раздела с путём
	   // Проверить,существует ли ф.настр.РЭП для данн.раздела
	   s1 =  s + "\\" + s1 + ".set" ;                            // Имя файла настройки
	   if( FileExists(s1) )
		{
		  if( ( f = fopen( s1.c_str(), "rt" ) ) == NULL )        // открыть файл настройки
		   {
			 ShowMessage( "Не открылся файл  " + s + "\\" + s1 + ".set" );
			 return;
		   }
		  else                                                   // ищем разделы с типом "П"
		   {
			  AnsiString ss = AnsiString();
			  char* c = (char*)calloc( 81, sizeof(char) );
			  for( int i = 0; ; i++ )
			   if( fgets( c, 80, f ) )
				{
				  if( i < 6 )                                    // пропустим паспорт
					 continue;
				  // есть файл ".rtf" в каталоге раздела?
				  if( c[0] == 'П' )
				   {
					 // выделим имя файла после "П"
					 char* c1 = StrScan( c + 2,'|' );
					 s1 = AnsiString( c + 2, c1 - ( c + 2 ) );
					 if( FileExists( ParameterSPOLocation + "\\" + s1 + "\\" + s1 + ".rtf" ) )
						PartisionRep->ListBox1->Items->Add(s1);  // тогда вставим в список
				   }
				}
			   else break;
			   free(c);
		   }
		fclose(f);
	  }
	p1 = p + 1;
  }
 }
}
//---------------------------------------------------------------------------
// Подпрограмма отображения через меню второго загруженного раздела РЭП
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::N9Click(TObject *Sender)
{
  if( !Second_Rep.IsEmpty() )
      {
        PartisionRep->rzd = Second_Rep;
		ISRMainForm->ShowED( etPart_REP2, "", false );
      }
}
//---------------------------------------------------------------------------
// Подпрограмма отображения через меню третьего загруженного раздела РЭП
//---------------------------------------------------------------------------
void __fastcall TISRMainForm::N10Click(TObject *Sender)
{
   if( !Third_Rep.IsEmpty() )
      {
        PartisionRep->rzd = Third_Rep;
		ISRMainForm->ShowED( etPart_REP3, "", false );
      }
}
//---------------------------------------------------------------------------
#include "CloseRep1.h"
void __fastcall TISRMainForm::N11Click(TObject *Sender)
{
  if( CloseRep->ListBox1->Items->Count )
     CloseRep->ShowModal();
}
//---------------------------------------------------------------------------
// Закрытие всех разделов РЭП
//-----------------------------------------------------------
#include "CloseRep1.h"
void __fastcall TISRMainForm::N12Click(TObject *Sender)
{
  int i;
  if( Partision_Rep.IsEmpty() && Second_Rep.IsEmpty() && Third_Rep.IsEmpty() )
      return; // нет ни одного раздела
  SendStringToPrisProtokol( "РЭП : Закрытие всех разделов :", stStandart, false, true, false );
  CloseRep->CloseWrd(4);
  if( !Partision_Rep.IsEmpty() )  // закрываем первый раздел
        {
		  SendStringToPrisProtokol( "РЭП : Выгружен раздел " + Partision_Rep, stStandart, false, true, false);
          Partision_Rep.SetLength(0);
          N7->Caption = "Раздел РЭП";
        }
  if( !Second_Rep.IsEmpty() )   // закрываем 2-й раздел
        {
          delete(ISRMainForm->SecondRep);
		  SendStringToPrisProtokol( "РЭП : Выгружен раздел " + Second_Rep, stStandart, false, true, false);
          Second_Rep.SetLength(0);
          N9->Caption = "Второй раздел";
        }
  if( !Third_Rep.IsEmpty() )    // закрываем 3-й раздел
        { 
          delete(ISRMainForm->ThirdRep);
		  SendStringToPrisProtokol( "РЭП : Выгружен раздел " + Third_Rep, stStandart, false, true, false);
          Third_Rep.SetLength(0);
          N10->Caption = "Третий раздел";
        }
  //Partision_Rep1.SetLength(0);
  //
  CloseRep->ListBox1->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TISRMainForm::N13Click( TObject *Sender )
{
  //int l = PartisionRep->ListBox1->Items->Count ;
   // if(l)
         PartisionRep->ShowModal();
  //  else
  //	  WarningMessageBox( "Не задан ни один раздел РЭП","ИСР" );
}
//---------------------------------------------------------------------------

