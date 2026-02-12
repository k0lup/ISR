//		parnas.h

#define MAX_PARN_STR		100
/* коды ошибок */
#define	E_PARN_EOF	1	/*ош. parnas конец файла - нет параметра	*/
#define E_PARN_INIT	2	/*ош. parnas файл настройки уже открыт		*/
#define E_PARN_STR	3	/*ош. parnas ош. в строке					*/
#define E_PARN_LN	4	/*ош. parnas мала длина буфера				*/
#define E_PARN_OPEN	5	/*ош. parnas файл	не открыт				*/
#define E_PARN_SEEK	6	/*ош. parnas		не сдвинулся			*/
#define	E_PARN_READ	7	/*ош. parnas		не читается				*/
#define E_PARN_NOINIT 8	/*ош. parnas файл настройки не открыт		*/

extern	int openparn(char *ftek, char *fsys);
extern	int	closeparn(void);
extern	int getparn(char *name, char *var, int ln);

