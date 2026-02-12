#ifndef _net_PRISH
#define _net_PRISH

#include <vcl.h>
#include <windows.h>
#include <ScktComp.hpp>
char* SocketError( char *buf,TCustomWinSocket *Socket,TErrorEvent ErrorEvent, int ErrorCode,unsigned int port,void*);
//------------------------------------------------------------------------------
class NET_CL_PRIS :  public TClientSocket
{
  public:
	bool SndPacket( char *Buf, unsigned short Size, BYTE From, BYTE To, BYTE Sort, BYTE Type,
			  BYTE AtmCnt, BYTE Kind, short No, BYTE s1, BYTE s2 );
	void *ptr;                 // указатель на потоковый класс
    short tmi_1_2;
	TForm *frm;
	TCustomWinSocket *OutSocket;
	NET_CL_PRIS( TForm *f_v,unsigned int port ); // для сервера
	virtual void __fastcall ClientSocketDisconnect( TObject *Sender, TCustomWinSocket *Socket );
	virtual void __fastcall ClientSocket1Error( TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode);
	virtual void __fastcall ClientSocketError( TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode);
	virtual void __fastcall ClientSocketRead( TObject *Sender, TCustomWinSocket *Socket );
	void __fastcall ClientSocketConnect( TObject *Sender, TCustomWinSocket *Socket );
	void __fastcall out( char*, int );
	char* __fastcall SocketError( char*, TCustomWinSocket*, TErrorEvent, int, unsigned int, void* );
  private:
  public:
   bool busy; // флаг занятости модуля SndPacket
   CRITICAL_SECTION p_cs;
};
#endif


