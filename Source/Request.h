//---------------------------------------------------------------------------

#ifndef RequestH
#define RequestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "ChildWin.h"
//---------------------------------------------------------------------------
class TRequestBox : public TForm
{
__published:	// IDE-managed Components
    TButton *MakeButton;
    TButton *CancelBtn;
    TMemo *OperationTextMemo;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall MakeButtonClick(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
private:	// User declarations
    OperationType CurrentType;
    AnsiString OperationText;
    bool bMakeOperation;
public:		// User declarations
    __fastcall TRequestBox(TComponent* Owner);
    void __fastcall SetCurrentType(OperationType Type) {CurrentType = Type;};
    bool __fastcall GetMakeOperation() {return (bMakeOperation);};
    void __fastcall SetText(TMDIChild* ch, int nTextStart, int nTextEnd, int nFieldNumber);
};
//---------------------------------------------------------------------------
extern PACKAGE TRequestBox *RequestBox;
//---------------------------------------------------------------------------
#endif
