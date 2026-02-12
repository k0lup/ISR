//----------------------------------------------------------------------------
#ifndef ChildWinH
#define ChildWinH
//----------------------------------------------------------------------------
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <Windows.hpp>
#include <System.hpp>
#include <StdCtrls.hpp>
#include <Grids.hpp>
#include "Common.h"
// признак СП из операции "И" СП в файле dii
extern short ISR_prot;
//----------------------------------------------------------------------------
class TMDIChild : public TForm
{
__published:
    TStringGrid *Grid;
    TLabel *SectionCaptionLabel;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall GridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall GridDblClick(TObject *Sender);
    void __fastcall GridKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormDestroy(TObject *Sender);
//---------------------------------------------------------------------------
// члены-данные
//---------------------------------------------------------------------------
public:
//private:
	// номер секции
	int nSectionNumber;
	// массив строк секции
	StructureString* pSectionStrings;
	// количество строк
	int nSectionStringCount;
	// заголовок секции
	AnsiString SectionCaption;
	// имя файла структуры
	AnsiString StructureFileName;
        // номер раздела (0-2)
        int nPartitionNumber;
	// заголовок окна
	AnsiString WindowCaption;
	// первые и последние стоки операций
	int* pnOperationStart;
	int* pnOperationEnd;
	// массив типов операций
	OperationType* pOperationType;
	// массив флагов, указывающих, что операция выполнена
	bool* pbMadeOperation;
	// массив количеств вариантов в директивах "Вариант"
	// если директива не явл. "Вариантом", то значение элемента массива = 0;
	// номер текущей операции
	int* pnVariantCount;
    // первая и последняя строка для каждого варианта
    int** ppnVariantStart;
    int** ppnVariantEnd;

    // текущая операция (если операций нет, nCurrentOperation == -1)
     //	int nCurrentOperation;
    // количество операций
	int nOperationCount;
    // флаг: в данный момент выбран вариант
    bool bSelectedVariant;
    // номер операции "Вариант", в которой сейчас выбран один из вариантов
    int nVariantOperationNumber;
    // номер выбранного варианта
    int nChosenVariantNumber;
    // объект на этапе конструирования (выполняется конструктор)
    bool bIsConstructed;
    // флаг: ПРИС выполняет операцию
    bool bPrisExecutesOperation;
    // коментарий к текущей операции, выполняющейся ПРИСом или самим ИСР
    AnsiString CommentToCurrentOperation;
    // номер операции, которая выполняется (ПРИСом или самим ИСР)
    int nExecutesOperationNumber;
    // начало и конец блока, выполняющегося автоматически
    // (номера операций)
    // если nAutoStart ==-1, то блока нет
    int nAutoFirst, nAutoLast;
    // флаг: начали выделять блок
    bool bBlockSelectionStart;
    // выполняется блок "Авто"
    bool bExecuteAutoBlock;
    // поступила заявка на остановку блока "Авто"
    bool bStopAutoRequest;
    // блок "Авто" выполняется из по команде пуска раздела из ПРИС в автоматическом режиме
    bool bAutoBlockFromPris;
    // после блока "Авто", который выполняеся по вызову из ПРИС, есть не-диполь операции
    bool bExistsNonDipolOperation;
    // в данный момент обрабатывается операция ИСР "Загрузка раздела"
    bool bISRPartitionOperationProcess;
    // кол-во блоков НШС
    int nNShSBlockCount;
    // блоки НШС (располагаются последовательно с верхнего к нижнему,
    // по номерам блоки не упорядочены)
    NShSBlockInfo aNShSBlock[MaxNShSBlockCount];
//---------------------------------------------------------------------------
// закрытые функции
//---------------------------------------------------------------------------
void __fastcall MakeCurrentOperation();
void __fastcall StartOperation();
void __fastcall EndOperation();
void __fastcall ExecuteAutoBlockOperation();
void __fastcall StopExecuteAutoBlock();
bool __fastcall TestCorrectAutoBlock(int nAutoFirst, int nAutoLast);
void __fastcall OnEndCalledPartition(TMessage& Message);
void __fastcall ProcessAuto();
AnsiString __fastcall GetCurrentPosition(int nFirstString, int nLastString);
int NShSBlockNumber(int nOperationNumber);
AnsiString NShSBlockDescription(int nOperationNumber);

//---------------------------------------------------------------------------
// методы
//---------------------------------------------------------------------------
public:
	virtual __fastcall TMDIChild(TComponent *Owner, int nTransSectionNumber,
	StructureString* pTransSectionStrings, int nTransSectionStringCount,
    AnsiString TransSectionCaption,AnsiString TransStructureFileName,
	int nTransPartitionNumber);
    int __fastcall GetSectionNumber();
    void __fastcall DrawGrid();
    void __fastcall UndoChosenVariant();
    void __fastcall AnswerForKO(int nAnswer);
    void __fastcall MakeDipolOperation(int nFirstString, int nLastString);
    bool __fastcall GetExistsSelectedVariant() {return (bSelectedVariant);}
    int __fastcall GetVariantOperationNumber() {return (nVariantOperationNumber);}
    bool __fastcall GetSelectedVariant() {return (bSelectedVariant);}
    void __fastcall SetSelectedVariant(bool bVal) {bSelectedVariant = bVal;}
    int __fastcall GetOperationCount() {return (nOperationCount);}
    bool __fastcall GetPrisExecutesOperation() {return (bPrisExecutesOperation);}
    AnsiString __fastcall GetCommentToCurrentOperation() {return (CommentToCurrentOperation);}
    int __fastcall GetAutoFirst() {return (nAutoFirst);}
    StructureString* __fastcall GetSectionStrings() {return (pSectionStrings);}
    bool __fastcall GetExecuteAutoBlock() {return (bExecuteAutoBlock);}
    bool __fastcall GetStopAutoRequest() {return (bStopAutoRequest);}
    void __fastcall BeginFinishBlock(void);
    void __fastcall ClearSelection(void);
    void __fastcall ExecuteAutoBlock(void);
    void __fastcall StopAutoBlock(void);
    void __fastcall MakeOperation();
    void __fastcall WorkInAutoMode();
    int __fastcall SetNshSBlock(int nBlockNumber);
    bool __fastcall MakeISROperation(int nFirstString, int nLastString);
    void __fastcall SetISRPartitionOperationProcess(bool bValue) {bISRPartitionOperationProcess = bValue;}
    void __fastcall GoNextOperation(bool bRedrawGrid);
    void __fastcall SetMadeOperation() {pbMadeOperation[nCurrentOperation] = true;}
    void __fastcall EndCalledPartition();
    void __fastcall SetLastMadeOperationOnCurrentOperation() {nLastMadeOperation = nCurrentOperation;}
    // текущая операция (если операций нет, nCurrentOperation == -1)
    int nCurrentOperation;
    // номер последней выполненной операции
    // вначале имеет значение -1
    int nLastMadeOperation;
    
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER (MESSAGE_END_CALLED_PARTITION, TMessage, OnEndCalledPartition);
    END_MESSAGE_MAP(TForm)
};
//----------------------------------------------------------------------------
#endif
