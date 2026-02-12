//---------------------------------------------------------------------------

#ifndef CommonH
#define CommonH

#include "Sysutils.hpp"
//---------------------------------------------------------------------------
// define
//---------------------------------------------------------------------------
// символы, определяющие тип операции
// сообщения
#define MESSAGE_END_PARTITION WM_USER + 1
#define MESSAGE_END_CALLED_PARTITION WM_USER + 2

//---------------------------------------------------------------------------
// типы
//---------------------------------------------------------------------------
class TMDIChild;

// строка структуры
struct StructureString
{
	AnsiString Field[4];
};

class PartitionRecord
{
public:
	AnsiString Abbr;
	AnsiString StructureName;
    AnsiString AdjustmentName;
    int nDirectioriesCount;
	AnsiString* Directories;
    bool bIsHEADAdjustment;
    bool* bAlienDirectory;
    bool* bCommonDirectory;
};

class LoadedPartitionRecord: public PartitionRecord
{
public:
    TMDIChild* pWindow[3];
    int nSectionFromISRPartitionOperationMake;
    LoadedPartitionRecord& __fastcall operator=(const PartitionRecord& Partition);
};

// информация из строки файла настройки
struct SeparatedAdjustmentString
{
	char cType;
	AnsiString Abbr;
	AnsiString Directory;
	AnsiString StructureName;
};

// информация о посланной операции
struct OperationInPris
{
    int nPartitionNumber;
    int nSectionNumber;
    bool bIsProgramCallOperation;
    bool bIsSPOperation;
    unsigned int nOperationNumber;
};

// информация о блоке НШС
struct NShSBlockInfo
{
    int nBlockNumber;
    int nBlockStart;
    int nBlockEnd;
};

// максимальная длина строки в файле настройки
enum {MaxStringLength = 95};
enum {MAXP = 512};
enum {MaxPartitionCount = 3};
enum {AportID = 105, ShowerID = 104, BintID = 101};
enum {MaxDirCount = 20};
enum {MaxBisectCount = 99};
enum {DialogBoxCount = 10};
enum {MaxNShSBlockCount = 100};

typedef enum _OperationType {otDipolOperation, otISROperation, otVariant, otCommand, otReport, otMessage} OperationType;
typedef enum _WorkMode {wmAuto, wmStep} WorkMode;
typedef enum _ISRState {isNonPartitionSelected, isPartitionSelected, isPartitionCarringOut} ISRState;
typedef enum _EndMessageBoxResult  {erCancel, erEndWithSaveProtokol, erEndWithoutSaveProtokol} EndMessageBoxResult;
typedef enum _KeysPressed  {kpF7, kpF9, kpCtrlF2} KeysPressed;
typedef enum _PartitionListBoxResult  {prCancel, prChoose, prLoad} PartitionListBoxResult;
typedef enum _StringType  {strStandart = 0, stRep = 523, strBlue = 524, strBlackBold = 525, strError = 526} StringType;
typedef enum _EDType {etArticleNShS, etArticleDamage, etArticleGoInitial,
                       etArticleNShS_REP, etArticleGoInitial_REP,
                       etArticleDamage_REP,
                       etPart_REP1, etPart_REP2,
                       etPart_REP3} EDType;
typedef int* PInt;

//---------------------------------------------------------------------------
// константы
//---------------------------------------------------------------------------
// сообщения
#define MESSAGE_CLEAR WM_USER + 4

extern const int anColWidth[];
// имя файла протокола
extern const AnsiString ProtokolFileName;
// символ, обозначающий начало и конец секции
extern const char cSectionBeginEndSymbol;
// символ, разделяющий поля в файле структуры
extern const char cFieldDelimiter;
// символ, обозначающий заимвстванную директорию
extern const char cAlienDirectorySymbol;
// символ, обозначающий общедоступную директориб
extern const char cCommonDirectorySymbol;
// символ, которым отмечается строка в блоке, выполняющимся автоматически
extern const char cAuto;
// символ, разделяющий поля в строке с параметрами директивы начала раздела
extern const char cPartitionDirFieldDelimiter;
// позиция заголовка
extern const int nCaptionLeft;
extern const int nCaptionTop;
// цвета
// фон таблицы
extern const unsigned int nGridBackColor;
// выделенная ячейка
extern const unsigned int nGridSelColor;
// текущая операция
extern const unsigned int nCurOperationBackColor;
// выполненная операция
extern const unsigned int nMadeOperationColor;
// цвет заголовка
extern const unsigned int nHeaderColor;
// цвет шрифта выделенной операции
extern const unsigned int nSelFontColor;
// цвет шрифта невыделенной операции
extern const unsigned int nNonSelFontColor;
// краткие названия секций
extern AnsiString SectionType[3];
// маркер выполненной операции
extern const unsigned char pcMadeMarker[2];
// маркер текущей операции
extern const unsigned char pcCurrentOperationMarker[2];

