//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
#include "Common.h"
#include "Main.h"
#include "AdjNames.h"
//---------------------------------------------------------------------------
// константы
//---------------------------------------------------------------------------
// размеры колонок
const int anColWidth[5] = {16, 31, 16, int(34*8.25), int(33*8.25)};
// имя файла протокола
const AnsiString ProtokolFileName = "Protokol.txt";
// символ, обозначающий начало и конец секции
const char cSectionBeginEndSymbol = '#';
// символ, разделяющий поля в файле структуры
const char cFieldDelimiter = '|';
// символ, обозначающий заимвстванную директорию
const char cAlienDirectorySymbol = '#';
// символ, обозначающий общедоступную директориб
const char cCommonDirectorySymbol = '*';
// символ, которым отмечается строка в блоке, выполняющимся автоматически
const char cAuto = 'A';
// символ, разделяющий поля в строке с параметрами директивы начала раздела
const char cPartitionDirFieldDelimiter =  '|';
//------------------------------------------------------------------------------
// цвета
//------------------------------------------------------------------------------
const unsigned int nGridBackColor = 0x00E7FFFF;          	// фон таблицы
const unsigned int nGridSelColor = RGB(0,0,128);         	// выделенная ячейка
const unsigned int nCurOperationBackColor = 0xc0ffc0;    	// текущая операция
const unsigned int nMadeOperationColor = 0xffffc0;       	// выполненная операция
const unsigned int nHeaderColor = clBtnFace;             	// цвет заголовка
const unsigned int nSelFontColor = clYellow;             	// цвет шрифта выделенной операции
const unsigned int nNonSelFontColor = RGB( 0, 0, 0 );    	// цвет шрифта невыделенной операции
//
AnsiString SectionType[3];                               	// краткие названия секций
const unsigned char pcMadeMarker[2] = {214, 0};          	// маркер выполненной операции
const unsigned char pcCurrentOperationMarker[2] = {222, 0}; // маркер текущей операции
// расширения
const AnsiString ProgramExtention    = ".dip";
const AnsiString StructureExtention  = ".dii";
const AnsiString AdjustmentExtention = ".set";
const AnsiString ProtokolExtention   = ".prt";
const AnsiString EDFilesExtention    = ".rtf";
// формат строки файла настройки
const int anAdjustmentMaxLength[6] = { 1, 30, 30, 30, 21, 21}; // { 1, 16, 17, 16, 21, 21};
const char cNShSLabelSymbol = ':';                          // символ НШС
int StencilLength = 17;  //9;                               // длина шаблона
//---------------------------------------------------------------------------
// переменные
//---------------------------------------------------------------------------
AnsiString PartitionStartDirName;                           // имя дир-вы ПРИСа, которая запускает раздел
AnsiString ISRStartDirName;                                 // имя команды ИСР, которая запускает раздел
AnsiString GoInitialLabel;                                  // имя метки секции приведения в исходное
AnsiString NormalWorkLabel;                                 // имя метки секции штат-го пров-я раздела
AnsiString NShSWorkLabel;                                   // имя метки секции НШС
AnsiString NShSLabel;                                       // метка блока НШС
AnsiString NShSEndLabel;                                    // метка конца блока НШС
AnsiString ReportName;                                      // точное название операции "Доклад"
AnsiString CommandName;                                     // точное название операции "Команда"
AnsiString MessageName;                                     // точное название оп-ии "Указание оператору"
AnsiString DipolOperationName;                              // точное название операции "Диполь-операция"
AnsiString ISROperationName;                                // точное название операции "Директива ИСР"
char acFileReadErrorMessage[2][100];                        // сообщения об ошибках при чтении файла

AnsiString AutoStartPartitionLabel;                         // точное назв-е метки на зап. раздела в авт.режиме
AnsiString StepStartPartitionLabel;                         // точное назв-е метки на запуск раздела в шаговом режиме
AnsiString ProgramCallStament;                              // название операции вызова диполь-программы
AnsiString SPStament;                                       // название операции сохранения протокола
AnsiString EDNshSArticleName;                               // название типа ЭД НШС изделия
AnsiString EDDamageArticleName;                             // название типа ЭД АВАР изделия
AnsiString EDGoInitialArticleName;                          // название типа ЭД ПРИ изделия
AnsiString NASRDirName;                                     // точное название дир-вы "переход на структуру раздела"
//---------------------------------------------------------------------------
AnsiString AutoMessage;                     // строка, которая выводится в загол.окна при вып-ии блока "Авто"
AnsiString aHeader[3][4];                   // заголовки колонок
// позиция главного окна
int nMainWindowLeft = 0;
int nMainWindowTop = 69;
int nMainWindowWidth = 647;
int nMainWindowHeight;
// позиция заголовка
const int nCaptionLeft = 10;
const int nCaptionTop = 5;
// позиция таблицы
int nGridLeft = 0;
int nGridTop = 25;
int nGridWidth = nMainWindowWidth - 7;
int nGridHeight;
//
int nGridRowCount;                          // кол-во строк в сетке
int nGridRowHeight = 18;                    // высота ячейки
bool bBigFont;                              // флаг: установлен ли крупный шрифт
// размер шрифта ячейки сетки
int nCellFontSizeSF = 10;
int nCellFontSizeBF = 8;
// размер шрифта заголовка
int nCaptionFontSizeSF = 9;
int nCaptionFontSizeBF = 7;
// параметры, получаемые из файла настройки
AnsiString ParameterFilesLocation;          // параметр "РАЗМЕЩЕНИЕ_ФАЙЛОВ="
AnsiString ParameterSPOLocation;            // параметр "РАЗМЕЩЕНИЕ_СПО="
AnsiString ParameterAdjustmentsLocation;    // параметр "РАЗМЕЩЕНИЕ_РАЗДЕЛОВ="
AnsiString ParameterAdjustments;            // параметр "РАЗДЕЛЫ="
AnsiString ParameterNShSArticle;            // параметр НШС_ИЗД
AnsiString ParameterDamageArticle;          // параметр АВАР_ИЗД авария
AnsiString ParameterGoInitialArticle;       // параметр ПРИ_ИЗД авария исходное
// Для трафарета
int size;                                   // количество символов в первой части трафарета 3 или >
String mask;                                // маска для трафарета
//
AnsiString ParameterNShSArticle_REP;        // параметр НШС_РЭП
AnsiString ParameterDamageArticle_REP;      // параметр АВАР_РЭП
AnsiString ParameterGoInitialArticle_REP;   // параметр ПРИ_РЭП
//
AnsiString ParameterProtokol;               // параметр "ПРОТОКОЛ="
int nCellTextIndent = 3;                    // отступ текста в ячейке таблицы
double dScr;                                // коэфициент, учитывающий размер системного шрифта
AnsiString AppTitle;                        // заголовок приложения
AnsiString OnFileName;                      // имя файла настройки пользователя ПРИС
AnsiString CfgFileName;                     // имя системного файла настройки ПРИС
//
PartitionRecord* Partitions;                // список разделов
int nTotalPartitionCount;                   // кол-во разделов в системе

LoadedPartitionRecord LoadedPartitions[MaxPartitionCount];      // загруженные разделы

int nPartitionCount;                        // кол-во загруженных разделов
int nCurrentPartition;                      // текущий раздел (порядковый номер)
int anCurrentSection[MaxPartitionCount];    // текущая секция
int nSelectedPartitionNumber;               // номер выбранного раздела
WorkMode CurrentWorkMode;                   // тип работы - шаговый или автомат

// инфо из окна "Заголовок"
AnsiString TBPartitionTitle;                          		// заголовок раздела
AnsiString TBProtokolNamePart1, TBProtokolNamePart2;  		// имя протокола
AnsiString TBRAPName;                                 		// имя РЭП
AnsiString TBOperatorPUAISName;                       		// имя оператора ПУ АИС
AnsiString TBOperatorAISName;                         		// имя оператора АИС
AnsiString TBOperatorTKCKName;                        		// имя оператора ТКЦК
AnsiString TBOperatorNTKSIName;                       		// имя оператора НТК СИ
AnsiString TBOperatorBSName;                          		// имя оператора БBС
AnsiString MainPartitionTitle;                        		// заголовок головного раздела
AnsiString CommonDirectories;                         		// "общедоступные" директории (со звездочкой)
int nISRID = 200;                                     		// наш ID
ISRState ISRCurrentState;                             		// состояние интерпретатора
bool bMainPartitionStructureExists;                   		// флаг: у головного раздела есть стуктура
bool bMainPartitionStructureLoaded;                   		// флаг: структура головного раздела загружена
AnsiString Stencil;                                   		// трафарет
bool bStencilExists;                                  		// флаг: в системе есть трафарет
AnsiString StencilFileName = "Stencil.dat";           		// имя файла с трафаретом
AnsiString FullStencilFileName;                       		// имя файла с трафаретом c путем
OperationInPris aOperationInPris[MaxPartitionCount * 3];  	// стек операций, посланных в ПРИС
int nOperationInPrisCount = 0;                              // кол-во операций в ПРИС
bool bMainPartitionLoadedFromISR;                           // флаг: головной раздел загружен из ИСР
AnsiString ErrFileName;                                     // имя с путем файла с сообщениями об ошибках
AnsiString DebugFileName;                                   // имя с путем файла с сообщениями об ошибках
AnsiString ProgramDir;                                      // путь к исполняемому файлу ИСР
int nBisect;                                                // номер резки
unsigned int PartitionStartTickCount;                       // время начала раздела
bool bRecievePortionForClose = false;                       // флаг: получена порция на закрытие
bool bDeveloperWorkPlace;                                   // флаг: рабочее место разработчика
bool bEndPartitionMessage = false;                          // флаг: пришло сообщение "Конец раздела"
bool bPartitionDebug = false;                               // флаг: отладка раздела
AnsiString DebugAdjustment;                                 // файл настройки отлаживаемого раздела
// расположение СПО для отлаживаемого раздела
AnsiString DebugDir;
// флаг: работаем со всеми разделами из каталога СПО
bool bWorkWithAllPartitionsFromSPO;
// флаг: в ПРИС послана а внутреняя операция ISR (не из структуры)
bool bSendInternalOperationToPris = false;
// массив указателей на диалоговые окна ИСР
HWND apDialogBox[DialogBoxCount];
// текущий список файлов настройки разделов
AnsiString CurrentAdjustmentsList;
// параметр настройки: разрешены ли переходы назад при перемещении по секции
bool bAllowBackCrossing = false;
// открыт диалог открытия файла
bool bOpenFileDialogOpen = false;
// коментарии на КО из ИСР
AnsiString KOStartComment = "ИСР: Директива";
AnsiString KOEndComment = "ИСР: Конец директивы";
// отступ в протоколе ПРИСа перед операцией
AnsiString ProtokolIndent = "      ";
// дополнительный отступ в протоколе ПРИСа
AnsiString AdditionalProtokolIndent = "          ";
// флаг: идет процесс завершения всех разделов
bool bEndAllPartitions = false;
// флаг: ПРИС загружен и мы можем начинать работу
bool bPrisLoaded = false;
// флаг: ПРИС обрабатывает конец раздела (мы ждем ответа - да или нет)
bool bPrisProcessEndPartition = false;
// флаг: параметр "ПРОТОКОЛ" непустой
bool bParameterProtokolExistsAndGood;
// домашний каталог ИСР
AnsiString ISRDir;
// имя файла ПФКС
AnsiString PFKSFullFileName;

