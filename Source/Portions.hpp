// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Portions.pas' rev: 5.00

#ifndef PortionsHPP
#define PortionsHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <InOutClnt.hpp>	// Pascal unit
#include <PortionConst.hpp>	// Pascal unit
#include <Math.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Portions
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TPortionAtom;
class DELPHICLASS TPortionCort;
class DELPHICLASS TPortion;
typedef void __fastcall (__closure *TPortionEvent)(const TPortion* Portion, const int AddrFrom, const 
	int Rod, const int Sort, const int AtomType, const TPortionAtom* Atom);

class PASCALIMPLEMENTATION TPortion : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	int FAddrFrom;
	int FAddrTo;
	Classes::TList* FCortsList;
	int FPortionNum;
	int FReserved1;
	int FReserved2;
	TPortionCort* FLastCort;
	Inoutclnt::TInOutClient* FDefaultInOut;
	TPortionEvent FOnPortion;
	Inoutclnt::TInOutClient* FInOut;
	AnsiString __fastcall GetAsString();
	Variant __fastcall GetAsVarArray();
	WideString __fastcall GetAsWideString();
	TPortionCort* __fastcall GetCort(int Index);
	int __fastcall GetCount(void);
	Inoutclnt::TInOutClient* __fastcall GetInOut(void);
	bool __fastcall GetNonBlocking(void);
	int __fastcall GetPortionLength(void);
	void __fastcall SetAddrFrom(const int Value);
	void __fastcall SetAddrTo(const int Value);
	void __fastcall SetAsString(const AnsiString Value);
	void __fastcall SetAsVarArray(const Variant &Value);
	void __fastcall SetAsWideString(const WideString Value);
	void __fastcall SetInOut(const Inoutclnt::TInOutClient* Value);
	void __fastcall SetNonBlocking(const bool Value);
	void __fastcall SetPortionNum(const int Value);
	void __fastcall SetReserved1(const int Value);
	void __fastcall SetReserved2(const int Value);
	
protected:
	int FHeadLength;
	virtual void __fastcall ReadFrom(const void *Buf);
	virtual void __fastcall WriteTo(void *Buf);
	__property WideString AsWideString = {read=GetAsWideString, write=SetAsWideString, stored=false};
	
public:
	__fastcall virtual TPortion(void)/* overload */;
	__fastcall virtual TPortion(const int AddrTo, const int AddrFrom)/* overload */;
	__fastcall virtual TPortion(const Inoutclnt::TInOutClient* AInOut)/* overload */;
	__fastcall virtual TPortion(const Variant &Portion);
	__fastcall virtual ~TPortion(void);
	virtual int __fastcall AddCort(void)/* overload */;
	virtual int __fastcall AddCort(const int Rod, const int Sort)/* overload */;
	virtual TPortionAtom* __fastcall AddAtom(const int Rod, const int Sort, const int AtomType);
	void __fastcall Build(const int AddrTo, const int AddrFrom);
	virtual bool __fastcall BuildFrom(const Variant &Portion);
	bool __fastcall Check(void)/* overload */;
	bool __fastcall Check(const int ID)/* overload */;
	virtual void __fastcall Clear(void);
	void __fastcall MakeAnswer(void);
	void __fastcall ProcessPortion(void);
	void __fastcall Send(void);
	bool __fastcall Wait(void)/* overload */;
	bool __fastcall Wait(const int TimeOut)/* overload */;
	bool __fastcall Wait(const int ID, const int TimeOut)/* overload */;
	__property TPortionCort* Corts[int Index] = {read=GetCort/*, default*/};
	__property int Count = {read=GetCount, nodefault};
	__property TPortionCort* LastCort = {read=FLastCort};
	__property int PortionLength = {read=GetPortionLength, nodefault};
	
__published:
	__property int AddrFrom = {read=FAddrFrom, write=SetAddrFrom, stored=false, nodefault};
	__property int AddrTo = {read=FAddrTo, write=SetAddrTo, stored=false, nodefault};
	__property AnsiString AsString = {read=GetAsString, write=SetAsString};
	__property Variant AsVarArray = {read=GetAsVarArray, write=SetAsVarArray, stored=false};
	__property Inoutclnt::TInOutClient* InOut = {read=GetInOut, write=SetInOut};
	__property bool NonBlocking = {read=GetNonBlocking, write=SetNonBlocking, nodefault};
	__property int PortionNum = {read=FPortionNum, write=SetPortionNum, stored=false, nodefault};
	__property int Reserved1 = {read=FReserved1, write=SetReserved1, stored=false, nodefault};
	__property int Reserved2 = {read=FReserved2, write=SetReserved2, stored=false, nodefault};
	__property TPortionEvent OnPortion = {read=FOnPortion, write=FOnPortion};
};


