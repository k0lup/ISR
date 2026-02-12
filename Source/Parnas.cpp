//		parnas.c
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include "parnas.h"
//#include "priserr.h"

#define MAX_PAR_FILE	2

#define TEK_PAR     1       // 0 - текущий   файл параметров
#define SYS_PAR     0       // 1 - системный файл параметров

static char *nameSpisPar[] = {  "ПРОГРАММЫ", "ПРОТОКОЛ", }; // TEK_PAR
#define lnSpisPar	(sizeof(nameSpisPar) / sizeof(nameSpisPar[0]))

static FILE *par_file[MAX_PAR_FILE] = { NULL, NULL, };  // дестрипторы

int closeparn(void)                          // закрыть файлы параметров
{
    if(par_file[TEK_PAR] != NULL) {
        fclose(par_file[TEK_PAR]);
        par_file[TEK_PAR] = NULL;
    }
    if(par_file[SYS_PAR] != NULL) {
		fclose(par_file[SYS_PAR]);
        par_file[SYS_PAR] = NULL;
    }
	return(0);
}

static int openparn1(char *fname, FILE **pfil) // открыть файл параметров
{
	char *p;

	if (*(fname+1) == ':')
		p = fname;
	else
		p = searchpath(fname);
	if(p == NULL || (*pfil = fopen(p,"r")) == NULL)
        return(E_PARN_OPEN);            // не найдено имя или не открыт
	return(0);
}

int openparn(char *ftek,char *fsys)
{                       // открыть текущий и системный файлы параметров
	int		i;

    if((par_file[TEK_PAR] != NULL && ftek != NULL) ||
       (par_file[SYS_PAR] != NULL && fsys != NULL))
        return(E_PARN_INIT);                        // уже открыты
    if(ftek == NULL && fsys == NULL)
        return(E_PARN_NOINIT);                      // не открыты
    if(ftek != NULL) {
        i = openparn1(ftek, par_file+TEK_PAR);      // открыть текущий
        if(i != 0) {
//            doperr(ftek);
            return(i);
        }
	}
    if(fsys != NULL) {
        i = openparn1(fsys, par_file+SYS_PAR);      // открыть системный
        if(i != 0) {
//            doperr(fsys);
            return(i);
        }
	}
	return(0);
}

static int findNamePar(char *name)                  // найти имя в списке
{
	int i;
	for(i = 0; i < lnSpisPar; i++)
		if(strcmp(name, nameSpisPar[i]) == 0)
			return(TEK_PAR);
	return(SYS_PAR);
}

//  Выдать значение параметра по имени
//  из файла параметров со структурой:  имя=значение
//
int getparn(char *name, char *buf, int lnbuf)
{
 char str[MAX_PARN_STR];
 char *s, *v, *b;
 int  lnstr, newln, no1, nf;
 FILE *pfile;
 for(nf = 0; nf< MAX_PAR_FILE; nf++)
   {
    if(par_file[nf] != NULL)
     {
       pfile = par_file[nf];
       if(fseek(pfile, 0, SEEK_SET) != 0)  // на начало
         	return(E_PARN_SEEK);
       *buf = 0;
       lnstr= newln = no1 = 0;
       b = buf;
       while(fgets(str,MAX_PARN_STR,pfile) != NULL)
       {
	 if(*str == '/' || *str == '\n') continue;
 	 if((s = strchr(str,'=')) == NULL) return(E_PARN_STR);
         *s = '\0';
	 if(strcmp(name,str) != 0) continue;
         v = ++s;    // нашли им
 	 if((s=strchr(v,'\n'))==NULL && (s=strchr(v,'\0'))==NULL)
			return(E_PARN_STR);
	 if(buf == NULL) return(0);   // значение не нужно
 	 *s = '\0';
	 if((s = strchr(v,'/')) != NULL && *(s+1) == '/')
         { // комментарий
	   *s = 0;											// обрезать
	   while(s != v && *--s == ' ')	// обрезать все пробелы с конца
	   	*s = 0;
	 }
	 newln = strlen(v);
	 if((lnstr += newln+no1) >= lnbuf)
            return(E_PARN_LN);      // мала длина буфера
	 if(no1 != 0)
           {
	     *b++ = ';';
	     *b = 0;
	   }
	 (void)strcpy(b, v);
	  b += newln;
	  no1 = 1;
	}
    if(lnstr != 0) break; // нашли имя хотя бы один раз
   }
 }
 if(nf == MAX_PAR_FILE) return(E_PARN_EOF); //  нет имени
 else return(0);			// нашли имя
}