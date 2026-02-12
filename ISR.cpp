//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("Source\PartisionRep1.cpp", PartisionRep);
USEFORM("Source\Request.cpp", RequestBox);
USEFORM("Source\Main.cpp", ISRMainForm);
USEFORM("Source\Title.cpp", TitleBox);
USEFORM("Source\Variant.cpp", VariantBox);
USEFORM("Source\Setting.cpp", SettingBox);
USEFORM("Source\SP.cpp", SPBox);
USEFORM("Source\Stencil.cpp", StencilBox);
USEFORM("Source\CheckSPO.cpp", CheckSPOBox);
USEFORM("Source\ChildWin.cpp", MDIChild);
USEFORM("Source\about.cpp", AboutBox);
USEFORM("Source\End.cpp", EndBox);
USEFORM("Source\Find.cpp", FindBox);
USEFORM("Source\List.cpp", PartitionListBox);
USEFORM("Source\CloseRep1.cpp", CloseRep);
USEFORM("Source\Completing.cpp", CompletingBox);
//---------------------------------------------------------------------------
extern HWND hwndPrevInstance;
extern HWND apDialogBox[];
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "";
		hwndPrevInstance = FindWindow( L"TISRMainForm", NULL);
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TISRMainForm), &ISRMainForm);
		Application->CreateForm(__classid(TVariantBox), &VariantBox);
		Application->CreateForm(__classid(TCheckSPOBox), &CheckSPOBox);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TCloseRep), &CloseRep);
		Application->CreateForm(__classid(TCompletingBox), &CompletingBox);
		Application->CreateForm(__classid(TEndBox), &EndBox);
		Application->CreateForm(__classid(TSPBox), &SPBox);
		Application->CreateForm(__classid(TFindBox), &FindBox);
		Application->CreateForm(__classid(TPartitionListBox), &PartitionListBox);
		Application->CreateForm(__classid(TRequestBox), &RequestBox);
		Application->CreateForm(__classid(TSettingBox), &SettingBox);
		Application->CreateForm(__classid(TStencilBox), &StencilBox);
		Application->CreateForm(__classid(TTitleBox), &TitleBox);
		Application->CreateForm(__classid(TPartisionRep), &PartisionRep);
		apDialogBox[0] = FindWindow( L"TAboutBox", NULL);
	apDialogBox[1] = FindWindow( L"TCheckSPOBox", NULL);
	apDialogBox[2] = FindWindow( L"TEndBox", NULL);
	apDialogBox[3] = FindWindow( L"TPartitionListBox", NULL);
	apDialogBox[4] = FindWindow( L"TFindBox", NULL);
	apDialogBox[5] = FindWindow( L"TRequestBox", NULL);
	apDialogBox[6] = FindWindow( L"TSPBox", NULL);
	apDialogBox[7] = FindWindow( L"TStencilBox", NULL);
	apDialogBox[8] = FindWindow( L"TTitleBox", NULL);
	apDialogBox[9] = FindWindow( L"TVariantBox", NULL);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
