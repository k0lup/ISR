//---------------------------------------------------------------------------

#ifndef FindH
#define FindH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TFindBox : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TButton *CancelBtn;
    TButton *OKBtn;
    TEdit *AbbrEdit;
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall AbbrEditEnter(TObject *Sender);
    void __fastcall AbbrEditChange(TObject *Sender);
private:	// User declarations
    AnsiString Abbr;
    void __fastcall SetOKBtnEnabled();
public:		// User declarations
    __fastcall TFindBox(TComponent* Owner);
    AnsiString __fastcall GetAbbr(void) { return (Abbr); }
};
//---------------------------------------------------------------------------
extern PACKAGE TFindBox *FindBox;
//---------------------------------------------------------------------------
#endif
