//---------------------------------------------------------------------------

//#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#include "pfks_p.h"
#include "pfks_df.h"
//#include "stringconst.hpp"
#include "restr.hpp"

//---------------------------------------------------------------------------
unsigned long int CirSum(unsigned long int x,unsigned long int y)
{
    int nXHigh = (x & 0xffff0000) >> 16;
    int nXLow = x & 0x0000ffff;
    int nYHigh = (y & 0xffff0000) >> 16;
    int nYLow = y & 0x0000ffff;
    bool bCarry = false;

    // сложим мл. байты
    int nLow = nXLow + nYLow;
    // если переполнение
	if (nLow > 0x0000ffff)
	{
        // удалим переполнение
        nLow = nLow & 0x0000ffff;
        // флаг переполнения
        bCarry = true;
    }
    // сложим ст. байты
    int nHigh = nXHigh + nYHigh + (bCarry ? 1 : 0);
    bCarry = false;
    // если переполнение
	if (nHigh > 0x0000ffff)
	{
        // удалим переполнение
        nHigh = nHigh & 0x0000ffff;
        // флаг переполнения
        bCarry = true;
    }
    // если переполнение при сложении старших разрядов, то +1 к младшему
    nLow += bCarry ? 1 : 0;
    // удалим переполнение, если есть
    nLow = nLow & 0x0000ffff;
    // вернем nHigh nLow
    return ((nHigh << 16) + nLow);
}
//=========================================
char UpCaseP(char a){
//       ~~~~~~       
  if((a >= 97)&&(a<=122)) return ('a'-32);
  else return a;
};// UpCase
//---------------------------------------------------------------------------
void FSplit (StringP file, StringP dir, StringP name, StringP ext)
//       ~~~~~~
{  StringP  drive;
   drive[0] = 0;
   _splitpath(file,drive,dir,name,ext);
   strcat(drive,dir);
   strcpy(dir, drive);
};// FSplit
//===============================================
int Pos (StringP pattern, StringP s) {
//      ~~~
   char *res;
   res = strstr(s,pattern);
   if (res == NULL)
      return 0;
   else
      return (int)(res - s + 1);
};// Pos
//===============================================

// Подпрограмма подсчета контрольной суммы KSCounter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FileName - имя файла с расширением, контрольная сумма которого считается
// Opt      - режим счета:
//            0-2 : контрольная сумма считается и заносится в шапку файла
//              3 : контрольная сумма считается и сравнивается с суммой,
//                  записанной в шапке
// KS       - значение подсчитанной контрольной суммы в виде строки
//
// KSCounter возвращает в режимах 0-2 всегда TRUE
//                      в режиме 3 :  TRUE  - суммы совпали
//                                    FALSE - суммы не совпали
// Запись и контроль КС только в файлах типа: .DIP, .GTS, .TPK
// в остальных - только счет и возврат TRUE! 

int KSCount(char *FilName, int Opt, char *KS) {
//      ~~~~~~~
   int   ret;
   char  OldKS[9];

//ShowMessage("DLL:  KSCount ");
   ret = KSCount0(FilName, Opt, KS, OldKS);
   if (ret == 0) return TRUE;
   else          return FALSE;

};// KSCount

