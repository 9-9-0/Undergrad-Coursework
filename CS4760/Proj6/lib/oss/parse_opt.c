#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parse_opt.h"

void getHelp(char progName[], const char usage[], const char help[]) {
  fprintf(stderr, usage, progName, progName);
  fprintf(stderr, "%s", help);
  exit(0);
}

void badUse(char progName[], const char usage[]) {
    fprintf(stderr, usage, progName, progName);
    exit(1);
}

int isNumber(char* str) {
  int ind = 0;
  int retval = 0;
  int length = strlen(str);

  while (ind < length) {
    retval = isdigit(str[ind]);
    if (retval == 0) {
      return 0;
    }
    ind++;
  }

  return 1;
}
