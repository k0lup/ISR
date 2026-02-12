//---------------------------------------------------------------------------

#ifndef EndH
#define EndH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Common.h"
//---------------------------------------------------------------------------
class TEndBox : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label2;
    TButton *EndWithSavingProtBtn;
    TButton *EndWithoutSavingProtBtn;
    TButton *CancelBtn;
    TLabel *Delimiter1Label;
    TLabel *Delimiter2Label;
    TEdit *ProtokolNamePart1Edit;
    TEdit *ProtokolNamePart2Edit;
    TEdit *ProtokolNamePart3Edit;
    void __fastcall EndWithSavingProtBtnClick(TObject *Sender);
    void __fastcall EndWithoutSavingProtBtnClick(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
    EndMessageBoxResult nResult;
    bool bEndAllPartitions;
public:		// User declarations
    __fastcall TEndBox(TComponent* Owner);
    EndMessageBoxResult __fastcall Result() {return (nResult);}
    AnsiString __fastcall GetProtokolName();
    void __fastcall SetMode(bool bMode) {bEndAllPartitions = bMode;}
};
//---------------------------------------------------------------------------
extern PACKAGE TEndBox *EndBox;
//---------------------------------------------------------------------------
#endif
