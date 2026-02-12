//---------------------------------------------------------------------------

#ifndef SettingH
#define SettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSettingBox : public TForm
{
__published:	// IDE-managed Components
    TButton *CancelBtn;
    TButton *OKBtn;
    TPanel *Panel1;
    TCheckBox *AllowBackCrossingCheckBox;
        TButton *Button1;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TSettingBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingBox *SettingBox;
//---------------------------------------------------------------------------
#endif
