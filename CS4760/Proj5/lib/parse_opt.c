#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
