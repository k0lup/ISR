//----------------------------------------------------------------------------
#ifndef MainH
#define MainH
//----------------------------------------------------------------------------
#include "ChildWin.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Messages.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <StdCtrls.hpp>
#include <Menus.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <ToolWin.hpp>
#include <OleServer.hpp>
//#include "Word_2K_SRVR.h"
#include "SelectionRep1.h"
#include <Vcl.OleCtnrs.hpp>
#include "Word_XP_srvr.h"
//----------------------------------------------------------------------------
typedef  struct
   {
     unsigned  short   LenPor;
     unsigned  char    AdrTo;
     unsigned  char    AdrFrom;
               short   NumPor;
     unsigned  char    DOP1;
     unsigned  char    DOP2;

     unsigned  short   LenArt;
     unsigned  char    KindofWork;
	 unsigned  char    SortofWork;

     unsigned  short   LenCort;
     unsigned  char    AtomType;
     unsigned  char    AtomQuant;
   }POR;
#include "net_PRIS.hpp"
//----------------------------------------------------------
class TISRMainForm : public TForm
{
__published:
    TMainMenu *MainMenu;
    TMenuItem *TestMenu;
    TMenuItem *TestTest1;
    TToolBar *ToolBar;
    TToolButton *ToolButton2;
    TMenuItem *TestTest2;
    TMenuItem *TestTest3;
    TMenuItem *PrepareMenu;
    TMenuItem *PrepareHeadline;
    TMenuItem *PrepareCheckSPO;
    TMenuItem *PreparePFKS;
    TMenuItem *PrepareLoadStructure;
    TMenuItem *ArticleNShS;
    TMenuItem *ArticleGoInitial;
    TMenuItem *ArticleDamage;
    TToolButton *TestBtn;
    TMenuItem *ArticleMenu;
    TToolButton *ToolButton4;
    TMenuItem *SectionsMenu;
    TMenuItem *SectionsNormal;
    TMenuItem *SectionsNShS;
    TMenuItem *SectionsGoInitial;
    TMenuItem *WorkMenu;
    TMenuItem *WorkSP;
    TMenuItem *PreparePartitionsList;
    TToolButton *PrepareBtn;
    TToolButton *ToolButton1;
    TToolButton *ToolButton5;
    TMenuItem *PrepareAddPartitions;
    TMenuItem *N3;
    TMenuItem *WorkUndoChosenVariant;
    TMenuItem *PartitionsMenu;
    TMenuItem *PartitionsFirst;
    TMenuItem *PartitionsSecond;
    TMenuItem *PartitionsThird;
    TToolButton *ToolButton6;
    TMenuItem *PrepareStencil;
    TMenuItem *PrepareRebuildList;
    TMenuItem *WorkEndPartition;
    TMenuItem *WorkEndAllPartitions;
    TMenuItem *N15;
    TMenuItem *N8;
    TMenuItem *WorkOptions;
    TMenuItem *ModeMenu;
    TMenuItem *AutoMakeAuto;
    TMenuItem *AutoStopAutoBlock;
    TMenuItem *WorkPartitionLength;
    TMenuItem *WorkGoToActiveSection;
    TMenuItem *TestTest5;
    TMenuItem *WorkReturnWindow;
    TMenuItem *TestTest4;
    TMenuItem *TestTest6;
    TMenuItem *AutoBeginFinishBlock;
    TMenuItem *N2;
    TMenuItem *AutoClearSelection;
    TMenuItem *TestTest7;
    TMenuItem *TestTest8;
    TMenuItem *TestTest10;
    TOpenDialog *OpenDialog;
    TMenuItem *WorkMakeOperation;
    TMenuItem *N1;
    TMenuItem *PrepareDebugPartition;
    TMenuItem *WorkImitAnswer;
	TWordApplication *WordApplication;
    TMenuItem *N4;
    TMenuItem *PrepareSupernumerary;
    TMenuItem *PrepareSupernumeraryExit;
        TMenuItem *ArticleNShS_REP;
        TMenuItem *ArticleDamage_REP;
        TMenuItem *ArticleGoInitial_REP;
		TToolButton *ToolButton3;
        TToolButton *ToolButton7;
        TMenuItem *N5;
		TMenuItem *N6;
		TMenuItem *N7;
        TMenuItem *N9;
        TMenuItem *N10;
        TMenuItem *N11;
        TMenuItem *N12;
        TMenuItem *N13;
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ProcessInPortion( char *Portion, int l);
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall PrepareLoadStructureClick(TObject *Sender);
    void __fastcall SectionsAnyMenuItemClick(TObject *Sender);
    void __fastcall OperationsEndPartitionClick(TObject *Sender);
    void __fastcall WorkUndoChosenVariantClick(TObject *Sender);
    void __fastcall ArticleAnyItemClick(TObject *Sender);
    void __fastcall WordApplicationQuit(TObject *Sender);
    void __fastcall PartitionsAnyItemClick(TObject *Sender);
    void __fastcall PrepareHeadlineClick(TObject *Sender);
    void __fastcall PreparePartitionsListClick(TObject *Sender);
    void __fastcall TestTest1Click(TObject *Sender);
    void __fastcall WorkEndPartitionClick(TObject *Sender);
    void __fastcall TestTest2Click(TObject *Sender);
    void __fastcall TestTest3Click(TObject *Sender);
    void __fastcall PrepareStencilClick(TObject *Sender);
        void __fastcall TestTest5Click(TObject *Sender);
    void __fastcall WorkGoToActiveSectionClick(TObject *Sender);
    void __fastcall WorkReturnWindowClick(TObject *Sender);
    void __fastcall TestTest4Click(TObject *Sender);
    void __fastcall TestTest6Click(TObject *Sender);
    void __fastcall WorkSPClick(TObject *Sender);
    void __fastcall AutoBeginFinishBlockClick(TObject *Sender);
    void __fastcall AutoClearSelectionClick(TObject *Sender);
    void __fastcall AutoStopAutoBlockClick(TObject *Sender);
    void __fastcall AutoMakeAutoClick(TObject *Sender);
    void __fastcall TestTest7Click(TObject *Sender);
    void __fastcall TestTest8Click(TObject *Sender);
    void __fastcall TestTest10Click(TObject *Sender);
    void __fastcall PrepareRebuildListClick(TObject *Sender);
    void __fastcall PrepareAddPartitionsClick(TObject *Sender);
    void __fastcall WorkPartitionLengthClick(TObject *Sender);
    void __fastcall WorkMakeOperationClick(TObject *Sender);
    void __fastcall PreparePFKSClick(TObject *Sender);
    void __fastcall PrepareDebugPartitionClick(TObject *Sender);
    void __fastcall PrepareCheckSPOClick(TObject *Sender);
    void __fastcall WorkOptionsClick(TObject *Sender);
    void __fastcall WorkImitAnswerClick(TObject *Sender);
    void __fastcall PrepareSupernumeraryExitClick(TObject *Sender);
    void __fastcall PrintAdjstmentKSToProt();
        void __fastcall N7Click(TObject *Sender);
        void __fastcall N9Click(TObject *Sender);
        void __fastcall N10Click(TObject *Sender);
        void __fastcall N11Click(TObject *Sender);
        void __fastcall N12Click(TObject *Sender);
        void __fastcall N13Click(TObject *Sender);
	//void __fastcall WordApplication1Quit(TObject *Sender);
        //void __fastcall TestBtnClick(TObject *Sender);
private:
	void __fastcall CreateMDIChild(const String Name);
private:
//---------------------------------------------------------------------------
// члены-данные
//---------------------------------------------------------------------------
    // указатели на пункты подменю меню "Раздел"
    TMenuItem* PartitionMenuItems[MaxPartitionCount];

//---------------------------------------------------------------------------
// закрытые функции
//---------------------------------------------------------------------------
    void __fastcall Prepare();
    void __fastcall PRISLoaded();
    void __fastcall OnException(TObject *Sender, Exception *E);
    void __fastcall RebuildPartitionsList();
    void __fastcall EndCurrentPartition(bool);
    void __fastcall RefusalEndPartition();
    void __fastcall Title(AnsiString ProtokolDir);

