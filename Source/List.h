//---------------------------------------------------------------------------

#ifndef ListH
#define ListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Common.h"
//---------------------------------------------------------------------------
class TPartitionListBox : public TForm
{
__published:	// IDE-managed Components
    TListBox *PartitionListBox;
    TButton *ChooseBtn;
    TButton *CancelBtn;
    TButton *FindBtn;
    TButton *LoadBtn;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall ChooseBtnClick(TObject *Sender);
    void __fastcall PartitionListBoxDblClick(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall FindBtnClick(TObject *Sender);
private:	// User declarations
    int nSelectedPartition;
    PartitionListBoxResult Result;
public:		// User declarations
    __fastcall TPartitionListBox(TComponent* Owner);
    int __fastcall GetSelectedPartitionNumber() {return (nSelectedPartition);}
    PartitionListBoxResult __fastcall GetPartitionListBoxResult() {return (Result);}
};
//---------------------------------------------------------------------------
extern PACKAGE TPartitionListBox *PartitionListBox;
//---------------------------------------------------------------------------
#endif
