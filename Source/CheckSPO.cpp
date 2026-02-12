//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckSPO.h"
#include "Common.h"
//#include "stringconst.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCheckSPOBox *CheckSPOBox;
//---------------------------------------------------------------------------
__fastcall TCheckSPOBox::TCheckSPOBox(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCheckSPOBox::CloseBtnClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TCheckSPOBox::AnyCheckBoxClick(TObject *Sender)
{
    SetCheckBtnEnabled();
}
//---------------------------------------------------------------------------
// функция устанавливает доступность кнопки "Проверить"
// для этого проверяется, установлен ли хоть один CheckBox
void __fastcall TCheckSPOBox::SetCheckBtnEnabled()
{
    CheckBtn->Enabled = AdjustmentCheckBox->Checked || StructureCheckBox->Checked
        || CPKUProgramsCheckBox->Checked || SSBVSProgramsCheckBox->Checked;
}


void __fastcall TCheckSPOBox::AnyCheckBoxKeyPress(TObject *Sender,
      char &Key)
{
    SetCheckBtnEnabled();
}
//---------------------------------------------------------------------------

void __fastcall TCheckSPOBox::CheckBtnClick(TObject *Sender)
{
    AnsiString ErrorMessage, ReportMessage, InformationMessage;
    AnsiString FindFileName;
    AnsiString ProgramFileName;
    AnsiString ProgramName;
    AnsiString PartitionHomeDir;
    AnsiString FullDirName;
    AnsiString ProgramDirName;
    bool bFindProgram;
    bool bFindResult, bFindResult2;
    bool bFindFirst;
    TSearchRec SearchRec;
    ListElement* pRequiredProgram;
    ListElement* pRealProgram;
    PartitionRecord* pCurrentPartition = &LoadedPartitions[nCurrentPartition];
    int nTmpErrorCount;

    nErrorCount = 0;
	// если отладка раздела
	if( bPartitionDebug )
	 {
        PartitionHomeDir = DebugDir;
	 }
    else {
        PartitionHomeDir = ParameterSPOLocation + "\\" + PartitionAbbr;
    }
	ReportMessage = "Протокол проведения контроля СПО раздела \"" + PartitionAbbr + "\".";
	SendStringToPrisProtokol( ReportMessage, stStandart, true, true, false );
    // если установлен переключатель "Проверить файл настройки раздела"
	if( AdjustmentCheckBox->Checked )
	 {
	   // если головной файл раздела
	   if( bHEADAdjustment )
		{
			// если файла настройки раздела нет на диске
			if( !FileExists(AdjustmentFileName) )
			 {
				SendStringToPrisProtokol( L"Файл настройки раздела - ОК!", st_Blue, false, false, true );
			 }
			else
			 {
				SendStringToPrisProtokol( "Файл настройки раздела присутствует на диске, при этом информация о разделе прописана в головном файле настройки раздела", stError, false, false, true );
				nErrorCount++;
			 }
		}
	   // файл настройки пользователя
	   else
		{
		  // если файла настройки раздела есть на диске
		  if( FileExists(AdjustmentFileName) )
			{
				SendStringToPrisProtokol( "Файл настройки раздела - ОК!", st_Blue, false, false, true );
			}
		  else
			{
				ReportMessage = "Файл структуры раздела \"" + AdjustmentFileName + "\" не найден";
				SendStringToPrisProtokol( ReportMessage, stError, false, false, true );
                nErrorCount++;
            }
		}
    }
    // если установлен переключатель "Проверить файл структуры"
	if( StructureCheckBox->Checked )
	 {
	   // если файла настройки раздела есть на диске
	   if( FileExists(StructureFileName) )
		 {
			ReportMessage = "Файл структуры раздела - ОК!";
			SendStringToPrisProtokol(ReportMessage, st_Blue, false, false, true);
		 }
	   else
		 {
			ReportMessage = "Файл структуры раздела \"" + AdjustmentFileName + "\" не найден";
			SendStringToPrisProtokol(ReportMessage, stError, false, false, true);
            nErrorCount++;
		 }
	 }
    // если установлен переключатель "Проверить файлы программ раздела в ЦПКУ"
	if( CPKUProgramsCheckBox->Checked )
     {
        nTmpErrorCount = nErrorCount;
        // очистим список программ в ЦПКУ
        CPKURealProgramList.ReleaseList();
        // цикл по всем каталогам программ
		for( int nCatalogNumber = 0; nCatalogNumber < pCurrentPartition->nDirectioriesCount; nCatalogNumber++ )
		 {
			ProgramDirName = pCurrentPartition->Directories[nCatalogNumber];
			// если директория заимствованная
			if( pCurrentPartition->bAlienDirectory[nCatalogNumber] )
			 {
				// если каталога нет
				if( !FindDir( ProgramDirName, FullDirName ) )
				 {
					ReportMessage = "Заимствованный каталог с программами (на ЦПКУ) \"" + ProgramDirName + "\" не найден";
					SendStringToPrisProtokol( ReportMessage, stError, false, false, true );
					nErrorCount++;
				 }
			 }
			// директория родная
			else
			 {
				FindFileName = PartitionHomeDir + "\\" + ProgramDirName;
				bFindResult = FindFirst( FindFileName, faDirectory, SearchRec ) == 0;
				FindClose(SearchRec);
				// если каталог не найден
				if( !bFindResult )
				 {
					ReportMessage = "Каталог с программами (на ЦПКУ) \"" + ProgramDirName + "\" не найден";
					SendStringToPrisProtokol(ReportMessage, stError, false, false, true);
					nErrorCount++;
				 }
				else
				 {
					FindFileName = PartitionHomeDir + "\\"
							   + pCurrentPartition->Directories[nCatalogNumber] + "\\*" + ProgramExtention;
					bFindFirst = true;
					bFindResult2 = true;
					// пока файлы программ ищутся
					while (bFindResult2)
					 {
						if( bFindFirst )
						 {
							bFindResult2 = FindFirst( FindFileName, faAnyFile, SearchRec ) == 0;
							bFindFirst = false;
						 }
						else
						 {
							bFindResult2 = FindNext(SearchRec) == 0;
						 }
						// если файл программы найден
						if( bFindResult2 )
						 {
							ProgramFileName = SearchRec.Name;
							// удалим расширение
							ProgramFileName = ProgramFileName.SubString( 1, ProgramFileName.Length() - 4 );
							// вставим имя программы в список
							CPKURealProgramList.Prepend(ProgramFileName);
						 }
					 }
					FindClose(SearchRec);
				 }
			 }
		 }
  // пройдемся по списку программ, которые должны быть
        pRequiredProgram = CPKURequiredProgramList.First();
		while( pRequiredProgram != NULL )
		{
            // пока считаем, что программа не найдена
            bFindProgram = false;
            ProgramName = pRequiredProgram->String;
            // пройдемся по списку программ, которые есть
			pRealProgram = CPKURealProgramList.First();
			while( pRealProgram != NULL )
			 {
                // если имя реальной программы совпало с именем требуемой
				if( pRealProgram->String.AnsiCompareIC(ProgramName) == 0 )
				 {
                    bFindProgram = true;
                    break;
				 }
                // перейдем к следующей программе
				pRealProgram = pRealProgram->Next;
			 }
            // если файла программы нет на диске
			if (!bFindProgram)
			 {
				ReportMessage = "Файл программы ЦПКУ \"" + ProgramName
								+ "\" есть в списке СПО раздела, но не найден на диске";
				WriteToDebugFile( ReportMessage );
				SendStringToPrisProtokol( ReportMessage, stError, false, false, true );
                nErrorCount++;
			 }
            // перейдем к следующей программе
            pRequiredProgram = pRequiredProgram->Next;
		}
        // пройдемся по списку программ, которые есть на диске
        pRealProgram = CPKURealProgramList.First();
		while( pRealProgram != NULL )
		{
            // пока считаем, что программа не найдена
            bFindProgram = false;
            ProgramName = pRealProgram->String;
            // пройдемся по списку программ, которые должны быть
            pRequiredProgram = CPKURequiredProgramList.First();
			while( pRequiredProgram != NULL )
			 {
                // если имя реальной программы совпало с именем требуемой
				if( pRequiredProgram->String.AnsiCompareIC(ProgramName) == 0 )
				 {
                    bFindProgram = true;
                    break;
				 }
                // перейдем к следующей программе
                pRequiredProgram = pRequiredProgram->Next;
			 }
            // если файла программы нет на диске
			if( !bFindProgram )
			 {
				ReportMessage = "Файл программы ЦПКУ \"" + ProgramName
								 + "\" есть на диске, но отсутствует в списке СПО раздела";
				SendStringToPrisProtokol( ReportMessage, stError, false, false, true );
                nErrorCount++;
			 }
            // перейдем к следующей программе
			pRealProgram = pRealProgram->Next;
		}
        // если кол-во ошибок за проход программ не изменилось
		if( nTmpErrorCount == nErrorCount )
		 {
			ReportMessage = "Файлы программ на ЦПКУ - ОК!";
			SendStringToPrisProtokol( ReportMessage, st_Blue, false, false, true );
		 }
	}
    // если ошибок нету
	if( nErrorCount == 0 )
	 {
		ReportMessage = "Контроль СПО для раздела в соответствии с заданными критериями прошел успешно";
		SendStringToPrisProtokol( ReportMessage, st_Blue, false, false, true );
		InformationMessage = "Контроль СПО для раздела в соответствии с заданными критериями прошел успешно";
		InformationMessageBox( InformationMessage );
	 }
	else
	 {
		ReportMessage = "В процессе проведения контроля СПО для раздела в соответствии с заданными критериями обнаружены ошибки. Количество ошибок - " + IntToStr(nErrorCount) + ".";
		SendStringToPrisProtokol( ReportMessage, stError, false, false, true );
		ReportMessage = "В процессе проведения контроля СПО для раздела в соответствии с заданными критериями обнаружены ошибки. Количество ошибок - " + IntToStr(nErrorCount) + ". Подробности смотри в протоколе ПРИСа";
        ErrorMessageBox(ReportMessage);
	 }
	ReportMessage = "Конец протокола проведения контроля СПО раздела";
	SendStringToPrisProtokol( ReportMessage, stStandart, true, true, false );
}
//---------------------------------------------------------------------------
void __fastcall TCheckSPOBox::Prepend()
{
    int nStructureFileHandler;
    AnsiString ErrorMessage;
    char pcReadBuffer[MaxStringLength + 1];
    AnsiString ReadBuffer;
    int nCurrentString;
    int i;
    int nErrorCode;

    // установим имя файла настройки раздела
    AdjustmentFileName = GetAdjustmentFileName(nCurrentPartition);
    // флаг: головной файл раздела
    bHEADAdjustment = LoadedPartitions[nCurrentPartition].bIsHEADAdjustment;
    StructureFileName = GetStructureFileName(nCurrentPartition);
    PartitionAbbr = LoadedPartitions[nCurrentPartition].Abbr;

    // файл настройки раздела
    AdjustmentCheckBox->Enabled = !bHEADAdjustment;
    AdjustmentCheckBox->Checked = !bHEADAdjustment;

    // если есть файл структуры
	if( !StructureFileName.IsEmpty() )
	 {
        // файл структуры раздела
        StructureCheckBox->Checked = true;
        StructureCheckBox->Enabled = true;
    	// откроем файл структуры
		nStructureFileHandler = FileOpen( StructureFileName, fmOpenRead + fmShareDenyWrite );
        // если неудачно
		if( nStructureFileHandler == -1 )
		 {
    		// сообщение о неудаче
			ErrorMessage = "Не могу открыть файл \"" + StructureFileName + "\".";
    		// выведем
			ErrorMessageBox( ErrorMessage, "Ошибка" );
            CPKUProgramsCheckBox->Checked = false;
            CPKUProgramsCheckBox->Enabled = false;
			SSBVSProgramsCheckBox->Checked = false;
            SSBVSProgramsCheckBox->Enabled = false;
            return;
		 }
		// дошли до конца файла
        bool bFileEnd = false;
        int anDelimiter[4];
        int nCurrentDelimier;
        int nDelimiterCount;
        AnsiString ProgramName, Location;
        nCurrentString = -1;
        CPKURequiredProgramList.ReleaseList();
        SSBVSRequiredProgramList.ReleaseList();
        // цикл по чтению строк из файла структуры
		while(1)
		 {
			ReadStringFromFile( nStructureFileHandler, pcReadBuffer, nErrorCode );
            nCurrentString++;
            ReadBuffer = pcReadBuffer;
            // если начало секции 4
			if( ReadBuffer.SubString( 1, 2 ).AnsiCompare( AnsiString(cSectionBeginEndSymbol) + "4" ) == 0 )
			 {
                break;
			 }
		 }
        // цикл до тех пор, пока не дойдем до первой строки тела секции 4
        // или конца секции 4
		while(1)
		 {
			ReadStringFromFile( nStructureFileHandler, pcReadBuffer, nErrorCode );
            nCurrentString++;
            ReadBuffer = pcReadBuffer;
            // если строка формата 6
			if( SymbolCount(ReadBuffer, cFieldDelimiter) == 4 )
			 {
                break;
			 }
            // если конец секции 4
			if( ReadBuffer.SubString( 1, 2 ).AnsiCompare( AnsiString(cSectionBeginEndSymbol) + "4" ) == 0 )
			 {
                bFileEnd = true;
                break;
			 }
            // если строка не формата 5
			if( SymbolCount(ReadBuffer, cFieldDelimiter) != 2 )
			{
				ErrorMessage = "Нарушена структура файла \"" + StructureFileName + "\" в строке "
							   + IntToStr(nCurrentString + 1) + ".";
				ErrorMessageBox( ErrorMessage, "Ошибка" );
                CPKUProgramsCheckBox->Checked = false;
                CPKUProgramsCheckBox->Enabled = false;
                SSBVSProgramsCheckBox->Checked = false;
                SSBVSProgramsCheckBox->Enabled = false;
                return;
            }
        }
        // цикл до конца файла
		while( !bFileEnd )
		 {
            nDelimiterCount = SymbolCount(ReadBuffer, cFieldDelimiter);
            // если строка формата 6
			if (nDelimiterCount == 4)
			{
                nCurrentDelimier = 0;
                // найдем все разделители
				for (i = 1; i <= ReadBuffer.Length(); i++)
				{
					if (ReadBuffer[i] == cFieldDelimiter)
					{
                        anDelimiter[nCurrentDelimier++] = i;
                    }
                }
                ProgramName = ReadBuffer.SubString(anDelimiter[0] + 1, anDelimiter[1] - anDelimiter[0] - 1);
                Location = ReadBuffer.SubString(anDelimiter[2] + 1, anDelimiter[3] - anDelimiter[2] - 1);
				if (!ProgramName.IsEmpty() && !IsComment(ProgramName) )
				 {
                    // добавим программу в соотв. список
					if( StringRusToLat(Location).AnsiCompareIC( StringRusToLat("СС БВС") ) == 0 )
					 {
                        SSBVSRequiredProgramList.Prepend(ProgramName);
					 }
					else
					{
						if (Location.IsEmpty())
						 {
                            CPKURequiredProgramList.Prepend(ProgramName);
						 }
						else
						 {
							ErrorMessage = "При анализе файла структуры выявлена ошибка: некорректно указано размещение программы. Ошибка обнаружена в строке "
										   + IntToStr(nCurrentString + 1) + ". Указано размещение: \""
										   + Location + "\". Данная строка будет проигнорирована";
							ErrorMessageBox( ErrorMessage, "Ошибка"  );
						 }
					}
				 }
			 }
			else
			 {
				// если строка формата 5
				if( nDelimiterCount == 2 )
				{
				   // если конец секции 4
				  if( ReadBuffer.SubString( 1, 2 ).AnsiCompare(AnsiString(cSectionBeginEndSymbol) + "4" ) == 0)
					{
                        bFileEnd = true;
                    }
                }
				else
				{
					ErrorMessage = "Нарушена структура файла \"" + StructureFileName + "\" в строке " + IntToStr(nCurrentString + 1) + ".";
					ErrorMessageBox( ErrorMessage, "Ошибка" );
                    CPKUProgramsCheckBox->Checked = false;
                    CPKUProgramsCheckBox->Enabled = false;
                    SSBVSProgramsCheckBox->Checked = false;
                    SSBVSProgramsCheckBox->Enabled = false;
					return;
				}
			 }
			if( ReadStringFromFile( nStructureFileHandler, pcReadBuffer, nErrorCode ) == 0 )
			 {
                bFileEnd = true;
			 }
            // если не конец файла
			if (!bFileEnd)
			 {
                ReadBuffer = pcReadBuffer;
                nCurrentString++;
			 }
		 }
        // установим доступность переключателей
        bool bCPKUProgramExists = CPKURequiredProgramList.GetElementCount() > 0;
        CPKUProgramsCheckBox->Enabled = bCPKUProgramExists;
        CPKUProgramsCheckBox->Checked = bCPKUProgramExists;
        // на будущее: при контроле СПО в СС БВС доступность переключателя
        // "Файлы программ раздела в СС БВС" определяется наличием программ
        // в СС БВС в списке СПО файла структуры (параметр bSSBVSProgramExists)
//        bool bSSBVSProgramExists = SSBVSRequiredProgramList.GetElementCount() > 0;
//        SSBVSProgramsCheckBox->Enabled = bSSBVSProgramExists;
//        SSBVSProgramsCheckBox->Checked = bSSBVSProgramExists;
        SSBVSProgramsCheckBox->Enabled = false;
        SSBVSProgramsCheckBox->Visible  = false;
        SSBVSProgramsCheckBox->Checked = false;
		FileClose( nStructureFileHandler );
	 }
	else
	 {
        StructureCheckBox->Checked = false;
        StructureCheckBox->Enabled = false;
        CPKUProgramsCheckBox->Checked = false;
        CPKUProgramsCheckBox->Enabled = false;
        SSBVSProgramsCheckBox->Checked = false;
        SSBVSProgramsCheckBox->Enabled = false;
	 }
    SetCheckBtnEnabled();
}
//----------------------------------------------------------
void __fastcall TCheckSPOBox::FormActivate(TObject *Sender)
{
    if (CheckBtn->Enabled)
        CheckBtn->SetFocus();
}
//---------------------------------------------------------------------------

