//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Stencil.h"
#include "Common.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TStencilBox *StencilBox;
extern int size;
//---------------------------------------------------------------------------
char* my_copy( wchar_t* ws )
{
  char* s;
  int l =  wcslen(ws);
  s = new char[l + 1];
  WideCharToMultiByte( 1251, 0, ws, -1, s, l, 0, 0 );
  s[l] = '\0';
  return s;
}
//---------------------------------------------------------------------------
wchar_t* my_copy_w( char* s )
{
  wchar_t* ws;
  int l =  strlen(s);
  ws = new wchar_t[l + 1];
  MultiByteToWideChar( 1251, 0, s, -1, ws, strlen(s) );
  ws[l] = '\0';
  return ws;
}
//---------------------------------------------------------------------------
__fastcall TStencilBox::TStencilBox(TComponent* Owner)
    : TForm(Owner)
{
  /*if(StencilBox)
		  StencilBox->*/StencilEdit->EditMask = mask;
}
//---------------------------------------------------------------------------
void __fastcall TStencilBox::FormActivate(TObject *Sender)
{
  if( size == 3 || size == 4 )
	 StencilEdit->CharCase = ecUpperCase;
  // если трафарет существует

	if(bStencilExists)
	{
	   StencilEdit->Text = Stencil;
	}
	else
	{
        StencilEdit->Text = "";
    }
	StencilEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TStencilBox::CancelBtnClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
extern int size;
void __fastcall TStencilBox::OKBtnClick(TObject *Sender)
{
  String sss, sss1;
  char* s;
	// дескриптор файла
	int nFileHandler;
	// удалим файл
	DeleteFile(FullStencilFileName);
	// создадим файл
	nFileHandler = FileCreate(FullStencilFileName);
	if( nFileHandler == -1 )
	 {
		AnsiString sMessage = "Ошибка при попытке создать файл\"" + FullStencilFileName + "\". Трафарет не сохранен";
		ErrorMessageBox(sMessage);
		return;
	 }
	else
	 {
		bool bWriteError = false;
		// сохраним трафарет в файле
		sss	= StencilEdit->Text;
		int j = sss.LastDelimiter(L"-");
		sss1 = sss.SubString( j, j + 5 );
		sss.SetLength( sss.Length() - 5 );
		sss = sss.TrimRight();
		sss = sss.TrimLeft();
		if( sss.IsEmpty() || sss.Pos(' ') || sss.Pos("-!<>#$*?/|\:") || sss.Pos('"') )
		 {
			WarningMessageBox( "Введенное значение некорректно. За информацией обращайтесь к эксплуатационной документации", "Ошибка" );
			FileClose(nFileHandler);
			return;
		 }
		sss = sss + sss1;
		s = my_copy( sss.c_str() );
		if( FileWrite( nFileHandler, s, strlen(s) ) != strlen(s) )
		 {
			AnsiString sMessage = "Ошибка при записи в файл \"" + FullStencilFileName + "\".";
            ErrorMessageBox(sMessage);
			bWriteError = true;
			delete s;  // удаляем, т.к. в ф-и my_copy выделяется динамич.память
		 }
        FileClose(nFileHandler);
        // еслим ошибки записи не было
		if( !bWriteError )
		{
            // занесем трафарет в переменную Stencil
			Stencil = sss; //StencilEdit->Text;
            // трафарет существует
			bStencilExists = true;
            Close();
        }
	 }
}
//---------------------------------------------------------------------------
void __fastcall TStencilBox::FormClose( TObject *Sender, TCloseAction &Action )
{
  //ShowMessage("Close trafaret");      
}
//---------------------------------------------------------------------------

