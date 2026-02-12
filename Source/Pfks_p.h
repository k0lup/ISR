#include <stdio.h>
/* #include "stdio_zz.h" */
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <io.h>
//#include <window.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include <errno.h>
#include <alloc.h>
#include <malloc.h>
#include <process.h>
//#include <syserr.h>
//#include "pfks_df.h"
//======== Include ==================================*/
/*#include "pfks_p.h"
#include "asmpro.a"
#include "asmpro.h"
#include "pfks_df.h"
#ifndef  AVTONOM
//  #include  "diagtxt.h"
#endif
  */
/*======== Define ===================================*/
#define MINDISKSPACE  20
#ifdef AVTONOM
  #define MAXOUTLENGTH  120     //61
#else
  #define MAXOUTLENGTH  120      //57
#endif
#define W_YARL        100
#define R_YARL        46
#define StrLen        256
#define SearchRec     struct ffblk //struct find_t
#define cComment      "//"
#define cCompExt      ".kss"//KSS"
#define cDocExt       ".ksd"//KSD"
#define cKSExt        ".ksf"//KSF"
#define cErrExt       ".kse"//KSE"
#define cOldKSExt     ".ksb"//KSB"
#define cRazdExt      ".ksp"//KSP"
#define cTmpExt       ".k$$"
/*#define cCompExt      ".KSS"
#define cDocExt       ".KSD"
#define cKSExt        ".KSF"
#define cErrExt       ".KSE"
#define cOldKSExt     ".KSB"
#define cTmpExt       ".K$$"*/
#define CCH           13
#define FALSE         0
#define TRUE          1
#define SkipPassportLines  1
#define pos_kc        2025

/*/======== Const =====================================*/
static char *CFilType[] =  { ".DIP", ".DII", ".TMP", ".SET",
                             ".dii", ".dip", ".tmp", ".set",
                            ".DB", ".PX", ".Y02", ".X02", ".MB",
                            ".Y03", ".X03",".YG0", ".XG0" };


/*/======== Types =====================================*/
typedef enum {CI_     =0, GTS_  =1, TPK_  =2, OTHER_ = 3} TMode;
typedef enum {NoWrite_=0, Write_=1, Match_=2} TOpt;

typedef unsigned char THeaderP [80];
typedef char StringP  [StrLen] ;
typedef unsigned char  unchar;

typedef struct {  unsigned int f1 : 5;                /*/sec*/
                  unsigned int f2 : 6;                /*/min*/
                  unsigned int f3 : 5; } ttm_;        /*/hour*/

typedef struct {  unsigned int f1 : 5;                /*/day*/
                  unsigned int f2 : 4;                /*/mon*/
                  unsigned int f3 : 7; } ddt_;        /*/year*/
typedef union  {
           ttm_         tm;
           unsigned int time;
}  tm_;

typedef union  {
           ddt_          dt;
           unsigned int  date;
} dt_;

typedef void     *pointer;
typedef unsigned int word;

typedef struct nod_ {
        struct nod_     *link;
          char     *line;
} nod__;

/*/======== functions =================================*/
/*void far IVal            (char *, int *, int *);
void far FSplit          (AnsiString, AnsiString, AnsiString, AnsiString);
void far FConcat         (StringP, StringP, StringP, StringP);
int  far FindFirst       (StringP, SearchRec *);
int  far FindNext        (SearchRec *s);
void far Trim            (StringP);
int  far Pos             (StringP, StringP);
char far *Copy           (char *, int, int);
void far GetListFileName (int, AnsiString, AnsiString);
int  far Srav            (SearchRec *, StringP);
void far InfDKO(char *s, int i);
long int GetLength_from_InpFile(StringP s);
  */

void far IVal            (char *, int *, int *);
void far FSplit          (StringP, StringP, StringP, StringP);
void far FConcat         (StringP, StringP, StringP, StringP);
int far FindFirst       (StringP, SearchRec *);
int far FindNext        (SearchRec *s);
void far Trim            (StringP);
int  far Pos             (StringP, StringP);
char far *Copy           (char *, int, int);
void far GetListFileName (int, StringP, StringP);
//int  far Srav            (SearchRec *, StringP);
//int Srav            (StringP, StringP);
void far InfDKO(char *s, int i);
long int GetLength_from_InpFile(StringP s);


int  far KSCount         (StringP, int, StringP);  //P
int  far KSCount0        (StringP, int, StringP, StringP); //P

//unsigned long
unsigned long CIRSUM     (unsigned long, unsigned long);
void far GetKS           (StringP, StringP); //P
void far UnpackTime      (SearchRec *, tm_ *);
void far UnpackDate      (SearchRec *, dt_ *);
void far GetAllDir       ();
char far *SearchFile     (char *Name, nod__ *Ptr);
void far pfks            (int mode, char *fname );
//int NoNN(AnsiString);

