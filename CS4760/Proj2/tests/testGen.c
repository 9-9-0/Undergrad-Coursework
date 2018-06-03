#include <stdio.h>

int main(int argc, char* argv[]) {
  FILE* fp = fopen("./testInputFile2.txt", "a");

  char buf[4];
  for (int i = 0; i < 500; i++) {
    sprintf(buf, "%d\n", i);
    fputs(buf, fp);
  }
  fclose(fp);
}