// hwnd MainForm предыдущей копии редактора
HWND hwndPrevInstance;
// флаг: дало ли сохрранять протокол в функции EndCurrentPartition()
bool bSaveProtokol;
// имя протокола для сохранения в функции EndCurrentPartition()
AnsiString ProtokolName;
// кол-во разделов "Спецсредства"
int SpecialFacilitiesPartitionCount = 0;
// аббревиатуры разделов "Спецсредства"
AnsiString* SpecialFacilitiesPartitions;
// строка в файл отладки
AnsiString DebugString;

char DIPOL_OPERATION_SYMBOL; // 'О'
char ISR_OPERATION_SYMBOL; // 'И'
char VARIANT_SYMBOL; // 'В'
char COMMAND_SYMBOL; // 'К'
char REPORT_SYMBOL; // 'Д'
char MESSAGE_SYMBOL; // 'П'
char LABEL_SYMBOL; // 'М'

bool externalOperation[100];
int totalOperationsInPris = 0;

char HandPartition;
char CommentString;
char ReferenceString;
char SpecialFacilities;
char HidePartition = 'И';  // раздел будет загружаться только по НАЧРАЗД из ф.структуры
TList* HideAbbrList = new TList();  // список аббревиатур разделов, загрузаемых по НАЧРАЗД
                  // (тип раздела = 'И' в ф.настройки ".set" разделов из +РАЗДЕЛЫ)
