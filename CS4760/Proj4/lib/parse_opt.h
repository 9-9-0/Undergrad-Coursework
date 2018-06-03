#ifndef parse_opt_h__
#define parse_opt_h__


extern int isNumber(char* str);
extern int isFile(char* str);
extern void badUse(char progName[], const char usage[]);
extern int errCheck(char progName[], int errnoval);
extern void getHelp(char progName[], const char usage[], const char help[]);

#endif
