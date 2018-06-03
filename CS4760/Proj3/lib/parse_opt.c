#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//Input checking functions used by getopt()

int isNumber(char* str) {
  int ind = 0;
  int retval = 0;
  int length = strlen(str);

  while (ind < length) {
    retval = isdigit(str[ind]);
    if (retval == 0) {
      errno = EINVAL;
      return 0;
    }
    ind++;
  }

  return 1;
}

int isFile(char* str) {
  //ERRNO set if -1 is returned
  if (access(str, W_OK) != -1 ) {
    return 0;
  }
  else {
    return 1;
  }
}

void badUse(char progName[], const char usage[]) {
    fprintf(stderr, usage, progName, progName);
    exit(1);
}

//Exits if ERRNO is set
int errCheck(char progName[], int errnoval) {
  if (errnoval) {
    fprintf(stderr, "%s: Error: %s\n", progName, strerror(errnoval));
    return 1;
  }
  return 0;
}

void getHelp(char progName[], const char usage[], const char help[]) {
  fprintf(stderr, usage, progName, progName);
  fprintf(stderr, "%s", help);
  exit(0);
}
