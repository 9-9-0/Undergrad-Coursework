#include <string.h>
#include <ctype.h>
#include <unistd.h>

//Input checking functions used by getopt()

int isNumber(char* str) {
  int ind = 0;
  int retval = 0;
  int length = strlen(str);

  while (ind < length) {
    retval = isdigit(str[ind]);
    if (retval == 0) {
      return retval;
    }
    ind++;
  }

  retval = 1;
  return retval;
}

int isFile(char* str) {
  //ERRNO set if -1 is returned
  if (access(str, F_OK|R_OK ) != -1 ) {
    return 1;
  }
  else {
    return 0;
  }
}
