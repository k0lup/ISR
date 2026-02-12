//---------------------------------------------------------------------------

#ifndef StencilH
#define StencilH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TStencilBox : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TButton *CancelBtn;
    TButton *OKBtn;
    TMaskEdit *StencilEdit;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
		void __fastcall FormClose( TObject *Sender, TCloseAction &Action );
private:	// User declarations
public:		// User declarations
    __fastcall TStencilBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TStencilBox *StencilBox;
//---------------------------------------------------------------------------
#endif