//---------------------------------------------------------------------------
// функция, читает строку из файла nHandler в буфер pcBuffer
// строка заканчивается нулем
// возвращает результат:
// количество считанных байтов, отрицательное значение - неудача
// nErrorCode
// 0 - отсутсвие '\r' после '\n'
// 1 - длина строки превышает значение MaxStringLength
//---------------------------------------------------------------------------
int __fastcall ReadStringFromFile( int nHandler, char* pcBuffer, int& nErrorCode )
{
	// кол-во считанных байтов
    int nBytesCount = 0;
    char cTmpBuffer;
   while(1)
	{
		// считаем байт
		// если он не считался
		if ( FileRead( nHandler, pcBuffer + nBytesCount, 1) == 0 )
		 {
			pcBuffer[nBytesCount] = 0;
			// выйдем
			break;
		 }
		// если конец строки
		if (pcBuffer[nBytesCount] == '\r')
		 { 	// считаем '\n'
			FileRead(nHandler, &cTmpBuffer, 1);
			// если '\n'
			if (cTmpBuffer == '\n')
			 {  // конец строки
				pcBuffer[nBytesCount] = 0;
				break;
			 }
			// не '\n'
			else
			 {  // код ошибки
				nErrorCode = 0;
				nBytesCount = -1;
				break;
			 }
		 }
		// если считали символов больше максимального
		if (++nBytesCount > MaxStringLength)
		 { // код ошибки
			nErrorCode = 1;
			nBytesCount = -1;
			break;
		 }
	}  // while(1)
  // вернем кол-во байтов
    return (nBytesCount);
}
//---------------------------------------------------------------------------
// функция, читает строку из файла nHandler в буфер pcBuffer
// строка заканчивается нулем
// возвращает результат:
// количество считанных байтов, отрицательное значение - неудача
// nErrorCode
// 0 - отсутсвие '\r' после '\n'
// 1 - длина строки превышает значение MaxStringLength
// 2 - ощибка чтения
//---------------------------------------------------------------------------
int __fastcall ReadStringFromFileWin32(HANDLE hFile, char* pcBuffer, int& nErrorCode)
{
	// кол-во считанных байтов
    int nBytesCount = 0;
    char cTmpBuffer;
    unsigned long nBytesRead;
    bool bResult;
  while(1)
	{
		// считаем байт
		bResult = ReadFile(hFile, pcBuffer + nBytesCount, 1, &nBytesRead, NULL);
		if (bResult &&  nBytesRead == 0)
		{  // we're at the end of the file
			pcBuffer[nBytesCount] = 0;
			// выйдем
			break;
		}
		if (!bResult)
		{  // код ошибки
		   // пока пишем 1 (в будущм справить на 2!!!)
			nErrorCode = 1;
			nBytesCount = -1;
			break;
		}
	 // если конец строки
		if (pcBuffer[nBytesCount] == '\r')
		 {
			// считаем '\n'
			ReadFile(hFile, &cTmpBuffer, 1, &nBytesRead, NULL);
			// если '\n'
			if (cTmpBuffer == '\n')
			 {
				// конец строки
				pcBuffer[nBytesCount] = 0;
				break;
			 }
			// не '\n'
			else
			 {
				// код ошибки
				nErrorCode = 0;
				nBytesCount = -1;
				break;
			 }
		 }
		// если считали символов больше максимального
		if (++nBytesCount > MaxStringLength)
		 {
			// код ошибки
			nErrorCode = 1;
			nBytesCount = -1;
			break;
		 }
	}  // while(1)
	// вернем кол-во байтов
	//AnsiString debugString = "nBytesCount = " + IntToStr(nBytesCount) + ", nErrorCode = " + IntToStr(nErrorCode);
    //WriteToDebugFile(debugString);
    return (nBytesCount);
}
//---------------------------------------------------------------------------
// функция определяет, является ли объект TestString пустой строкой
// при наличии метки начала или конца блока НШС строка все равно
// считается пустой
bool __fastcall IsEmptyOrNShS(AnsiString TestString)
{
	// пока считаем, что результат false
	bool bResult = false;
	// проверка на НШС
	int nNShS = IsNShS(TestString);
	// если в строке НШС
	if (nNShS == 1 || nNShS == 2)
	 { 	// результат true
		bResult = true;
	 }
	// не НШС
	else
	 {  //проверка не пустроту
		bResult = TestString.IsEmpty();
	 }
	return (bResult);
}
// функция определяет, является ли объект TestString строкой начала
// или конца блока НШС
// результат
// 1 - входная строка - начало блока НШС
// 2 - входная строка - конец блока НШС
// 0 - строка не является началом или концом блока НШС
int __fastcall IsNShS(AnsiString TestString)
{
	// пока считаем, что результат 0
	int nResult = 0;
	// если в строке - начало блока НШС
	if (TestString.SubString(1, 1) == cNShSLabelSymbol
					&& TestString.SubString(TestString.Length() - 2, 3) == NShSLabel)
	 { // результат 1
	   nResult = 1;
	 }
	else
	{  // если в строке - конец блока НШС
		if (TestString.SubString(1, 3) == (NShSEndLabel + cNShSLabelSymbol)
					&& TestString.SubString(TestString.Length() - 2, 3) == NShSLabel)
		  {  // результат 2
			 nResult = 2;
		  }
	}
 // вернем реультат
	return (nResult);
}
//---------------------------------------------------------------------------
// функция выводит MessageBox с иконкой ошибки
//---------------------------------------------------------------------------
void __fastcall ErrorMessageBox( String Message, AnsiString MessageBoxTitle )
{
	String Title;
	if( MessageBoxTitle.IsEmpty())
	 {
		Title = AppTitle;
	 }
	else
	 {
        Title = MessageBoxTitle;
	 }
	ISRMainForm->SetMainWindowOnTop();
	Application->MessageBox( Message.c_str(), Title.c_str(), MB_OK + MB_ICONERROR);
}
//---------------------------------------------------------------------------
// функция выводит MessageBox с иконкой предупреждения
//---------------------------------------------------------------------------
void __fastcall WarningMessageBox( String Message, AnsiString MessageBoxTitle)
{
	String Title;
	if (MessageBoxTitle.IsEmpty())
	 {
        Title = AppTitle;
	 }
	else
	 {
        Title = MessageBoxTitle;
	 }
	ISRMainForm->SetMainWindowOnTop();
	Application->MessageBox( Message.c_str(), Title.c_str(), MB_OK + MB_ICONWARNING );
}
//---------------------------------------------------------------------------
// функция выводит MessageBox с иконкой информации
//---------------------------------------------------------------------------
void __fastcall InformationMessageBox( String Message, AnsiString MessageBoxTitle )
{
   String Title;
   if( MessageBoxTitle.IsEmpty() )
	{
		Title = AppTitle;
	}
   else
	{
		Title = MessageBoxTitle;
	}
   ISRMainForm->SetMainWindowOnTop();
   Application->MessageBox( Message.c_str(), Title.c_str(), MB_OK + MB_ICONINFORMATION );
}
//---------------------------------------------------------------------------
// функция выводит MessageBox с запросом с кнопками OK и Cancel
// возвращает результат запроса
int __fastcall QuestionMessageBox( String Message, AnsiString MessageBoxTitle, UINT nStyle)
{
	String Title;
	if (MessageBoxTitle.IsEmpty())
	{
        Title = AppTitle;
    }
	else
	{
        Title = MessageBoxTitle;
	}
	ISRMainForm->SetMainWindowOnTop();
    return (Application->MessageBox(Message.c_str(), Title.c_str(), nStyle));
}
// функция добавляет аббревиатуру(каталог) из структуры SearchRec
// в список аббревиатур CurrentAbbrList в случае, если в SearchRec
// действительно каталог аббревиатуры раздела
void __fastcall AddAbbr(TSearchRec &SearchRec, StrList* CurrentAbbrList)
{
	if (SearchRec.Attr & faDirectory)
	{
        AnsiString AdjustmentFileName = ParameterSPOLocation + "\\" + SearchRec.Name + "\\" + SearchRec.Name + AdjustmentExtention;
        // если есть файл настройки раздела
        if (FileExists(AdjustmentFileName))
            CurrentAbbrList->Prepend(SearchRec.Name);
    }
}
//---------------------------------------------------------------------------
// функция обрабатывает файлы настройки разделов
void __fastcall ProcessAdjustments(void)
{
    // если не отладка
    if (!bPartitionDebug)
    { // если параметра об расположении СПО нету
        if (ParameterSPOLocation.IsEmpty())
        {
            return;
        }
        // если работаем не со всеми разделами в СПО
        if (!bWorkWithAllPartitionsFromSPO)
        { // если параметра "Размещение разделов" нет
          if (ParameterAdjustmentsLocation.IsEmpty()) return;
        }
    }

    int nFileHandler;
	String FileName;
    int nStringLength;
    int i;
    // если есть спецсредства
    if (SpecialFacilitiesPartitionCount > 0)
    {
        SpecialFacilitiesPartitionCount = 0;
        delete[] SpecialFacilitiesPartitions;
    }
    // имя текущего анализируемого файла настройки
    AnsiString AdjustmentName;
    // кол-во разделов сеанс
    int nMaxPartitionCount = 0;
    // буфер для приема строки
    char acReadString[MaxStringLength + 1];
    // AnsiString-версия прочитаннойиз файла строки
    AnsiString ReadString;
    // сообщение об ошибке
    AnsiString ErrorMsg;
    // номер строки
    int nStringNumber;
    // разделенная на поля строка структуры
    SeparatedAdjustmentString SeparatedString;
    // список аббревиатур из каталога СПО
    StrList AllSPODirAbbrList;
    // список аббревиатур, полученных по ссылке из файлов настройки 
    StrList RefAbbrList;
    // имена файлов настройки в текущем сеансе
    AdjustmentNames SessionAdjustmentNames(CurrentAdjustmentsList);
    //флаг: сиписок разделов строим из всех разделов в каталоге СПО
    bool bUseAllPartitionsFromSPO;
    ListElement* AllSPODirAbbrListElement;
    ListElement* RefAbbrListElement;
    int nErrorCode;
    AnsiString ProcessTitle = "Построение списка разделов";
    if (!bPartitionDebug) // если не отладка
	 {
	  // если работаем со всеми разделами из СПО
	  if( bWorkWithAllPartitionsFromSPO )
       {
          AnsiString FindString = ParameterSPOLocation + "\\*.*";
          TSearchRec SearchRec;
          // поищем первый файл
          if (FindFirst(FindString, faDirectory, SearchRec) == 0)
          {
            AddAbbr(SearchRec, &AllSPODirAbbrList);
            // ищем еще
			while( FindNext(SearchRec) == 0 )
			 {
			   AddAbbr( SearchRec, &AllSPODirAbbrList );
			 }
          }
          bUseAllPartitionsFromSPO = true;
          AllSPODirAbbrListElement = AllSPODirAbbrList.First();
          FindClose(SearchRec);
        }
        else bUseAllPartitionsFromSPO = false;
        // составление списка общедоступных директорий начнем снова
        CommonDirectories = "";
     }
    bool bProcessDebugAdjustment = false;
    //  если были разделы, загружаемые только по НАЧРАЗД
      if( HideAbbrList->Count )
	   {
		 for( i = 0; i < HideAbbrList->Count; i++ )
			 delete[] HideAbbrList->Items[i];
         delete HideAbbrList;
         HideAbbrList = new TList();
       }
    // цикл по файлам настройки
    while (1)
    {
		if( bPartitionDebug )  // если отладка
		{
		  // если настройку отладки уже обработали
		  if( bProcessDebugAdjustment )
					break;
          // сформируем имя файла
          FileName = DebugAdjustment;
          bProcessDebugAdjustment = true;
        }
        else
        {
		   if (bUseAllPartitionsFromSPO)
			{ // если список окончился
			  if (AllSPODirAbbrListElement == NULL) break;
			  else
			   { // имя
				 AdjustmentName = AllSPODirAbbrListElement->String;
				 // перейдем к след. элементу
				 AllSPODirAbbrListElement = AllSPODirAbbrListElement->Next;
			   }
			  // сформируем имя файла
			  FileName = ParameterSPOLocation + "\\" + AdjustmentName + "\\" + AdjustmentName + ".set";
			}
		   else
			 { // вытащим имя файла настройки
			   AdjustmentName = SessionAdjustmentNames.GetAdjustmentName();
			   // если имени нет
				if( AdjustmentName.IsEmpty() )
				 {
					break;                       // окончим цикл
				 }
                // сформируем имя файла
                FileName = ParameterAdjustmentsLocation + AnsiString("\\") + AdjustmentName + ".set";
			 }
        }
/*        // откроем файл
        nFileHandler = FileOpen(FileName, fmOpenRead + fmShareDenyWrite);
        // если неудачно
        if (nFileHandler == -1) {
            // сообщение о неудаче
            ErrorMsg = Stringconst_sCantOpenFile + " \"" + FileName + "\".";
            // выведем
			ErrorMessageBox(ErrorMsg, ProcessTitle);
            // обрабатываем список разделов дальше
            continue;
        } */

        HANDLE hFile;
		hFile = CreateFile( FileName.c_str(),           // open
				GENERIC_READ,                           // open for reading
				FILE_SHARE_READ,                        // share for reading
				NULL,                                   // no security
				OPEN_EXISTING,                          // existing file only
				FILE_ATTRIBUTE_NORMAL,                  // normal file
				NULL );                                 // no attr. template
		if( hFile == INVALID_HANDLE_VALUE )
		 {
		   ErrorMsg = "Не открылся файл \"" + FileName + "\".";  // сообщение о неудаче
		   ErrorMessageBox( ErrorMsg, ProcessTitle );            // выведем
		   WriteToDebugFile( ErrorMsg );
		   continue;                                             // обрабатываем список разделов дальше

		 }
        // номер строки
        nStringNumber = 0;
        // цикл по чтению строк
        while (1)
		 {
		   nErrorCode = 0;
		   nStringLength = ReadStringFromFileWin32( hFile, acReadString, nErrorCode ); // прочитаем строку
		   // если ошибка при чтении
		   if( nStringLength < 0 )
			{
			  // сообщение о неудаче
			  ErrorMsg = "Нарушена структура файла \"" + FileName + "\" в строке " + IntToStr(nStringNumber + 1) + ": " + acFileReadErrorMessage[nErrorCode] + ".";
			  // выведем
			  ErrorMessageBox( ErrorMsg, ProcessTitle );
			  WriteToDebugFile( ErrorMsg );
			  break;                        // выйдем из цикла
			}
		   // если строк больше нет
		   if( nStringLength == 0 )
			   break;
		   // если не паспорт
		   if( nStringNumber > 5 )
			{
			  ReadString = acReadString;    // создадим AnsiString-версию строки, считанной из файла
			  // разобьем строку
			  SeparatedString.Abbr.SetLength(0);
			  if( SeparateAdjustmentString( ReadString, SeparatedString ) == -1 )
			     continue;
			  if (SeparatedString.cType == SpecialFacilities)
			   {
				 SpecialFacilitiesPartitionCount++;
			   }
			  if ((!SeparatedString.Abbr.IsEmpty()) && (SeparatedString.cType != HandPartition) && (SeparatedString.cType != CommentString) && (SeparatedString.cType != SpecialFacilities))
			   {
				 // включаем раздел в список
				 nMaxPartitionCount++;
				 // если это ссылка или раздел по НАЧРАЗД
				 if( SeparatedString.cType == ReferenceString || SeparatedString.cType == HidePartition )
				  {
					// добавим аббревиатуру раздела в список
					RefAbbrList.Prepend( SeparatedString.Abbr );
				  }
				 if( SeparatedString.cType == HidePartition )
				  {
					// если это раздел, загружаемый по НАЧРАЗД
					int ls = SeparatedString.Abbr.Length();
					char* as = new char[ls + 1];
					memset( as, 0, ls + 1 );
					memcpy( as, SeparatedString.Abbr.c_str(), ls );
					// добавим аббревиатуру раздела в список
					HideAbbrList->Add(as);
				  }
			   }
			}
		   nStringNumber++;
		 }
        CloseHandle(hFile);    // закроем файл
    }
    if (nMaxPartitionCount == 0) return;
    // выделим память под разделы
    PartitionRecord* TempPartitions = new PartitionRecord[nMaxPartitionCount];
    int nPartitionNumber = 0;
    bool bProcessPartition;
    AnsiString Directory;
    AnsiString Abbr;
    int nRealPartitionCount = 0;
    bool bPARFiles = false;
	if( SpecialFacilitiesPartitionCount > 0 ) // есть спецсредства
	 {
		// выделим память под список аббевиатур спецсрредств
        SpecialFacilitiesPartitions = new AnsiString[SpecialFacilitiesPartitionCount];
     }
    // если не отладка
	if( !bPartitionDebug )
	 {
	   // в данный момент обрабатываются файлы настройки РЭП
       bPARFiles = true;
	   if( bUseAllPartitionsFromSPO )
		  {
			AllSPODirAbbrListElement = AllSPODirAbbrList.First();
		  }
	   RefAbbrListElement = RefAbbrList.First();
	   // сбросим отчет файлов настройки в начало
       SessionAdjustmentNames.Reset();
	 }
    else
      {
        bProcessDebugAdjustment = false;
      }
    int nSpecialFacilitiesCounter = 0;
    // цикл по файлам настройки
    while (1)
     {
	   if (bPartitionDebug)  // если отладка
		{
		  // если настройку отладки уже обработали
		  if( bProcessDebugAdjustment )
			  {
				break;
			  }
		  // сформируем имя файла
		  FileName = DebugAdjustment;
		  ErrorMsg = "";
		  bProcessDebugAdjustment = true;
		}
	   else
		{
		  // если в данный момент обрабатываются файлы настройки РЭП
		  if( bPARFiles )
           {
			   if( bUseAllPartitionsFromSPO )
				{
				  // если список окончился
				  if( AllSPODirAbbrListElement == NULL )
					{
						bPARFiles = false;
					}
				  else
					{
					  AdjustmentName = AllSPODirAbbrListElement->String;      // имя
					  // перейдем к след. элементу
                      AllSPODirAbbrListElement = AllSPODirAbbrListElement->Next;
                      // сформируем имя файла
                      FileName = ParameterSPOLocation + AnsiString("\\") + AdjustmentName + AnsiString("\\") + AdjustmentName + ".set";
                    }
                }
                else
                { // вытащим имя файла настройки
                  AdjustmentName = SessionAdjustmentNames.GetAdjustmentName();
                  // если имени нет
                  if (AdjustmentName.IsEmpty())
                    { // окончили работу с файлами настройки РЭП
                        bPARFiles = false;
                    }
                  else
                    { // сформируем имя файла
                       FileName = ParameterAdjustmentsLocation + AnsiString("\\") + AdjustmentName + ".set";
                    }
                }
                if (bPARFiles)
                {
                    ErrorMsg = "";
                }
            }
            // если перешли к файлам настройки пользователя
            if (!bPARFiles)
            { // если текущий элемент есть
              if (RefAbbrListElement != NULL)
               { // имя
                 Abbr = RefAbbrListElement->String;
                 // перейдем к след. элементу
                 RefAbbrListElement = RefAbbrListElement->Next;
                 // сформируем имя файла
                 FileName = ParameterSPOLocation + AnsiString("\\") + Abbr + AnsiString("\\") + Abbr + ".set";
                 // по просьбе Москаленко сообщение не выводим
//                    ErrorMsg = "Не могу открыть файл \"" + FileName + "\". Аббревиатура \"" + Abbr + "\" не обработана.";
                 ErrorMsg = "";
               }
              else
                  { // окончим цикл
                    break;
                  }
            }
        }
        // откроем файл
/*        nFileHandler = FileOpen(FileName, fmOpenRead + fmShareDenyWrite);
        // если неудачно
        if (nFileHandler == -1) {
            if (ErrorMsg != "") {
                ErrorMessageBox(ErrorMsg, ProcessTitle);
            }
            // обрабатываем список разделов дальше
            continue;
        } */

  HANDLE hFile;
  hFile = CreateFile( FileName.c_str(),           // open
					  GENERIC_READ,               // open for reading
					  FILE_SHARE_READ,            // share for reading
					  NULL,                       // no security
					  OPEN_EXISTING,              // existing file only
					  FILE_ATTRIBUTE_NORMAL,      // normal file
					  NULL );                     // no attr. template

		if( hFile == INVALID_HANDLE_VALUE )
		 {
			if( ErrorMsg != "" )
			 {
				ErrorMessageBox( ErrorMsg, ProcessTitle );
				WriteToDebugFile( ErrorMsg );
			 }
			continue;                             // обрабатываем список разделов дальше
		 }
        // номер строки
        nStringNumber = 0;
        bProcessPartition = false;
        // цикл по чтению строк
        while (1)
        {
            nErrorCode = 0;
            // прочитаем строку
            nStringLength = ReadStringFromFileWin32(hFile, acReadString, nErrorCode);
            // если ошибка при чтении
            if (nStringLength < 0)
             { // выйдем из цикла
               break;
             }
            // если строк больше нет
            if (nStringLength == 0)
            { // если идет обработка раздела
              if (bProcessPartition)
               { // увеличим кол-во разделов
                 nPartitionNumber++;
               }
               break;
            }
            // если не паспорт
			if( nStringNumber > 5 )
            { // создадим AnsiString-версию строки, считанной из файла
                ReadString = acReadString;
				//WriteToDebugFile("ReadString = " + ReadString);
                // разобьем строку
				SeparatedString.Abbr.SetLength(0);
				if( SeparateAdjustmentString( ReadString, SeparatedString ) == -1 )
				  continue;
                // вычленим чтобы не добираться каждый раз через структуру
				Directory = SeparatedString.Directory;
                // перебор по типу строки
 another_label:
                if (SeparatedString.cType == SpecialFacilities)
                {
                    // если первый символ - это символ общедоступной директории
                    if (!Directory.IsEmpty())
                    {
                      if (Directory[1] == cCommonDirectorySymbol)
                        { // если есть параметр "РАЗМЕЩЕНИЕ СПО="
                            if (!ParameterSPOLocation.IsEmpty())
							{   //ShowMessage("ИСР: записано разм.СПО");
                                // обрежем директорию на один символ слева
                                Directory = Directory.SubString(2, Directory.Length() - 1);
                                // добавим директорию в список общедоступных
                                CommonDirectories += ParameterSPOLocation + "\\" + SeparatedString.Abbr + "\\"+ Directory + ";";
                            }
                        }
                    }
                    SpecialFacilitiesPartitions[nSpecialFacilitiesCounter++] = SeparatedString.Abbr;
                }
                if (SeparatedString.cType == '\0')
                { // если есть аббревиатура
                  if (!SeparatedString.Abbr.IsEmpty())
                   { // если обработка нового раздела не начата
                     if (!bProcessPartition)
                      { // начнем обработку нового раздела
                        bProcessPartition = true;
                        TempPartitions[nPartitionNumber].Abbr = SeparatedString.Abbr;
                        TempPartitions[nPartitionNumber].StructureName = SeparatedString.StructureName;
                        TempPartitions[nPartitionNumber].Directories = new AnsiString[MaxDirCount];
                        TempPartitions[nPartitionNumber].bAlienDirectory = new bool[MaxDirCount];
                        TempPartitions[nPartitionNumber].bCommonDirectory = new bool[MaxDirCount];
                        TempPartitions[nPartitionNumber].nDirectioriesCount = 0;
                        TempPartitions[nPartitionNumber].AdjustmentName = FileName;
                        TempPartitions[nPartitionNumber].bIsHEADAdjustment = bPARFiles;
                        AddDirectory(Directory, nPartitionNumber, TempPartitions);
                       }
                      // идет обработка раздела
                     else
                      { // остановим обработку
                        bProcessPartition = false;
                        // номер раздела
                        nPartitionNumber++;
                        // обработаем эту же строку по иному
                        goto another_label;
                      }
                    }
                    // нет аббревиатуры
                    else
                     { // если обработка нового раздела начата
                        if (bProcessPartition)
                         { // добавим директорию
                            AddDirectory(Directory, nPartitionNumber, TempPartitions);
                         }
                     }
                }
            }
            nStringNumber++;
        }
        // закроем файл
        CloseHandle(hFile);
        nRealPartitionCount++;
    }
    nRealPartitionCount = nPartitionNumber;
    // удалим память, выделенную под разделы
    DeletePartitionsMemory();
    // выделим память под разделы
    Partitions = new PartitionRecord[nRealPartitionCount];
	if(Partitions == NULL)
        { ShowMessage("ИСР: Ош.при формировании списка разделов");
          goto E;
        }
    // скопируем информацию о разделах
    for (i = 0; i < nRealPartitionCount; i++)
      {
        Partitions[i] = TempPartitions[i];
      }
    // удалим временный массив разделов
 E: delete[] TempPartitions;
    // кол-во разделов в системе
    nTotalPartitionCount = nRealPartitionCount;

}
//------------------------------------------------------------------------------
// функция разбивает строку файла настройки SourceString на поля
// результат разбиения помещается в SeparatedString
// возвращаемое значение: 0 - разбиение удачное
//                       -1 - нарушена структура строки
//------------------------------------------------------------------------------
int __fastcall SeparateAdjustmentString( AnsiString& SourceString, SeparatedAdjustmentString& SeparatedString)
{
    int nCurrentField = 0;
	int nPos = 1;
    int nFieldStart = 1;
    AnsiString SubString;
    int SourceStringLength = SourceString.Length();
  // цикл по символам строки SourceString
	while (1)
	{
	  if (SourceString[nPos] == cFieldDelimiter)                                // если разделитель
	   {
		 SubString = SourceString.SubString( nFieldStart, nPos - nFieldStart ); // выделим подстроку
		 if( SubString.Length() > anAdjustmentMaxLength[nCurrentField] )        // ее длина > макс.возм.?
		  {
			WriteToDebugFile( "ИСР: длина строки наименования раздела " + SubString + " больше допустимой"  );
			return (-1);
		  }
		 // если комментарий
		 if (IsComment(SubString))
		  {
			SubString = "";
		  }
		 // перебор по полю
		 switch (nCurrentField)
		  {
			case 0: // поле "Тип"
					if (SubString.IsEmpty())            // если строка пуста
					 {
						SeparatedString.cType = '\0';
					 }
					else
					 {
						SeparatedString.cType = SubString[1];
					 }
					break;
			case 1: // поле "Аббревиатура"
					SeparatedString.Abbr = SubString;
					break;
			case 2: // поле "Директория"
					SeparatedString.Directory = SubString;
					break;
			case 3: // поле "Структура"
					SeparatedString.StructureName = SubString;
					break;
		  }
		 nFieldStart = nPos + 1;          // начало след. поля
		 if( nCurrentField == 3 )         // если заполнили поле 3
			  {
				break;                    // конец разбора
			  }
		 nCurrentField++;                 // перейдем к след. полю
	   }
	  // если рассмотрели последний символ
	  if( nPos == SourceStringLength )
		{
			break;
		}
	  nPos++;                             // перейдем к след. символу
	}  // while(1)
 // если рассмотрели меньше 4-х полей
  if( nCurrentField < 3 )
	 {
		return (-1);
	 }
	// все ОК
  return (0);
}
//------------------------------------------------------------------------------
// функция добавляет директорию Directory в список директорий в раздела
// и инкрементирует кол-во директорий в разделе
// nPartitionNumber - номер раздела
void __fastcall AddDirectory(AnsiString& Directory, int nPartitionNumber, PartitionRecord* PartitionsArray)
{
    // длина директории
    int nDirectoryLength = Directory.Length();
    // если длина равна 0
    if (nDirectoryLength == 0)
     { // выйдем
        return;
     }
    // если первый символ - это символ заимствованной директории
    if (Directory[1] == cAlienDirectorySymbol)
     {
        // обрежем директорию на один символ слева
        Directory = Directory.SubString(2, nDirectoryLength - 1);
        // флаг заимствованной директории
        PartitionsArray[nPartitionNumber].bAlienDirectory[PartitionsArray[nPartitionNumber].nDirectioriesCount] = true;
        // флаг общей директории
        PartitionsArray[nPartitionNumber].bCommonDirectory[PartitionsArray[nPartitionNumber].nDirectioriesCount] = false;
     }
    else
     {
        PartitionsArray[nPartitionNumber].bAlienDirectory[PartitionsArray[nPartitionNumber].nDirectioriesCount] = false;
        // если первый символ - это символ общедоступной директории
        if (Directory[1] == cCommonDirectorySymbol)
         {
            PartitionsArray[nPartitionNumber].bCommonDirectory[PartitionsArray[nPartitionNumber].nDirectioriesCount] = true;
            // если есть параметр "РАЗМЕЩЕНИЕ СПО="
            if (!ParameterSPOLocation.IsEmpty())
            { // обрежем директорию на один символ слева
              Directory = Directory.SubString(2, nDirectoryLength - 1);
              // добавим директорию в список общедоступных
              CommonDirectories += ParameterSPOLocation + "\\" + PartitionsArray[nPartitionNumber].Abbr + "\\" + Directory + ";";
            }
        }
        else
        {
            PartitionsArray[nPartitionNumber].bCommonDirectory[PartitionsArray[nPartitionNumber].nDirectioriesCount] = false;
        }
	 }
    PartitionsArray[nPartitionNumber].Directories[PartitionsArray[nPartitionNumber].nDirectioriesCount] = Directory;
    PartitionsArray[nPartitionNumber].nDirectioriesCount++;
    //AnsiString strToDebugFile = "Add directory to partition " + PartitionsArray[nPartitionNumber].Abbr + ", Dir: " + Directory;
    //WriteToDebugFile(strToDebugFile);
}
// функция определяет номер раздела по аббревиатуре раздела
// в случае успеха возвр. номер
// если такой аббревиатуры нет, возвращается -1
int __fastcall GetPartitionNumber(AnsiString PartitionAbbr)
{
    // цикл по всем разделам
	for (int i = 0; i < nTotalPartitionCount; i++)
	{
		if (Partitions[i].Abbr == PartitionAbbr)
		{
			return (i);
        }
    }
    return (-1);
}
//---------------------------------------------------------------------------
// функция читает трафарет из файла с именем StencilFileName в переменную Stencil
// возвращает результат
// 0 - трафарет прочитан нормально
// -1 - ошибка при открытии файла
// -2 - ошибка при чтении из файла
int __fastcall ReadStencilFromFile( AnsiString StencilFileName, AnsiString& ReadStencil )
{
    // дескриптор
    int nStencilFileHandler;
    // результат
    int nResultCode = 0;
    // откроем файл
	nStencilFileHandler = FileOpen( StencilFileName, fmOpenRead + fmShareDenyWrite );
    // буфер для чтения трафарета
	char* acBuffer = new char[StencilLength + 1];
	if( acBuffer == NULL )
	 {  // КЗ
		nResultCode = -1;
	 }
	else
    // если неудачно
	if( nStencilFileHandler == -1 )
	 {  // КЗ
		nResultCode = -1;
	 }
	else
	 {   memset( acBuffer, 0, StencilLength + 1 );
		// прочитаем StencilLength символов
		int nRealStencilLength = FileRead( nStencilFileHandler, &acBuffer[0], StencilLength );
		// шаблон
		 /* ReadStencil = AnsiString::StringOfChar( 'X', StencilLength);
			for (int i = 0 ; i < StencilLength; i++)
            {
				char c = acBuffer[i];
				if( c == ' ' )
						 nResultCode = -2;
				ReadStencil[i + 1] = c;
                c = ReadStencil[i + 1];
                c = ReadStencil[i + 1];
			} */
		ReadStencil = acBuffer;
	 }
    // закроем файл
	FileClose(nStencilFileHandler);
	delete [] acBuffer;
    return (nResultCode);
}
//-----------------------------------------------------------------
// функция возвращает имя текущего протокола
AnsiString __fastcall GetProtokolName()
{
    return (TBProtokolNamePart1 + "." + TBProtokolNamePart2);
}
//-----------------------------------------------------------------
// функция возвращает заголовок загруженного раздела nPartitionNumber
AnsiString __fastcall GetPartitionTitle(int nPartitionNumber)
{
    AnsiString ReturnTitle;
    // если есть загруженые разделы
    if (nPartitionCount > 0)
     { // если раздел верхнего уровня и он загружен из ИСР
       if (nPartitionNumber == 0 && bMainPartitionLoadedFromISR)
        { // заголовок, введенный в окне "Заголовок"
          ReturnTitle = MainPartitionTitle;
        }
       else
        { // аббревиатура раздела
          ReturnTitle = LoadedPartitions[nPartitionNumber].Abbr;
        }
     }
    return (ReturnTitle);
}
//-----------------------------------------------------------------
// функция определяет максимальный номер протокола по трафарету
// если все протоколы исчерпаны, возвращается значение "Occupied"
AnsiString __fastcall FindMaxProtokolNumber(AnsiString ProtokolDir, AnsiString Stencil)
{
	// части 1 и 2 трафарета
	int j = Stencil.Pos('-');
	AnsiString Part1 = Stencil.SubString( 1, j - 1 );
	AnsiString Part2 = Stencil.SubString( j + 1, 4);
	int nPart2;
	try
	{
	   nPart2 = StrToInt(Part2);
	}
	catch(... )
	{
	   WarningMessageBox( "Некорректный номер протокола", "Ошибка" );
	}
    AnsiString Directory, FindFileName;
	TSearchRec SearchRec;
    bool bFindResult;
    // ищем
	while (1)
	{
        // если пространство имен протоколов исчерпано
		if (nPart2 >= 9999)
		{
            return ("Occupied");
        }
        Directory = ProtokolDir;
		if (Directory.IsEmpty())
		{ // имя файла
            FindFileName = "*." + Part1 + "-" + StringWithLeadZero4(nPart2) + "*" + ProtokolExtention;
        }
		else
		{  // имя файла
            FindFileName = Directory +  "\\" +  "*." + Part1 + "-" + StringWithLeadZero4(nPart2) + "*" + ProtokolExtention;
        }
//        FindFileName = Directory +  "\\" +  "*." + Part1 + "-" + StringWithLeadZero4(nPart2) + ProtokolExtention;
        bFindResult = FindFirst(FindFileName, faAnyFile, SearchRec) == 0;
        FindClose(SearchRec);
        // если файла нет
		if (!bFindResult)
		{
            break;
        }
        // перейдем к след. файлу
        nPart2++;
    }
    return (Part1 + "-" + StringWithLeadZero4(nPart2));
}
//---------------------------------------------------------------------------
// функция выводит строку в файл ошибок
//---------------------------------------------------------------------------
void __fastcall WriteToErrorFile( AnsiString OutString )
{
    int nFileHandler;
    AnsiString OutStr, ErrorMessage;
   // если файл есть
	if( FileExists( ErrFileName ) )
	 {
		// открыть файл
		nFileHandler = FileOpen( ErrFileName, fmOpenReadWrite + fmShareDenyWrite );
		if( nFileHandler == -1 )
		 {
			ErrorMessage = "Ошибка открытия файла \"" + ErrFileName + "\".";
			ErrorMessageBox( ErrorMessage );
            return;
		 }
	 }
	else
	 {
		nFileHandler = FileCreate( ErrFileName );        // создать файл
		if( nFileHandler == -1 )
		 {
			ErrorMessage = "Ошибка создания файла \"" + ErrFileName + "\".";
			ErrorMessageBox( ErrorMessage );
			return;
        }
	 }
    // строка для записи в файл ошибок
    OutStr = Date().DateString() + " " + Time().TimeString() +  " " + OutString + "\r\n";
	// сдвинем указатель в конец файла
    FileSeek(nFileHandler, 0, 2);
    // сохраним сообщение об ошибке
	MyFileWrite(nFileHandler, OutStr.c_str(), OutStr.Length());
    FileClose(nFileHandler);
}
//---------------------------------------------------------------------------
void __fastcall SnapFoto( AnsiString Message )
{
    char pcBuffer[1000];
	sprintf( pcBuffer, "nPartitionCount = %d, nCurrentPartition = %d, nSelectedPartitionNumber = %d, ISRCurrentState = %d, bMainPartitionStructureExists = %d, bMainPartitionStructureLoaded = %d, bMainPartitionLoadedFromISR = %d, bPrisLoaded = %d, bPartitionDebug = %d", nPartitionCount, nCurrentPartition, nSelectedPartitionNumber, ISRCurrentState, bMainPartitionStructureExists, bMainPartitionStructureLoaded, bMainPartitionLoadedFromISR, bPrisLoaded, bPartitionDebug);
	WriteToDebugFile( "Моментальный снимок в ситуации: " + Message );
	WriteToDebugFile( pcBuffer );
}
//---------------------------------------------------------------------------
// функция выводит строку в файл отладки
//---------------------------------------------------------------------------
void __fastcall WriteToDebugFile( AnsiString OutString )
{
    int nFileHandler;
    AnsiString OutStr, ErrorMessage;
  // если файл есть
	if (FileExists(DebugFileName))
     {
        // попробуем открыть файл
		if ((nFileHandler = FileOpen(DebugFileName, fmOpenReadWrite + fmShareDenyNone)) == -1)
		{
			ErrorMessage = "Ошибка открытия файла \"" + DebugFileName + "\".";
//            ErrorMessageBox(ErrorMessage);
            return;
        }
    }
	else {
        // попробуем создать файл
		if ((nFileHandler = FileCreate(DebugFileName)) == -1)
		{
			ErrorMessage = "Ошибка при создании файла \"" + DebugFileName + "\".";
//            ErrorMessageBox(ErrorMessage);
            return;
        }
    }
    // строка для записи в файл ошибок
    OutStr = Date().DateString() + " " + Time().TimeString() +  " " + OutString + "\r\n";
    // сдвинем указатель в конец файла
    FileSeek(nFileHandler, 0, 2);
    // сохраним сообщение об ошибке
    MyFileWrite(nFileHandler, OutStr.c_str(), OutStr.Length());
    FileClose(nFileHandler);
}
//------------------------------------------------------------------------------
// функция выводит строку в файл, обрабатывает ошибку записи и возвр. результат
//------------------------------------------------------------------------------
bool __fastcall MyFileWrite( int nFileHandler, const void *pBuffer, int nCount )
 {
    int nWriteBytes = FileWrite(nFileHandler, pBuffer, nCount);
    // если КЗ = -1
	if( nWriteBytes == -1 )
	 {
		ErrorMessageBox( "Ошибка записи на диск" );
		return (false);
	 }
    // если байтов записано меньше
	if( nWriteBytes < nCount )
	 {
        ErrorMessageBox( "Ошибка записи на диск" );
        return (false);
	 }
    return (true);
 }
