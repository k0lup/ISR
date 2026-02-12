//---------------------------------------------------------------------------

#ifndef CheckSPOH
#define CheckSPOH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdjNames.h"

//---------------------------------------------------------------------------
class TCheckSPOBox : public TForm
{
__published:	// IDE-managed Components
    TButton *CloseBtn;
    TButton *CheckBtn;
    TGroupBox *GroupBox1;
    TCheckBox *AdjustmentCheckBox;
    TCheckBox *StructureCheckBox;
    TCheckBox *CPKUProgramsCheckBox;
    TCheckBox *SSBVSProgramsCheckBox;
    void __fastcall CloseBtnClick(TObject *Sender);
    void __fastcall AnyCheckBoxClick(TObject *Sender);
    void __fastcall AnyCheckBoxKeyPress(TObject *Sender, char &Key);
    void __fastcall CheckBtnClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
    AnsiString AdjustmentFileName, StructureFileName;
    AnsiString PartitionAbbr;
    // פכאד: דמכמגםמי פאיכ נאחהוכא
    bool bHEADAdjustment;
    StrList CPKURequiredProgramList, SSBVSRequiredProgramList;
    StrList CPKURealProgramList;
    int nErrorCount;
    int nReportFileHandler;
    void __fastcall SetCheckBtnEnabled();
public:		// User declarations
    void __fastcall Prepend();
    __fastcall TCheckSPOBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCheckSPOBox *CheckSPOBox;
//---------------------------------------------------------------------------
#endif
