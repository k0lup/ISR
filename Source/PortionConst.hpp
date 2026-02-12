// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'PortionConst.pas' rev: 5.00

#ifndef PortionConstHPP
#define PortionConstHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <SysUtils.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Portionconst
{
//-- type declarations -------------------------------------------------------
typedef int TDeviceAddress;

typedef Byte TByteArray[1000000001];

class DELPHICLASS EPortionError;
class PASCALIMPLEMENTATION EPortionError : public Sysutils::Exception 
{
	typedef Sysutils::Exception inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EPortionError(const AnsiString Msg) : Sysutils::Exception(
		Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EPortionError(const AnsiString Msg, const System::TVarRec 
		* Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EPortionError(int Ident)/* overload */ : Sysutils::Exception(
		Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EPortionError(int Ident, const System::TVarRec * Args
		, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EPortionError(const AnsiString Msg, int AHelpContext) : 
		Sysutils::Exception(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EPortionError(const AnsiString Msg, const System::TVarRec 
		* Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext
		) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EPortionError(int Ident, int AHelpContext)/* overload */
		 : Sysutils::Exception(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EPortionError(System::PResStringRec ResStringRec
		, const System::TVarRec * Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(
		ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EPortionError(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
static const Shortint OldStileAtomHeadLength = 0x4;
static const Shortint OldStileCortHeadLength = 0x4;
static const Shortint OldStilePortionHeadLength = 0x8;
#define SIntError "¬нутренн€€ ошибка. "
extern PACKAGE System::ResourceString _SInvalidArg;
#define Portionconst_SInvalidArg System::LoadResourceString(&Portionconst::_SInvalidArg)
extern PACKAGE System::ResourceString _SInvalidVariant;
#define Portionconst_SInvalidVariant System::LoadResourceString(&Portionconst::_SInvalidVariant)
extern PACKAGE System::ResourceString _SInvalidIndex;
#define Portionconst_SInvalidIndex System::LoadResourceString(&Portionconst::_SInvalidIndex)
extern PACKAGE System::ResourceString _SInvalidPortion;
#define Portionconst_SInvalidPortion System::LoadResourceString(&Portionconst::_SInvalidPortion)
extern PACKAGE System::ResourceString _SIdNotAssigned;
#define Portionconst_SIdNotAssigned System::LoadResourceString(&Portionconst::_SIdNotAssigned)
extern PACKAGE System::ResourceString _SCntExecuteWhileWait;
#define Portionconst_SCntExecuteWhileWait System::LoadResourceString(&Portionconst::_SCntExecuteWhileWait)
	
extern PACKAGE System::ResourceString _SArgMastByPortion;
#define Portionconst_SArgMastByPortion System::LoadResourceString(&Portionconst::_SArgMastByPortion)
	
extern PACKAGE int CBintID;
static const Shortint BINT_ID = 0x65;
extern PACKAGE int CShowerID;
static const Shortint CSlegID = 0x6f;
extern PACKAGE int CAutoAnswer1ID;
extern PACKAGE int CCfm1ID;
extern PACKAGE int CCfm2ID;
static const Shortint CCfmZZ_ID = 0x0;
static const Shortint CCfmOI_ID = 0x1;
static const Shortint CCfmUI_ID = 0x2;
static const Shortint CCfmCK_ID = 0x3;
static const Shortint CCfmTS_ID = 0x15;
static const Shortint CCfmCI_ID = 0x4;
static const Shortint CCfmACI_ID = 0x18;
static const Shortint CCfmACK_ID = 0x17;
static const Shortint CCfmRECS_ID = 0x5;
static const Shortint CCfmARECS_ID = 0x19;
static const Shortint CFmRG_ZZ_ID = 0x6;
static const Shortint CVpkuOI_ID = 0x1f;
static const Shortint CVpkuUI_ID = 0x20;
static const Shortint CVpkuTC_ID = 0x21;
static const Shortint CVpkuCI_ID = 0x22;
static const Shortint CVpkuCK_ID = 0x23;
static const Shortint CVpkuRG_ID = 0x24;
static const Shortint CFMZO_ID = 0x2;
static const Shortint CFMTC_ID = 0x3;
static const Shortint CFMOI_ID = 0x1;
static const Shortint CFMCI_ID = 0x4;
static const Shortint CFMCK_ID = 0x5;
static const Shortint CFMACK_ID = 0x19;
static const Shortint CFMACI_ID = 0x18;

}	/* namespace Portionconst */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Portionconst;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// PortionConst
