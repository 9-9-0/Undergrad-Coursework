#include <string.h>
#include <stdio.h>

int isPalin(char* str, int index) {
  int endInd = strlen(str) - (index + 1);

  //fprintf(stderr, "endInd: %d strLen: %d %c %c\n", endInd, strlen(str), str[index], str[endInd]);

  if (str[index] == str[endInd])
  {
    if (index + 1 == endInd || index == endInd)
    {
        return 1;
    }
    isPalin(str, index + 1);
  }
  else
  {
    return 0;
  }

  return -1; //Should not return
}
