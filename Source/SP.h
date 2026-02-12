//---------------------------------------------------------------------------

#ifndef SPH
#define SPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TSPBox : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label2;
    TEdit *ProtokolNamePart1Edit;
    TLabel *Label7;
    TEdit *ProtokolNamePart2Edit;
    TLabel *Label1;
    TEdit *ProtokolNamePart3Edit;
    TButton *CancelBtn;
    TButton *OKBtn;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall ProtokolNamePart1EditChange(TObject *Sender);
private:	// User declarations
    void __fastcall SetOKBtnEnabled();
public:		// User declarations
    __fastcall TSPBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSPBox *SPBox;
//---------------------------------------------------------------------------
#endif
