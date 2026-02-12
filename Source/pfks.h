#ifndef _PFKS_H
#define _PFKS_H

#ifdef __DLL__
# define DLL_EI __declspec(dllexport)
#else
# define DLL_EI __declspec(dllimport)
#endif


//extern "C" char * DLL_EI Code_Dec(char *s,char Key);
#define StrLen        120
typedef char StringP  [StrLen] ;
extern "C" int far DLL_EI KSCount(StringP FilName, int Opt, StringP KS);
extern "C" int far DLL_EI KSCount0(StringP FilName, int Opt, StringP KS, StringP HeadKS);
#endif



