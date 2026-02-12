//----------------------------------------------------------------------------
#ifndef VariantH
#define VariantH
//----------------------------------------------------------------------------
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <Windows.hpp>
#include <System.hpp>
//----------------------------------------------------------------------------
class TVariantBox : public TForm
{
__published:
    TButton *ChooseBtn;
    TButton *CancelBtn;
    TListBox *VariantListBox;
    TLabel *Label1;
    void __fastcall ChooseBtnClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall VariantListBoxDblClick(TObject *Sender);
private:
    int nVariantCount;
    int nChosenVariant;
public:
	virtual __fastcall TVariantBox(TComponent *Owner);
    void SetVariantCount(int nTrasVarCount) {nVariantCount = nTrasVarCount;}
    int GetChosenVariant() {return nChosenVariant;}
};
//----------------------------------------------------------------------------
extern TVariantBox *VariantBox;
//----------------------------------------------------------------------------
#endif	