//================================================/
//KSCount0  return:
//    0  - all is OK
//    1  - no file
//    2  - no passport in file .DIP type
//    3  - different KS in file head and file
//Parameters:
//    FilName - file name checked file (in)
//    Opt     - mode                   (in)
//    KS      - counted sum            (out)
//    HeadKS  - sum from header        (out)
//===================================================
int KSCount0(char *FilName, int Opt, char *KS, char *HeadKS)
{
  static int err;
  int      Find, equ;
  StringP   OldKS, D, N, E, tmp, S;
  TOpt     What;
  TMode    Mode = CI_;
  unsigned int  I, i;
  FILE *   Fil;
  THeaderP  Header;
  unsigned long int LongintKS=0, CurCELL, KSPos;
//--------------------------------------------------
  err = 0;
//  errno = 0;
  strcpy(KS,"00000000");
////////////////
//  cprintf("KS(1)=%s\n",KS);
//  cprintf("LongintKS=%08lX\n",LongintKS);
////////////////
//ShowMessage("параметр AVTONOM = "AnsiString(AVTONOM));// + AnsiString(WRITE_KS_TO_HEADER));
  switch (Opt) {
     case 0:
     case 1:
     case 2: What = Write_; break;
     case 3: What = Match_; break;
     default: break;
  };// switch opt

  FSplit(FilName,D,N,E);
//  for(i=1;i<strlen(E);i++) {
//    E[i] = UpCaseP(E[i]);
//  };
  Find = FALSE;
//  for (Mode=CI_; Mode <= TPK_;  Mode++) {
  for (i=0; i <= 16;  i++) {
    strcpy(tmp,CFilType[ i ]);//I->Mode
    if(strcmp(E,tmp) == 0) {
      Find = TRUE;     //Find = 1 - есть такое расширение
    if(i>7&&i<17) Mode = GTS_;
       break;
    };// if
  };// for
  if(! Find) Mode = OTHER_;
  Fil = fopen(FilName,"rb");
  if( Fil == NULL ) {
    KS[0] = 0;
///////////
//  cprintf("KS(2)=%s\n",KS);
//  cprintf("LongintKS=%08lX\n",LongintKS);
///////////
    err = 1;  // no file
    return err;
  };
  if (feof(Fil)) {
    err = 0;
    fclose(Fil);
    return err;
  };
//---- skip header--------
  OldKS[0] = 0;
  AnsiString PassportSymbol;
  switch(Mode)
  {
    case CI_:
      fgets(tmp,StrLen,Fil);
      I = Pos("=",tmp);
//      PassportSymbol = Restr_paspStr;
	  PassportSymbol = "П";
	  if ( (tmp[0] == 'П' || tmp[0] == 'P') && I != NULL )
	   {
		strncpy( OldKS, &tmp[I], 8 );
        OldKS[8] = 0;
		strcpy( HeadKS, OldKS );
        KSPos = I;
	   }
	  else
	   {
		 OldKS[0] = 0;
		 KSPos = 0;
		 fseek( Fil, KSPos, SEEK_SET );
		 err = 2;  // no passport
	   };
      break;
    case GTS_:
    case TPK_:
             fseek( Fil, pos_kc, SEEK_SET );//2025
     fread(Header, 11, 1, Fil);//80
      memmove(OldKS, &Header[3], 8);
      OldKS[8] = 0;
      strcpy(HeadKS,OldKS);
      KSPos = 0;
//     fseek(Fil, 80, SEEK_SET);
//////////////
//  cprintf("HeadKS=%s\n",HeadKS);
//  cprintf("KS(3)=%s\n",KS);
// cprintf("LongintKS=%08lX\n",LongintKS);
//////////////
      break;
    default:
      KSPos = 0;
      OldKS[0] = 0;
      break;
  };// switch

//--- count KS ----------
  LongintKS = 0;
//////////////
//  cprintf("KS(4)=%s\n",KS);
//  cprintf("LongintKS=%08lX\n",LongintKS);
//////////////
  CurCELL=0;
  while( ! feof(Fil)) {
    fread(&CurCELL,4,1,Fil);
 //cprintf("CurCELL=%08lX\n",CurCELL);
//    if(CurCELL!=0){
//? //     if(! feof(Fil)) {

//    LongintKS = CIRSUM(LongintKS, CurCELL);
    LongintKS = CirSum(LongintKS, CurCELL);
//Form1->Edit1->Text=AnsiString(LongintKS);
//  cprintf("LongintKS(%d)=%08lX\n",ii,LongintKS); ii++;
//  cprintf("KS=%s\n",KS);
//  cprintf("CurCELL=%08lX\n",CurCELL);
////////////////
//?   //         }
  };// while

  fclose(Fil);

     sprintf(KS,"%08lX", LongintKS);
///////////////////
//  sprintf(buf,"%08lX", LongintKS);
//  cprintf("*** LongintKS=%08lX\n",LongintKS);
////////////////////
  if( ((OldKS[0] == 'X')||(OldKS[0] == 'Х'))&&((Mode == GTS_)||(Mode == TPK_))) {
    equ = -5;
  }
  else {
    equ = strcmp(KS,OldKS);
  };
  if (Mode == OTHER_) {
    equ = 0;
  };
  if (err != 0) return err;

//--- post processing ---
  switch (Mode) {
    case CI_:
      if ( equ != 0 ) {
#ifdef WRITE_KS_TO_HEADER
        if ( (What == Write_)&&(err == 0) ) {
          // printf("== 1 == %d ,%s, %s\n",equ,KS,OldKS);
	  Fil = fopen(FilName,"rb+");
          fseek(Fil, KSPos, SEEK_SET);
          fputs(KS,Fil);
          fclose(Fil);
        };
#endif
        if (equ != -5) {
          err = 3;
        }
        else {
          err = 0;
	};
      }
      else err = 0;
        break;
    case GTS_:
    case TPK_:
      if ( equ != 0 ) {
#ifdef WRITE_KS_TO_HEADER
        if ( (What == Write_)&&(err == 0) ) {
	  strcpy(S,"KS=");
          strcat(S,KS);
          memcpy(Header,S,strlen(S));
          Fil = fopen(FilName,"rb+");
          fseek(Fil,pos_kc,SEEK_SET);//2025
//          fwrite(Header,80,1,Fil);
fwrite(Header,11,1,Fil);
          fclose(Fil);
        };
#endif
        if (equ != -5) {
          err = 3;
        }
	else {
          err = 0;
        };
      }
      else err = 0;
      break;
    default:
      break;
    };// case
  return err;
};// KSCount
