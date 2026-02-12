#include <vcl.h>
#include <windows.h>
#include <ScktComp.hpp>
#include <iostream.h>
#include <fstream.h>
#include "net_PRIS.hpp"
#include "main.h"
#include <winbase.h>
//----------------------------------------------------------------------------
//             Конструктор для клиента
//---------------------------------------------------------------------------
NET_CL_PRIS::NET_CL_PRIS( TForm *f_v,unsigned int port ) : TClientSocket(0)
{
  OutSocket = 0 ;
  Active = false;
  frm  = f_v;
  Port = port;
  ClientType = ctNonBlocking;
  OnConnect = &ClientSocketConnect;
  OnDisconnect = &ClientSocketDisconnect;
  OnRead = &ClientSocketRead;
  if( Port == 0x4020 )
	  OnError = &ClientSocketError;
  else
	  OnError = &ClientSocket1Error;
  Address = "127.0.0.1";
  InitializeCriticalSection(&p_cs);
  busy = false; // флаг занятости модуля SndPacket
 }
//------------------------------------------------------------------------------
//                 ФУНКЦИИ ДЛЯ КЛИЕНТА
//------------------------------------------------------------------------------
bool NET_CL_PRIS::SndPacket( char *Buf, unsigned short Size, BYTE From, BYTE To, BYTE Sort, BYTE Type,
			   BYTE AtmCnt, BYTE Kind, short No, BYTE s1, BYTE s2 )
{
  if( busy )
	{
	  WriteToDebugFile( "--!!!!!!!---ИСР занят" );
	  return false;
	}
   busy = true;
   char* por = 0;
   por = new char[Size + sizeof(POR)];
   if( por )
	 {
	   ((POR*)por)->LenPor = Size + sizeof(POR);
	   ((POR*)por)->AdrTo  = To;
	   ((POR*)por)->AdrFrom= From;
	   ((POR*)por)->NumPor = No;
	   ((POR*)por)->DOP1   = s1;
	   ((POR*)por)->DOP2   = s2;

	   ((POR*)por)->LenArt = Size + 8;
	   ((POR*)por)->KindofWork = Kind;
	   ((POR*)por)->SortofWork = Sort;

	   ((POR*)por)->LenCort    = Size;
	   ((POR*)por)->AtomType   = Type;
	   ((POR*)por)->AtomQuant  = AtmCnt;

	   memcpy( por + sizeof(POR), Buf, Size );
	   out( por,((POR*)por)->LenPor );
	   busy = false;
	   delete [] por;
	   return true;
	 }
   else
	 {
	   busy = false;
	   return false;
	 }
}
//------------------------------------------------------------------------------
void __fastcall NET_CL_PRIS::ClientSocketConnect( TObject *Sender, TCustomWinSocket *Socket )
{
  AnsiString s;
  OutSocket = Socket;
  if( Port == 0x4020 )
	{
	  WriteToDebugFile( "-------CВЯЗЬ УСТАНОВЛЕНА С BINT" );
	  WriteToDebugFile( "1" );
	  SendDirectoriesToPris();
	}
  else
	  WriteToDebugFile( "------CВЯЗЬ УСТАНОВЛЕНА С PROT" );
}
//------------------------------------------------------------------------------
void __fastcall NET_CL_PRIS::ClientSocketDisconnect( TObject *Sender, TCustomWinSocket *Socket )
{
  AnsiString s;
  if( Port == 0x4020 )
	   WriteToDebugFile( "-------CВЯЗЬ РАЗОРВАНА С BINT" );
  else
	   WriteToDebugFile( "------CВЯЗЬ РАЗОРВАНА С PROT" );
 OutSocket = 0;
}
//------------------------------------------------------------------------------
void __fastcall NET_CL_PRIS::ClientSocket1Error( TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode )
{
  char* p;
  char pp[256];
  memset( pp, 0, 256 );
  p = NET_CL_PRIS::SocketError( pp, Socket, ErrorEvent, ErrorCode, Port,(void*)ptr );
  if( bPrisLoaded )
	 SendStringToPrisProtokol( p, stStandart, true, true, false );
  else
	 ShowMessage( p );
  WriteToDebugFile( p );
  if( ErrorEvent == eeDisconnect )
  {
   OutSocket = 0;
   Active = false;
  }
  OutSocket = 0;
  ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall NET_CL_PRIS::ClientSocketError( TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode )
{
  char* p;
  char pp[256];
  memset( pp, 0, 256 );
  p = NET_CL_PRIS::SocketError( pp, Socket, ErrorEvent, ErrorCode, Port,(void*)ptr );
  if( bPrisLoaded )
	 SendStringToPrisProtokol( p, stStandart, true, true, false );
  else
	 ShowMessage( p );
  WriteToDebugFile( p );
  if( ErrorEvent == eeDisconnect )
   {
	 OutSocket = 0;
	 Active = false;
   }
  OutSocket = 0;
  ErrorCode = 0;
}
//---------------------------------------------------------------------------
//              ПРИЕМ ПО СЕТИ ПО СИНХРОННЫМ КАНАЛАМ
//---------------------------------------------------------------------------
#include <winbase.h>
void __fastcall NET_CL_PRIS::ClientSocketRead( TObject *Sender, TCustomWinSocket *Socket )
{
   int l = 0, ll = 0, lll = 0;
   char* p = 0, *pp;
   l = Socket->ReceiveLength();
   if( l == 0 )
	  return;
   WriteToDebugFile( "ИСР: приняли по сети l = " + IntToStr(l) );
   try
	{
	  p = new char[l];
	}
   catch( ... )
	{
	  WriteToDebugFile( "ИСР: НЕ ВЫДЕЛИЛАСЬ ПАМЯТЬ ДЛЯ ПРИЕМА ПО СЕТИ " );
      return;
	}
   pp = p;
   lll = l;
RCV:
   Socket->Lock();
   ll = Socket->ReceiveBuf( pp, lll );
   Socket->Unlock();
   if( ll < lll )
	{
	   pp += ll;
	   lll -= ll;
	   goto RCV;
	}
    if( ll > lll)
		{
		  WriteToDebugFile( "ИСР: Сбой приёма информации от ПРИС (ош.длина)" );
		  if( p )
			delete [] p;
          return;
		}
  //
  EnterCriticalSection( &p_cs );
  ((TISRMainForm*)frm)->ProcessInPortion( p, ll );
  LeaveCriticalSection( &p_cs );
  if( p )
	  delete [] p;
}
//---------------------------------------------------------------------------
void __fastcall NET_CL_PRIS::out( char* buf, int l )
{
  int kol;
  AnsiString s;
 //
  if( Port == 0x4020 )
	 s = "ИСР(net_PRIS)";
  else
	 s = "ИСР(net_PROT)";
  try {
		 OutSocket->Lock();
		 kol = OutSocket->SendBuf( (void*)buf, l );
         OutSocket->Unlock();
         if( kol != l )
		   {
			 WriteToDebugFile( s + ": Сбой при передаче информации в ПРИС (ош.длина)");
			 ShowMessage( s + ": Сбой при передаче информации в ПРИС (ош.длина)");
			 return;
           }
      }
  catch(...) {
			   WriteToDebugFile( s + ": Сбой при передаче инф.в ПРИС");
			   ShowMessage( s + ": Сбой при передаче инф.в ПРИС");
			   return;
             }
  if( Port == 0x4020 )
	 WriteToDebugFile( s + ": Передали информацию в " + ((POR*)buf)->AdrTo );
  else
	 WriteToDebugFile( s + ": Передали информацию в протокол" );
}
//-----------------------------------------------------------------------------
//     ФУНКЦИЯ ОБРАБОТКИ ОШИБОК СЕТИ
//-----------------------------------------------------------------------------
char* __fastcall NET_CL_PRIS::SocketError( char *err_buf, TCustomWinSocket *Socket,
				 TErrorEvent ErrorEvent, int ErrorCode, unsigned int port, void* ptr )
{
  AnsiString s;
 if( Port == 0x4020 )
   s = "ИСР(net_PRIS)";
 else
   s = "ИСР(net_PROT)";
 switch( ErrorEvent )
  {
	case eeGeneral:    sprintf( err_buf,": KZ = %d"," - ОБЩАЯ ОШИБКА СЕТИ", ErrorCode );
					   break;
	case eeSend:       sprintf( err_buf,": Ошибка при передаче информации (%s): КЗ = %d", s, ErrorCode );
					   break;
	case eeReceive:    sprintf( err_buf,": Ошибка при приеме информации: КЗ = %d", ErrorCode );
					   break;
	case eeConnect:    sprintf( err_buf,": Ошибка установки связи с БИНТ: КЗ = %d", ErrorCode );
					   break;
	case eeDisconnect: sprintf( err_buf,": Потеря связи с БИНТ: KZ = %d", ErrorCode );
					   break;
   // Только для сервера:
	case eeAccept:     sprintf( err_buf,": Общая ошибка сети (Accept): КЗ = %d", ErrorCode );
					   break;
	default:           sprintf( err_buf,": Неопознанный тип события (ErrorEvent)" );
  }
 s = s  +  AnsiString( err_buf );
 return s.c_str();
}

