#ifndef PARSE_OPT_H
#define PARSE_OPT_H

void getHelp(char progName[], const char usage[], const char help[]);

void badUse(char progName[], const char usage[]);

int isNumber(char* str); 
#endif
