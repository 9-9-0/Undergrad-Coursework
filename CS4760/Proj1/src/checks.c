#include <string.h>
#include <ctype.h>

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