class PASCALIMPLEMENTATION TPortionCort : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	Classes::TList* FAtomsList;
	int FRod;
	int FSort;
	TPortion* FOwner;
	TPortionAtom* FLastAtom;
	TPortionAtom* __fastcall GetAtom(int Index);
	int __fastcall GetCount(void);
	void __fastcall SetRod(const int Value);
	void __fastcall SetSort(const int Value);
	void __fastcall SetOwner(const TPortion* Value);
	
protected:
	int FHeadLength;
	virtual int __fastcall GetCortLength(void);
	void __fastcall ReadFrom(const void *Buf, int &Position);
	void __fastcall WriteTo(void *Buf, int &Position);
	
public:
	__fastcall virtual TPortionCort(const TPortion* AOwner);
	__fastcall virtual ~TPortionCort(void);
	virtual int __fastcall AddAtom(void)/* overload */;
	virtual int __fastcall AddAtom(const int AtomType)/* overload */;
	virtual void __fastcall Clear(void);
	__property TPortionAtom* Atoms[int Index] = {read=GetAtom/*, default*/};
	__property int CortLength = {read=GetCortLength, nodefault};
	__property int Count = {read=GetCount, nodefault};
	__property TPortionAtom* LastAtom = {read=FLastAtom};
	__property TPortion* Owner = {read=FOwner, write=SetOwner};
	
__published:
	__property int Rod = {read=FRod, write=SetRod, nodefault};
	__property int Sort = {read=FSort, write=SetSort, nodefault};
};


class PASCALIMPLEMENTATION TPortionAtom : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	AnsiString FBody;
	int FAtomType;
	TPortionCort* FOwner;
	int FBodiesCount;
	Byte __fastcall GetAsByte(void);
	__int64 __fastcall GetAsInt64(void);
	int __fastcall GetAsInteger(void);
	double __fastcall GetAsReal(void);
	short __fastcall GetAsSmallInt(void);
	AnsiString __fastcall GetAsString();
	int __fastcall GetBodyLength(void);
	void __fastcall SetAsByte(const Byte Value);
	void __fastcall SetAsInt64(const __int64 Value);
	void __fastcall SetAsInteger(const int Value);
	void __fastcall SetAsReal(const double Value);
	void __fastcall SetAsSmallInt(const short Value);
	void __fastcall SetAsString(const AnsiString Value);
	void __fastcall SetAtomType(const int Value);
	void __fastcall SetOwner(const TPortionCort* Value);
	
protected:
	int FHeadLength;
	virtual int __fastcall GetAtomLength(void);
	void __fastcall ReadFrom(const void *Buf, int &Position);
	void __fastcall WriteTo(void *Buf, int &Position);
	
public:
	__fastcall virtual TPortionAtom(const TPortionCort* AOwner);
	virtual void __fastcall Append(const Variant &Value)/* overload */;
	virtual void __fastcall Append(const Variant &Value, const int Length)/* overload */;
	virtual void __fastcall AppendBuf(const void *Buf, const int Length);
	virtual void __fastcall AppendZeroes(const int Count);
	virtual void __fastcall Clear(void);
	virtual int __fastcall GetBody(void *ABody);
	__property int AtomLength = {read=GetAtomLength, nodefault};
	__property AnsiString Body = {read=FBody};
	__property int BodyLength = {read=GetBodyLength, nodefault};
	__property int BodiesCount = {read=FBodiesCount, write=FBodiesCount, nodefault};
	__property TPortionCort* Owner = {read=FOwner, write=SetOwner};
	
__published:
	__property Byte AsByte = {read=GetAsByte, write=SetAsByte, nodefault};
	__property __int64 AsInt64 = {read=GetAsInt64, write=SetAsInt64};
	__property int AsInteger = {read=GetAsInteger, write=SetAsInteger, nodefault};
	__property double AsReal = {read=GetAsReal, write=SetAsReal};
	__property short AsSmallInt = {read=GetAsSmallInt, write=SetAsSmallInt, nodefault};
	__property AnsiString AsString = {read=GetAsString, write=SetAsString};
	__property int AtomType = {read=FAtomType, write=SetAtomType, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TPortionAtom(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE int __fastcall PortionAddrTo(const Variant &Portion);
extern PACKAGE int __fastcall PortionAddrFrom(const Variant &Portion);
extern PACKAGE void __fastcall DecodePortion(const Variant &Portion, const Classes::TStrings* Dest, 
	const bool Full);

}	/* namespace Portions */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Portions;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Portions