//---------------------------------------------------------------------------
// в ПРИС посылаются общедоступные директории
// в списке директорий сначала идут общедоступные директории раздела,
// затем - общедоступные директории со '*'
//---------------------------------------------------------------------------
void __fastcall SendDirectoriesToPris(void)
{
    AnsiString DirectoriesList, Directory, FullDirName;
    AnsiString ErrorMessage, WarningMessage;
	if( nPartitionCount > 0 )
	 {
	   // указатель на запись о текущем разделе
	   PartitionRecord* pCurrentPartition = &LoadedPartitions[nCurrentPartition];
	   WriteToDebugFile( "nDirectioriesCount = " + IntToStr(pCurrentPartition->nDirectioriesCount) );
	   // цикл по директориям
	   for( int i = 0; i < pCurrentPartition->nDirectioriesCount; i++ )
		{
		  if( !pCurrentPartition->bCommonDirectory[i] )         // если не общедоступная директория
		   {
			 if( bPartitionDebug )                              // если отладка раздела
			  {
				  if( pCurrentPartition->bAlienDirectory[i] )   // если заимствованная директория
					{
					  if( !ParameterSPOLocation.IsEmpty() )     // если параметр "Размещение СПО" есть
					   {  // если она есть
						 if( FindDir( pCurrentPartition->Directories[i], FullDirName ) )
							{
								DirectoriesList += ParameterSPOLocation + "\\" + FullDirName + ";";
							}
					   }
					}
					else
					{
						DirectoriesList += DebugDir + "\\" + pCurrentPartition->Directories[i] + ";";
					}
			  }
			 else                                               // не отладка
			  {
				if( !ParameterSPOLocation.IsEmpty() )           // если параметра о расположении СПО есть
				 {
				   if( pCurrentPartition->bAlienDirectory[i] )  // если заимствованная директория
					{
					  if( FindDir( pCurrentPartition->Directories[i], FullDirName ) ) // если она есть
						 DirectoriesList += ParameterSPOLocation + "\\" + FullDirName + ";";
					  else
					   {
						 ErrorMessage = "В разделе \"" + pCurrentPartition->Abbr
						 + "\" в качестве заимствованной директории указана директория\"" + pCurrentPartition->Directories[i] + "\". Данная директория в каталоге СПО не найдена";
						 ErrorMessageBox( ErrorMessage, "Ошибка" );
						 WriteToDebugFile( ErrorMessage );
					   }
					}
				   else
					 DirectoriesList += ParameterSPOLocation + "\\" + pCurrentPartition->Abbr + "\\" + pCurrentPartition->Directories[i] + ";";
				 }
			  }
		   }
		}
	 }
    // добавим "общедоступные" директории (со звездочкой)
    DirectoriesList = CommonDirectories + DirectoriesList;
	if( ISRMainForm->pr_vosst )
			   return;            // восстанавливаемся
	//
	if( !DirectoriesList.IsEmpty() )
	 {
		DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 2, тело атома - \""
					   + DirectoriesList + "\", суть - список директорий";
		//Por->AddAtomA(1, 1, 2)->Append(DirectoriesList);
	 }
	else
	 {  // ждём ответ, если начало раздела
		DebugString = "Послана порция. Род: 1 , сорт: 1, тип атома: 2, суть - пустой список директорий";
		//Por->AddAtomA(1, 1, 2);
	 }
	// пошлем порцию
	if( ISRMainForm->net_PRIS->OutSocket )
	 {
	   ISRMainForm->net_PRIS->SndPacket( DirectoriesList.c_str(), DirectoriesList.Length(),
												   nISRID, AportID, 1, 2, 1, 1, 0, 0, 0 );
	   // ISRMainForm->nR=2; // тип операции - список директорий
	   WriteToDebugFile(DebugString);
	 }
	else
	   WriteToDebugFile( "Не передали список директорий (" + DirectoriesList + ") в ПРИС. Не установлена связь !");
}
//---------------------------------------------------------------------------
// в ПРИС посылается заголовок
//---------------------------------------------------------------------------
void __fastcall SendWindowTitleToPris(void)
{
    AnsiString ProtokolName;
    // если разделов больше нет
    if (nPartitionCount == 0)
                    ProtokolName = "";
	else
	  ProtokolName = GetProtokolName();
	if(ISRMainForm->pr_vosst)
		  return; // восстанавливаемся
     // пошлем порцию Por->Send();
   if( ISRMainForm->net_PRIS->OutSocket )
	  ISRMainForm->net_PRIS->SndPacket( ProtokolName.c_str(), ProtokolName.Length(),
										nISRID, AportID, 1, 3, 1, 1, 0, 0, 0 );
    ISRMainForm->nR = 3; // тип операции - СП
	DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 3, тело атома - \""
			  + ProtokolName + "\",  суть - заголовок раздела";
	WriteToDebugFile( DebugString );
}
//---------------------------------------------------------------------------
// послать КО в ПРИС
// Comment - коментарий. если пустой, то выводится стандартный коментарий
//---------------------------------------------------------------------------
int __fastcall SendKOToPris( AnsiString KO, bool bExternalOperation, AnsiString Comment, bool bAuto )
{
	bool  KO_Empty = false;                     // признак пустой операции
    AnsiString FullComment = "ИСР: Директива";
	if( ISRMainForm->pr_vosst )
			 return 0;                          // восстанавливаемся
	if( !Comment.IsEmpty() )
			FullComment += " (" + Comment + ")";
	FullComment = FullComment.UpperCase();
    AnsiString Str = KO + '\0' + FullComment + '\0';
	if( KO.IsEmpty() )                           // пустая операция
		   KO_Empty = true;
	char atom;
	if( !KO_Empty )                              // есть непустая операция
	 {
	   if ( bAuto && KO.SubString(1, 7).AnsiCompareIC(ProgramCallStament) == 0 )
		{
		  DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 9, тело атома - \"" + Str + "\", суть - КО ВЫЗВАТЬ";
		  atom = 9;
		  ISRMainForm->nR = 9;                   // тип операции  КО ВЫЗВАТЬ
		}
	   else
		{
		  DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 1, тело атома - \"" + Str + "\", суть - КО";
		  atom = 1;
		  // тип операции  КО не меняем при СП из меню
		  if( ISRMainForm->nR != 8 )
                    ISRMainForm->nR = 1;
		}
	 }
   if( bExternalOperation )   // не команда от РЭП или команда от РЭП ожидает, а от ИСР - новая команда
	{
	  bSendInternalOperationToPris = !bExternalOperation;
	  externalOperation[totalOperationsInPris] = bExternalOperation;
	  totalOperationsInPris++;
	  // WriteToDebugFile(DebugString);
	}
   WriteToDebugFile(DebugString);
  // пошлем порцию
	if( !KO_Empty )
	 {
	   if( ISRMainForm->net_PRIS->OutSocket )
		 ISRMainForm->net_PRIS->SndPacket( Str.c_str(), Str.Length(), nISRID, AportID, 1, atom, 1, 1, 0, 0, 0);
     }
    else
	   ISRMainForm->TestTest6Click(0);
    return (0);
}
//---------------------------------------------------------------------------
// пошлем начало раздела в ПРИС
//---------------------------------------------------------------------------
void __fastcall SendBeginPartitionToPris( AnsiString PartitionTitle )
{
  if( ISRMainForm->pr_vosst )
				   return;             // восстанавливаемся
  if( ISRMainForm->net_PRIS->OutSocket )
	ISRMainForm->net_PRIS->SndPacket( PartitionTitle.c_str(), PartitionTitle.Length(),
															 nISRID, AportID, 1, 4, 1, 1, 0, 0, 0 );
  ISRMainForm->nR = 4; // тип операции - начало раздела
  DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 4, тело атома - \"" + PartitionTitle + "\", суть - начало раздела";
  ISRMainForm->PrepareHeadline->Enabled = false;
  WriteToDebugFile(DebugString);
}
//---------------------------------------------------------------------------
// пошлем конец раздела в ПРИС
//---------------------------------------------------------------------------
void __fastcall SendEndPartitionToPris()
{
  if( ISRMainForm->pr_vosst )
	   return;                          // восстанавливаемся
  if( ISRMainForm->net_PRIS->OutSocket )
	ISRMainForm->net_PRIS->SndPacket( 0, 0, nISRID, AportID, 1, 5, 1, 1, 0, 0, 0 );
  bPrisProcessEndPartition = true;
  ISRMainForm->nR = 5;                  // тип операции - конец раздела
  DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 5, суть - конец раздела";
  WriteToDebugFile( DebugString );
}
//---------------------------------------------------------------------------
// принимаем: 0 - не надо начинать новый раздел
//            1 - надо начинать новый раздел
//---------------------------------------------------------------------------
void __fastcall SendAnswerToPartitionOperationToPris( int nAnswer, AnsiString PartitionTitle )
{
    char* buf = 0;
    int l = 4;
    buf = new char[PartitionTitle.Length() + 4];
    ((int*)buf)[0] = nAnswer;
	if( ISRMainForm->pr_vosst )
				 return;                  // восстанавливаемся
	if( nAnswer == 0 )
	   DebugString = "Послана порция. Адресат - БИНТ. Род: 1 , сорт: 5, тип атома: " + IntToStr(nAnswer)
					  + ", суть - ответ на ПУСКРАЗД";
	else
	 {
		DebugString = "Послана порция. Адресат - БИНТ. Род: 1 , сорт: 5, тип атома: " + IntToStr(nAnswer)
		              + ", тело атома - \"" + PartitionTitle + "\", суть - ответ на ПУСКРАЗД";
		memcpy( buf + 4, PartitionTitle.c_str(), PartitionTitle.Length() ) ;
        l += PartitionTitle.Length();
	 }
  if( ISRMainForm->net_PRIS->OutSocket )
	  ISRMainForm->net_PRIS->SndPacket( buf, l, nISRID, BintID, 5, nAnswer, 1, 1, 0, 0, 0 );
  WriteToDebugFile(DebugString);
  delete [] buf;
}
//---------------------------------------------------------------------------------------
// функция выводит строку в протокол ПРИСа
//----------------------------------------------------------------------------------------
void __fastcall SendStringToPrisProtokol( AnsiString OutputString, StringType OutputStringType, bool bPrintAppTitle, bool bPrintTime, bool bAdditionalIndent)
{
	char pcOutBuffer[200];
	int nOutBufferLength, l;
	if( OutputString.IsEmpty() )
		return;
 	memset( pcOutBuffer, 0, 200 );
	// внутренний вариант выводимой строки
	AnsiString InternalOutputString = OutputString;
	if (bPrintAppTitle)
	 {
        InternalOutputString = AppTitle + ": " + InternalOutputString;
	 }
    // если выводим время
	if( bPrintTime )
	 {
		TDateTime CurTime = Time();
        AnsiString Time = CurTime.FormatString( "hh.mm.ss" );
        InternalOutputString = Time + "  " + InternalOutputString;
	 }
	// если нужен доп. отступ
	if( bAdditionalIndent )
	 {
        InternalOutputString = AdditionalProtokolIndent + InternalOutputString;
	 }
	// отступ
    InternalOutputString = ProtokolIndent + InternalOutputString;
    InternalOutputString = InternalOutputString.UpperCase();
	// выделим память под буфер
	l = InternalOutputString.Length();
	nOutBufferLength = l + 3 + 2;
	memcpy( pcOutBuffer + 2 + 2, InternalOutputString.c_str(), l );  // выводимая строка
	//
	if(ISRMainForm->pr_vosst)
			   return;                                               // восстанавливаемся
	*((short*)(pcOutBuffer)) = nOutBufferLength;                     // длина передаваемой инф-и
	*((short*)(pcOutBuffer + 2)) = (short)OutputStringType;          // тип выводимой строки
	// пошлем порцию
	try {
		 if( ISRMainForm->net_PRIS->OutSocket == 0 )
		  {
			 ISRMainForm->net_PROT->OutSocket->Close();
			 return;
		  }
		  if( ISRMainForm->net_PRIS->OutSocket )
			  WriteToDebugFile( "ISRMainForm->net_PRIS->OutSocket != 0" );
		  if( ISRMainForm->net_PROT->OutSocket )
			  WriteToDebugFile( "ISRMainForm->net_PROT->OutSocket != 0" );
		  if( ISRMainForm->net_PRIS->OutSocket && ISRMainForm->net_PROT->OutSocket && ( ISRMainForm->KS == 0 ) )
		   {
			 DebugString = "Послылаем порцию Bint из SendStringToPrisProtokol(). Род: 1 , сорт: 1, тип атома: 2, суть - строка в протокол";
			 WriteToDebugFile(DebugString);
			 ISRMainForm->net_PROT->out( pcOutBuffer, nOutBufferLength );
			 DebugString = "Послана порция. Адресат - Bint(net_PROT). Род: 1 , сорт: 1, тип атома: 2, суть - строка в протокол";
			 WriteToDebugFile(DebugString);
           }  
		  else
			 WriteToDebugFile( "OutSocket = 0" );
        }
    catch(...)
	 {
	   WriteToDebugFile( "ИСР(SendStringToPrisProtokol): Сбой при посылке строки в протокол ПРИСа" );
     }
}
//------------------------------------------------------------------------------
// посылка сигнала "очистить" в ИСР
//------------------------------------------------------------------------------
void __fastcall SendClearOperationToISR()
{
    /*Por->Clear();
    Por->AddrTo = nISRID;
    Por->AddrFrom = nISRID;
    Por->AddAtomA(2, 1, 0);
    Por->Send();*/
}
//------------------------------------------------------------------------------
// пошлем запрос тек. каталога в ПРИС
//------------------------------------------------------------------------------
void __fastcall SendRequestCurrentPartitionToPris()
{
  if( ISRMainForm->pr_vosst )
		return;                            // восстанавливаемся
  if( ISRMainForm->net_PRIS->OutSocket )
	ISRMainForm->net_PRIS->SndPacket( 0, 0, nISRID, AportID, 1, 6, 1, 1, 0, 0, 0 );
    DebugString = "Послана порция. Адресат - Aport. Род: 1 , сорт: 1, тип атома: 6, суть - запрос тек. каталога";

  WriteToDebugFile(DebugString);
}
//------------------------------------------------------------------------------
// пошлем ответ на дир. ЭД
//------------------------------------------------------------------------------
void __fastcall SendEDDirAnswerToPris(int nAnswer)
{
  if( ISRMainForm->pr_vosst )
	 return;                              // восстанавливаемся
  if( ISRMainForm->net_PRIS->OutSocket )
	ISRMainForm->net_PRIS->SndPacket( 0, 0, nISRID, BintID, 8, nAnswer, 1, 2, 0, 0, 0 );
  DebugString = "Послана порция. Адресат - БИНТ. Род: 2 , сорт: 8, тип атома: "
                   + IntToStr(nAnswer) + ", суть - ответ на дир. ЭД";
  WriteToDebugFile(DebugString);
}
//------------------------------------------------------------------------------
// пошлем ответ на дир. НА_СР
//------------------------------------------------------------------------------
void __fastcall SendNASRDirAnswerToPris( int nAnswer )
{
  if(ISRMainForm->pr_vosst) return; // восстанавливаемся
    /*Por->Clear();
    Por->AddrTo = BintID;
    Por->AddrFrom = nISRID;
	Por->AddAtomA(2, 9, nAnswer);
	Por->Send();  */
  if( ISRMainForm->net_PRIS->OutSocket )
	ISRMainForm->net_PRIS->SndPacket( 0, 0, nISRID, BintID, 9, nAnswer, 1, 2, 0, 0, 0 );
    DebugString = "Послана порция. Адресат - БИНТ. Род: 2 , сорт: 9, тип атома: "
               + IntToStr(nAnswer) + ", суть - ответ на дир. НА_СР";
  WriteToDebugFile(DebugString);
}
//---------------------------------------------------------------------------
// функция возвращает строку с лидирующими нулями для числа
// в результирующем числе не менее 4-х символов
AnsiString __fastcall StringWithLeadZero4(int nNumber)
{
	if (nNumber < 10)
	{
        return ("000" + IntToStr(nNumber));
    }
	else
	{
		if (nNumber < 100)
		{
            return ("00" + IntToStr(nNumber));
        }
		else
		{
			if (nNumber < 1000)
			{
                return ("0" + IntToStr(nNumber));
            }
        }
    }
    return (IntToStr(nNumber));
}
//---------------------------------------------------------------------------
// функция возвращает строку с лидирующими нулями для числа
// в результирующем числе не менее 2-х символов
AnsiString __fastcall StringWithLeadZero2(int nNumber)
{
	if (nNumber < 10)
	{
        return ("0" + IntToStr(nNumber));
    }
    return (IntToStr(nNumber));
}
//---------------------------------------------------------------------------
void DeletePartitionsMemory()
{  int i;
    if (nTotalPartitionCount > 0)
    {  // цикл по всем разделам
        for ( i = 0; i < nTotalPartitionCount; i++)
        { // удалим директории
            delete[] Partitions[i].Directories;
            delete[] Partitions[i].bAlienDirectory;
            delete[] Partitions[i].bCommonDirectory;
        }
        delete[] Partitions;
    }
}

