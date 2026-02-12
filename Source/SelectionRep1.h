//---------------------------------------------------------------------------

#ifndef SelectionRep1H
#define SelectionRep1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TSectionRep : public TForm
{
__published:	// IDE-managed Components
        TStringGrid *StringGrid1;
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall StringGrid1DrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall StringGrid1DblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
         TComponent* owner;
        __fastcall TSectionRep(TComponent* Owner);
        void AnswerForRep(int Answer);
        AnsiString KO;  // наименование операции в ПРИС
        TList* OpCount; //количество операций в разделе РЭП
        int CurrOp; // номер текущей операции в разделе РЭП
        int Op;     // номер текущей строки в разделе РЭП
        bool p ; // признак поторного выполнения операции
        int k1;     // для сохранения № тек. оп-и при нарушении порядка вып-я операций
};
//
typedef  struct {
                  int  kol_op;  // количество строк в операции
                  bool v;       // признак выполнения операции
                } K_OP;
//---------------------------------------------------------------------------
extern PACKAGE TSectionRep *SectionRep;
//---------------------------------------------------------------------------
#endif
