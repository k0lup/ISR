//---------------------------------------------------------------------------

#ifndef PartisionRep1H
#define PartisionRep1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Common.h"
//---------------------------------------------------------------------------
class TPartisionRep : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox1;
        TButton *LoadBtn;
        TButton *Button2;
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall LoadBtnClick(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPartisionRep(TComponent* Owner);
        AnsiString rzd;  // имя загружаемого раздела, выбранного из формы PartisionPep
        void  LoadPart(EDType com);
};
//---------------------------------------------------------------------------
extern PACKAGE TPartisionRep *PartisionRep;
//---------------------------------------------------------------------------
#endif
