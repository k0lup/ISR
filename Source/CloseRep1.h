//---------------------------------------------------------------------------

#ifndef CloseRep1H
#define CloseRep1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "SelectionRep1.h"
//---------------------------------------------------------------------------
class TCloseRep : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox1;
        TButton *Button1;
        TButton *Button2;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TCloseRep(TComponent* Owner);
        void CloseWrd(char n);
};
//---------------------------------------------------------------------------
extern PACKAGE TCloseRep *CloseRep;
//---------------------------------------------------------------------------
#endif