// расширения
extern const AnsiString ProgramExtention;
extern const AnsiString StructureExtention;
extern const AnsiString AdjustmentExtention;
extern const AnsiString ProtokolExtention;
extern const AnsiString EDFilesExtention;
// формат строки файла настройки
extern const int anAdjustmentMaxLength[];
// символ НШС
extern const char cNShSLabelSymbol;
// флаг: ПРИС загружен и мы можем начинать работу
extern bool bPrisLoaded;
// длина шаблона
extern /*const*/ int StencilLength;

//---------------------------------------------------------------------------
// переменные
//---------------------------------------------------------------------------
// имя директивы ПРИСа, которая запускает раздел
extern AnsiString PartitionStartDirName;
// имя команды ИСР, которая запускает раздел
extern AnsiString ISRStartDirName;
// имя метки секции приведения в исходное
extern AnsiString GoInitialLabel;
// имя метки секции штатного проведения раздела
extern AnsiString NormalWorkLabel;
// имя метки секции НШС
extern AnsiString NShSWorkLabel;
// метка блока НШС
extern AnsiString NShSLabel;
// метка конца блока НШС
extern AnsiString NShSEndLabel;
// точное название операции "Доклад"
extern AnsiString ReportName;
// точное название операции "Команда"
extern AnsiString CommandName;
// точное название операции "Сообщение оператору"
extern AnsiString MessageName;
// точное название операции "Диполь-операция"
extern AnsiString DipolOperationName;
// точное название операции "Директива ИСР"
extern AnsiString ISROperationName;
// точное название метки на запуск раздела в автоматическом режиме
extern AnsiString AutoStartPartitionLabel;
// точное название метки на запуск раздела в шаговом режиме
extern AnsiString StepStartPartitionLabel;
// название операции вызова диполь-программы
extern AnsiString ProgramCallStament;
// название операции сохранения протокола
extern AnsiString SPStament;
// название типа ЭД НШС изделия
extern AnsiString EDNshSArticleName;
// название типа ЭД АВАР изделия
extern AnsiString EDDamageArticleName;
// название типа ЭД ПРИ изделия
extern AnsiString EDGoInitialArticleName;
// точное название директивы "переход на структуру раздела"
extern AnsiString NASRDirName;
// сообщения об ошибках при чтении файла
extern char acFileReadErrorMessage[2][100];

// строка, которая выводится в заголовке окна при выполнении блока "Авто"
extern AnsiString AutoMessage;
// заголовки колонок
extern AnsiString aHeader[3][4];
// позиция главного окна
extern int nMainWindowLeft;
extern int nMainWindowTop;
extern int nMainWindowWidth;
extern int nMainWindowHeight;
// позиция таблицы
extern int nGridLeft;
extern int nGridTop;
extern int nGridWidth;
extern int nGridHeight;
// высота ячейки
extern int nGridRowHeight;
// кол-во строк в сетке
extern int nGridRowCount;
// отступ текста в ячейке таблицы
extern int nCellTextIndent;
// коэфициент, учитывающий размер системного шрифта
extern double dScr;
// размер шрифта ячейки сетки
extern int nCellFontSizeSF;
extern int nCellFontSizeBF;
// размер шрифта заголовка
extern int nCaptionFontSizeSF;
extern int nCaptionFontSizeBF;
// заголовок приложения
extern AnsiString AppTitle;
// имя файла настройки ПРИС
extern AnsiString OnFileName;
// имя системного файла настройки ПРИС
extern AnsiString CfgFileName;
// параметры, получаемые из файла настройки
// параметр "РАЗМЕЩЕНИЕ_ФАЙЛОВ="
extern AnsiString ParameterFilesLocation;
// параметр "РАЗМЕЩЕНИЕ_СПО="
extern AnsiString ParameterSPOLocation;
// параметр "РАЗМЕЩЕНИЕ_РАЗДЕЛОВ="
extern AnsiString ParameterAdjustmentsLocation;
// параметр "РАЗДЕЛЫ="
extern AnsiString ParameterAdjustments;
// параметр НШС_ИЗД
extern AnsiString ParameterNShSArticle;
// параметр АВАР_ИЗД авария
extern AnsiString ParameterDamageArticle;
// параметр ПРИ_ИЗД авария исходное
extern AnsiString ParameterGoInitialArticle;
// параметр "ПРТОКОЛ="
extern AnsiString ParameterProtokol;
// порция для обмена с ПРИС
//extern TPortion* Por;
// кол-во разделов в системе
extern int nTotalPartitionCount;
// список разделов
extern PartitionRecord* Partitions;
// загруженные разделы
extern LoadedPartitionRecord LoadedPartitions[];
// кол-во загруженных разделов
extern int nPartitionCount;
// текущий раздел (порядковый номер)
extern int nCurrentPartition;