    // обработчики сообщений Windows
    void __fastcall OnSysCommand(TMessage& Message);
    void __fastcall OnWindowPosChanging(TMessage& Message);
    void __fastcall OnEndPartition(TMessage& Message);

public:
//---------------------------------------------------------------------------
// методы
//---------------------------------------------------------------------------
	virtual __fastcall TISRMainForm(TComponent *Owner);
    bool LoadStructure(bool bFromPartitionOperation, AnsiString& StrToProtokol, AnsiString& ErrorMessage, int nTopSection = 0);
    void __fastcall ConfigureMenu();
    void __fastcall SetTopSection(int nSectionNumber, int PartitionNumber = -1);
    bool __fastcall PartitionOperation(AnsiString PartitionAbbr, AnsiString Label, bool bAuto, bool bFromPris, bool bISRStructureOperation, AnsiString OperationText = "");
    void __fastcall AnswerForKO(int AtomType);
    void __fastcall GoToActiveSection();
    void __fastcall KeyPressedInChildWindow(KeysPressed Key);
    bool __fastcall RecievePartitionOperation(AnsiString Parameters, bool bFromPris, AnsiString OperationText);
    void __fastcall PreProcessPartitionOperation(AnsiString Parameters);
    void __fastcall ProcessEDDir(AnsiString DirText);
    int __fastcall ShowED(EDType CurrentType,  AnsiString Label, bool bFromPris);
    void __fastcall ProcessNASRDir(AnsiString DirText);
    void __fastcall SetMainWindowOnTop();
	void __fastcall On_TM(TObject *Sender);      // ф-я обраб.таймера ТМ
	NET_CL_PRIS* net_PRIS;
	NET_CL_PRIS* net_PROT;
	//--------------------------------------------------------------------------
	// работа с КТ
	//--------------------------------------------------------------------------
	short __fastcall vosst_KT();                 // ф-я восстановления с КТ
	short __fastcall save_KT(AnsiString);        // ф-я сохранения КТ
	bool pr_save;                                // признак работы с КТ ( сохр-е )
	bool pr_vosst;                               // -*- (восст-е)
	bool r_vrem;
	bool prer;
	AnsiString vosst;                            // имя файла с полным путем для восстановления
	AnsiString save_loc;                         // имя файла с полным путем для лок.сохр
	AnsiString save_rem;                         // имя файла с полным путем для удаленн.сохр
	//--------------------------------------------------------------------------
	//      Работа с разделами РЭП
	//--------------------------------------------------------------------------
	AnsiString Partision_Rep;                    // Имя 1-го выбранного раздела РЭП
	AnsiString Second_Rep;                       // Имя 2-го выбранного раздела РЭП
	AnsiString Third_Rep;                        // Имя 3-го выбранного раздела РЭП
	void  FormPartition_REP(); 					 // формирование списка разделов РЭП
	TSectionRep* SecondRep;   					 // окно второго раздела РЭП
	TSectionRep* ThirdRep;    					 // окно третьего раздела РЭП
	//--------------------------------------------------------------------------
	short nR;                                    // тип последней операции, посланной в ПРИС
	short BR;                                    // признак загрузки раздела ПРИСом ( 1-норм.;(-1)-ош. )
    bool KS;                                     // признак конца работы
	TTimer* TM;                                  // таймер ожидания ответа от БИНТа
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER (MESSAGE_END_PARTITION, TMessage, OnEndPartition);
    END_MESSAGE_MAP(TForm)

};
//----------------------------------------------------------------------------
extern TISRMainForm *ISRMainForm;
extern TMDIChild *__fastcall MDIChildCreate(void);
#endif
