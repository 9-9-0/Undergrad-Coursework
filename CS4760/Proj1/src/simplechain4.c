#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "checks.h"


int main (int argc, char *argv[]) {
  pid_t childpid = 0;
  int i, n;
  //setvbuf(stderr, NULL, _IOLBF, 1024);

  int errno;
  static char usage[] = "%s: Usage: %s [-n # of processes]\n";
  int optErr = 0;
  int c;
  extern char *optarg;
  extern int optind;

  //Check for argument count
  if (argc != 3) {
    optErr = 1;
  }
  while ((c = getopt(argc, argv, "n:")) != -1) {
    switch (c) {
      case 'n':
        ;
        if (isNumber(optarg)) {
          n = (int)strtol(optarg, NULL, 0);
        }
        else {
          optErr = 2;
          errno = EINVAL;
        }
        break;
      case '?':
        optErr = 1;
        break;
    }
  }

  switch (optErr) {
      //Invalid # of arguments or unknown argument found
      case 1:
        fprintf(stderr, usage, argv[0], argv[0]);
        exit(1);
      //Invalid n-value
      case 2:
        fprintf(stderr, "%s: Error: %s\n", argv[0], strerror(errno));
        fprintf(stderr, usage, argv[0], argv[0]);
        exit(1);
  }

  for (i = 1; i < n; i++) {
    if ( (childpid = fork()) ) {
        break;
    }
  }

  fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
  wait(NULL);
  return 0;
}