// текущая секция
extern int anCurrentSection[];
// номер выбранного раздела
extern int nSelectedPartitionNumber;
// аббревиатуры разделов
extern AnsiString aPartitionAbbr[MaxPartitionCount];

// тип работы - шаговый или автомат
extern WorkMode CurrentWorkMode;
// заголовок раздела
extern AnsiString TBPartitionTitle;
// имя протокола
extern AnsiString TBProtokolNamePart1, TBProtokolNamePart2;
// имя РЭП
extern AnsiString TBRAPName;
// имя оператора ПУ АИС
extern AnsiString TBOperatorPUAISName;
// имя оператора АИС
extern AnsiString TBOperatorAISName;
// имя оператора ТКЦК
extern AnsiString TBOperatorTKCKName;
// имя оператора НТК СИ
extern AnsiString TBOperatorNTKSIName;
// имя оператора БС
extern AnsiString TBOperatorBSName;
// флаг: в системе есть трафарет
extern bool bStencilExists;
// трафарет
extern AnsiString Stencil;


// заголовок головного раздела
extern AnsiString MainPartitionTitle;

// "общедоступные" директории (со звездочкой)
extern AnsiString CommonDirectories;
// наш ID
extern int nISRID;
// состояние интерпретатора
extern ISRState ISRCurrentState;
// флаг: у головного раздела есть стуктура
extern bool bMainPartitionStructureExists;
// флаг: структура головного раздела загружена
extern bool bMainPartitionStructureLoaded;
// имя файла с трафаретом
extern AnsiString StencilFileName;
// имя файла с трафаретом c путем
extern AnsiString FullStencilFileName;
// стек операций, посланных в ПРИС
extern OperationInPris aOperationInPris[];
// кол-во операций в ПРИС
extern int nOperationInPrisCount;
// флаг: головной раздел загружен из ИСР
extern bool bMainPartitionLoadedFromISR;
// имя с путем файла с сообщениями об ошибках
extern AnsiString ErrFileName;
// имя с путем файла с сообщениями об ошибках
extern AnsiString DebugFileName;
// путь к исполняемому файлу ИСР
extern AnsiString ProgramDir;
// номер резки
extern int nBisect;
// время начала раздела
extern unsigned int PartitionStartTickCount;
// флаг: получена порция на закрытие
extern bool bRecievePortionForClose;
// флаг: рабочее место разработчика
extern bool bDeveloperWorkPlace;
// флаг: пришло сообщение "Конец раздела"
extern bool bEndPartitionMessage;
// флаг: отладка радела
extern bool bPartitionDebug;
// файл настройки отлаживаемого раздела
extern AnsiString DebugAdjustment;
// расположение СПО для отлаживаемого раздела
extern AnsiString DebugDir;
// флаг: работаем со всеми разделами из каталога СПО
extern bool bWorkWithAllPartitionsFromSPO;
// флаг: в ПРИС послана а внутреняя операция ISR (не из структуры)
extern bool bSendInternalOperationToPris;
// массив указателей на диалоговые окна ИСР
extern TForm* apDialogBox[];
// текущий список файлов настройки разделов
extern AnsiString CurrentAdjustmentsList;
// параметр настройки: разрешены ли переходы назад при перемещении по секции
extern bool bAllowBackCrossing;
// открыт диалог открытия файла
extern bool bOpenFileDialogOpen;
// коментарии на КО из ИСР
extern AnsiString KOStartComment;
extern AnsiString KOEndComment;
// отступ в протоколе ПРИСа перед операцией
extern AnsiString ProtokolIndent;
// дополнительный отступ в протоколе ПРИСа
extern AnsiString AdditionalProtokolIndent;
// флаг: идет процесс завершения всех разделов
extern bool bEndAllPartitions;
// флаг: ПРИС обрабатывает конец раздела (мы ждем ответа - да или нет)
extern bool bPrisProcessEndPartition;
// флаг: параметр "ПРОТОКОЛ" непустой
extern bool bParameterProtokolExistsAndGood;
// имя файла ПФКС
extern AnsiString PFKSFullFileName;
// hwnd MainForm предыдущей копии редактора
extern HWND hwndPrevInstance;
// флаг: дало ли сохрранять протокол в функции EndCurrentPartition()
extern bool bSaveProtokol;
// имя протокола для сохранения в функции EndCurrentPartition()
extern AnsiString ProtokolName;
// кол-во разделов "Спецсредства"
extern int SpecialFacilitiesPartitionCount;
// аббревиатуры разделов "Спецсредства"
extern AnsiString* SpecialFacilitiesPartitions;
// строка в файл отладки
extern AnsiString DebugString;
extern char DIPOL_OPERATION_SYMBOL; // 'О'
extern char ISR_OPERATION_SYMBOL; // 'И'
extern char VARIANT_SYMBOL; // 'В'
extern char COMMAND_SYMBOL; // 'К'
extern char REPORT_SYMBOL; // 'Д'
extern char MESSAGE_SYMBOL; // 'П'
extern char LABEL_SYMBOL; // 'М'
extern bool externalOperation[100];
extern int totalOperationsInPris;
extern char HandPartition;
extern char CommentString;
extern char ReferenceString;
extern char SpecialFacilities;
extern char HidePartition;
extern TList* HideAbbrList;
//---------------------------------------------------------------------------
// функции
//---------------------------------------------------------------------------
int __fastcall ReadStringFromFile(int nHandler, char* pcBuffer, int& nErrorCode);
bool __fastcall IsEmptyOrNShS(AnsiString pTestString);
int __fastcall IsNShS(AnsiString TestString);
void __fastcall ProcessAdjustments(void);
void __fastcall ErrorMessageBox(AnsiString Message, AnsiString Title = "");
void __fastcall WarningMessageBox(AnsiString Message, AnsiString Title = "");
void __fastcall InformationMessageBox(AnsiString Message, AnsiString Title = "");
int __fastcall QuestionMessageBox(AnsiString Message, AnsiString Title = "", UINT nStyle = MB_OKCANCEL + MB_ICONQUESTION);
int __fastcall SeparateAdjustmentString(AnsiString& SourceString, SeparatedAdjustmentString& SeparateString);
void __fastcall AddDirectory(AnsiString& Directory, int nPartitionNumber, PartitionRecord* PartitionsArray);
int __fastcall GetPartitionNumber(AnsiString PartitionAbbr);
int __fastcall ReadStencilFromFile(AnsiString StencilFileName, AnsiString& ReadStencil);
AnsiString __fastcall GetProtokolName();
AnsiString __fastcall GetPartitionTitle(int nPartitionNumber);
AnsiString __fastcall FindMaxProtokolNumber(AnsiString ProtokolDir, AnsiString Stencil);
void __fastcall WriteToErrorFile(AnsiString OutString);
void __fastcall WriteToDebugFile(AnsiString OutString);
bool __fastcall MyFileWrite(int nFileHandler, const void *pBuffer, int nCount);
int __fastcall SendKOToPris(AnsiString KO, bool bExternalOperation, AnsiString Comment = "", bool bAuto = false);
void __fastcall SendDirectoriesToPris(void);
void __fastcall SendWindowTitleToPris(void);
void __fastcall SendBeginPartitionToPris(AnsiString PartitionTitle);
void __fastcall SendEndPartitionToPris();
void __fastcall SendStringToPrisProtokol(AnsiString OutputString, StringType Type, bool bPrintAppTitle, bool bPrintTime, bool bAdditionalIndent);
void __fastcall SendAnswerToPartitionOperationToPris(int nAnswer, AnsiString PartitionTitle = "");
void __fastcall SendClearOperationToISR();
void __fastcall SendRequestCurrentPartitionToPris();
void __fastcall SendEDDirAnswerToPris(int nAnswer);
void __fastcall SendNASRDirAnswerToPris(int nAnswer);
AnsiString __fastcall StringWithLeadZero4(int nNumber);
AnsiString __fastcall StringWithLeadZero2(int nNumber);
void DeletePartitionsMemory();
AnsiString __fastcall StringLatToRus(AnsiString OriginalString);
AnsiString __fastcall GetAdjustmentFileName(int nPartitionNumber);
AnsiString __fastcall GetStructureFileName(int nPartitionNumber);
int __fastcall SymbolCount(AnsiString String, char cSymbol);
bool __fastcall IsBlockAutoExecuted();
bool __fastcall FindDir(AnsiString DirName, AnsiString& FullDirName);
AnsiString __fastcall StringRusToLat(AnsiString OriginalString);
void __fastcall ReplaceMultiSpaceToOneSpace(AnsiString& String);
void __fastcall AttentionSoundSignal();
bool __fastcall IsGoodSymbolForPartitionOperationParameters(UCHAR cSym);
bool __fastcall IsComment(AnsiString TestString);
bool IsPartitionInPartListOrInSpecialFacilitiesPartList(AnsiString PartitionName);
void __fastcall SnapFoto(AnsiString Message);
void KillApplication(HWND hWnd);
#endif