unsigned char pcRus[] = "АВЕКМНОРСТУХаекморсух";
unsigned char pcLat[] = "ABEKMHOPCTYXaekmopcyx";
//---------------------------------------------------------------------------
// функция конвертирует похожие по написанию русские буквы в латинские
unsigned char __fastcall RusToLat(unsigned char cSym)
{
    unsigned char* pcFirst;
    if( (pcFirst = strchr((char*)pcRus, (int)cSym)) != NULL)
        return (pcLat[pcFirst-pcRus]);
    else
        return (cSym);
}
//---------------------------------------------------------------------------
// функция конвертирует похожие по написанию русские буквы в латинские
AnsiString __fastcall StringRusToLat(AnsiString OriginalString)
{
    AnsiString ReturnString;
	for (int i = 1; i <= OriginalString.Length(); i++)
	{
        ReturnString += AnsiString(char(RusToLat(OriginalString[i])));
    }
    return (ReturnString);
}
//---------------------------------------------------------------------------
// функция конвертирует похожие по написанию русские буквы в латинские
unsigned char __fastcall LatToRus(unsigned char cSym)
{
    unsigned char* pcFirst;
    if( (pcFirst = strchr((char*)pcLat, (int)cSym)) != NULL)
        return (pcRus[pcFirst-pcLat]);
    else
        return (cSym);
}
//---------------------------------------------------------------------------
// функция конвертирует похожие по написанию русские буквы в латинские
AnsiString __fastcall StringLatToRus(AnsiString OriginalString)
{
    AnsiString ReturnString;
	for (int i = 1; i <= OriginalString.Length(); i++)
	{
        ReturnString += AnsiString(char(LatToRus(OriginalString[i])));
    }
    return (ReturnString);
}
//---------------------------------------------------------------------------
// функция возвращает имя файла настройки загруженного раздела
// с номером PartitionNumber
AnsiString __fastcall GetAdjustmentFileName(int nPartitionNumber)
{
    // указатель на тек. раздел
    PartitionRecord* pCurrentPartition = &LoadedPartitions[nPartitionNumber];
    AnsiString AdjustmentFileName;
    // если отладка раздела
	if (bPartitionDebug)
	{
		if (nPartitionNumber == 0)
		{
            AdjustmentFileName = DebugAdjustment;
        }
		else
		{
            AdjustmentFileName = "";
        }
    }
	else
	{
        AdjustmentFileName = ParameterSPOLocation + "\\" + pCurrentPartition->Abbr + "\\" + pCurrentPartition->Abbr + AdjustmentExtention;
    }
	if (!FileExists(AdjustmentFileName))
	{
        AdjustmentFileName = "";
    }
    return (AdjustmentFileName);
}
//---------------------------------------------------------------------------
// функция возвращает имя файла структуры загруженного раздела
// с номером PartitionNumber
AnsiString __fastcall GetStructureFileName(int nPartitionNumber)
{
    // указатель на тек. раздел
    PartitionRecord* pCurrentPartition = &LoadedPartitions[nPartitionNumber];
    AnsiString StructureFileName;
    if (pCurrentPartition->StructureName.IsEmpty())
    {
        return ("");
    }
    // если отлака раздела
	if (bPartitionDebug)
	{
		if (nPartitionNumber == 0)
		{
            StructureFileName = DebugDir + "\\" + pCurrentPartition->StructureName + StructureExtention;
        }
		else
		{
            StructureFileName = "";
        }
    }
	else
	{
        StructureFileName = ParameterSPOLocation + "\\" + pCurrentPartition->Abbr + "\\" + pCurrentPartition->StructureName + StructureExtention;
    }
    return (StructureFileName);
}
//---------------------------------------------------------------------------
// функция подсчитывает кол-во символов cSymbol в строке String
int __fastcall SymbolCount(AnsiString String, char cSymbol)
{
    int nCounter = 0;
	for (int i = 1; i <= String.Length(); i++)
	{
		if (String[i] == cSymbol)
		{
            nCounter++;
        }
    }
    return (nCounter);
}
//---------------------------------------------------------------------------
// функция возвращает: выполняется ли в данный момент блок "Авто"
bool __fastcall IsBlockAutoExecuted()
{
    TMDIChild* ch;
    int i;
    // пока считаем, что не выполняется
    bool bExecuteAutoBlock = false;
    if (nPartitionCount > 0)
     { // флаг: загружена структура текущего раздела
       bool bStructureLoaded = !(nCurrentPartition == 0 && !bMainPartitionStructureLoaded);
        // если загр. структура
       if (bStructureLoaded)
        {  // цикл по секциям раздела
           for (i = 0; i < 3; i++)
            {
              ch = LoadedPartitions[nCurrentPartition].pWindow[i];
              // если в секции вып. блок "Авто"
              if (ch->GetExecuteAutoBlock())
               {
                  bExecuteAutoBlock = true;
               }
            }
        }
    }
    return (bExecuteAutoBlock);
}
//---------------------------------------------------------------------------
// функция принимает имя каталога с программами,
// а выдает путь к данному каталогу внутри каталога СПО
// в параметре FullDirName
// результат - найден ли каталог с программами
bool __fastcall FindDir(AnsiString DirName, AnsiString& FullDirName)
{
    TSearchRec SearchRec, SearchRec2;
    AnsiString FindString, FindString2;
    bool bFind = false;
    int nFindResult;
    bool bFirstFind = true;
    bool bFindResult;
   // пока не нашли или не прошли все каталоги
   // ищем в каталоге СПО
    FindString = ParameterSPOLocation + "\\*.*";
	while (1)
	{
		if (bFirstFind)
		{
            // поищем первый файл
            nFindResult = FindFirst(FindString, faDirectory, SearchRec);
            bFirstFind = false;
        }
		else
		{
            nFindResult = FindNext(SearchRec);
        }
        // если нашли что-то
		if (nFindResult == 0)
		{
            // если это директория
			if ((SearchRec.Attr & faDirectory) && SearchRec.Name != "." && SearchRec.Name != "..")
			{
                FindString2 = ParameterSPOLocation + "\\" + SearchRec.Name + "\\" + DirName;
                bFindResult = FindFirst(FindString2, faDirectory, SearchRec2) == 0;
                FindClose(SearchRec2);
                // если в ней есть нужный нам каталог с программами
				if (bFindResult)
				{
                    // если раздел есть в списке разделов или списке спецсредств
					if (IsPartitionInPartListOrInSpecialFacilitiesPartList(SearchRec.Name))
					{
                        // нашли
                        bFind = true;
                        FullDirName = SearchRec.Name + "\\" + SearchRec2.Name;
                        break;
                    }
                }
            }
        }
        // файл (каталог) не найден
		else
		{
            break;
        }
	}
    FindClose(SearchRec);
    return (bFind);
}
//------------------------------------------------------------------------------
// ф-я пров-т, есть ли раздел PartitionName в сп. разделов или сп. спецсредств
//------------------------------------------------------------------------------
bool IsPartitionInPartListOrInSpecialFacilitiesPartList(AnsiString PartitionName)
{
    int i;
	for (i = 0; i < nTotalPartitionCount; i++)
	{
		if (PartitionName == Partitions[i].Abbr)
		{
            return (true);
        }
    }
	for (i = 0; i < SpecialFacilitiesPartitionCount; i++)
	{
		if (PartitionName == SpecialFacilitiesPartitions[i])
		{
            return (true);
        }
    }
    return (false);
}
//------------------------------------------------------------------------------
// ф-я заменяет подстроку из нескольких пробелов в строке String на 1 пробел
//------------------------------------------------------------------------------
void __fastcall ReplaceMultiSpaceToOneSpace(AnsiString& String)
{
    char* pcString = String.c_str();
    char* pcMultiSpaceStart = strstr(pcString, "  ");
	if (pcMultiSpaceStart != NULL)
	{
        int nIndex = 0;
        while (pcMultiSpaceStart[++nIndex] == ' ');
        int nPart1Length = pcMultiSpaceStart - pcString;
        int nSpacesAreaLength = nIndex;
        AnsiString Part1 = String.SubString(1, nPart1Length);
		AnsiString Part2 = String.SubString(nPart1Length + nSpacesAreaLength + 1,
									   String.Length() - nPart1Length - nSpacesAreaLength);
        String = Part1 + " " + Part2;
    }
}
//---------------------------------------------------------------------------
LoadedPartitionRecord& __fastcall LoadedPartitionRecord::operator=(const PartitionRecord& Partition)
{
	Abbr = Partition.Abbr;
	StructureName = Partition.StructureName;
    AdjustmentName = Partition.AdjustmentName;
    nDirectioriesCount = Partition.nDirectioriesCount;
	Directories = Partition.Directories;
    bAlienDirectory = Partition.bAlienDirectory;
    bCommonDirectory = Partition.bCommonDirectory;
    bIsHEADAdjustment = Partition.bIsHEADAdjustment;
    return (*this);
}
//---------------------------------------------------------------------------
void __fastcall AttentionSoundSignal()
{
    MessageBeep(MB_OK);
    MessageBeep(MB_OK);
}
//------------------------------------------------------------------------------
// ф-я проверяет, подходит ли символ для строки параметров оп. "Запуск раздела"
//------------------------------------------------------------------------------
bool __fastcall IsGoodSymbolForPartitionOperationParameters(UCHAR cSym)
{
    if ((cSym >= UCHAR('А') && cSym <= UCHAR('Я')) || (cSym >= '0' && cSym <= '9')
        || (cSym >= 'A' && cSym <= 'Z') || (cSym == ' ')
		|| (cSym == UCHAR('Ё')) || (cSym == '_') || (cSym == '-') || (cSym == '/')
		|| (cSym == '.') || (cSym == '+') || (cSym == ':'))
		 {
		   return (true);
		 }
    else {
		   return (false);
		 }
}
//---------------------------------------------------------------------------
// функция проверяет, являтся ли TestString коментарием
//---------------------------------------------------------------------------
bool __fastcall IsComment(AnsiString TestString)
{
    if (TestString.SubString(1, 2) == "//") {
        return (true);
    }
    else {
        return (false);
    }
}
//---------------------------------------------------------------------------
void KillApplication(HWND hWnd)
{
    // определяем идентификатор процесса и потока, которым
    // принадлежит указанное окно
	DWORD dwProcessId;
	if(hWnd == NULL)
		 WriteToDebugFile("********************Мьютекс занят, но окна нет");
	GetWindowThreadProcessId( hWnd, &dwProcessId );
	__int64 l = dwProcessId;
	WriteToDebugFile("*********************Завершаем процесс, ProcessId = " + IntToStr(l));
    // открываем описатель процесса
    HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,   FALSE, dwProcessId);
    if (hProcess == NULL)
    {
        WriteToDebugFile("********************hProcess == NULL");
    }
    else
    {
        WriteToDebugFile("hProcess " + IntToStr((int)hProcess));
        TerminateProcess(hProcess, (UINT)-1);
    }
    CloseHandle(hProcess);
}

#pragma package(smart_init)
