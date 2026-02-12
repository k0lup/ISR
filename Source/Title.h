//---------------------------------------------------------------------------

#ifndef TitleH
#define TitleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TTitleBox : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TEdit *ProtokolNamePart1Edit;
    TEdit *OperatorPUAISEdit;
    TEdit *RAPEdit;
    TEdit *OperatorAISEdit;
    TEdit *OperatorTKCKEdit;
    TButton *OKBtn;
    TButton *CancelBtn;
    TEdit *ProtokolNamePart2Edit;
    TLabel *Label7;
    TLabel *Label9;
    TLabel *Label8;
    TEdit *OperatorNTKSIEdit;
    TEdit *OperatorBSEdit;
    TEdit *TitleEdit;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall CancelBtnClick(TObject *Sender);
    void __fastcall TitleEditChange(TObject *Sender);
    void __fastcall ProtokolNamePart1EditChange(TObject *Sender);
private:	// User declarations
    bool bIsOk;
    AnsiString ProtokolDir;
private:	// User declarations
    void __fastcall SetOKBtnEnabled();
public:		// User declarations
    __fastcall TTitleBox(TComponent* Owner);
    bool __fastcall IsOK() {return (bIsOk);}
    void __fastcall SetProtokolDir(AnsiString Dir) {ProtokolDir = Dir;};
};
//---------------------------------------------------------------------------
extern PACKAGE TTitleBox *TitleBox;
//---------------------------------------------------------------------------
#endif
