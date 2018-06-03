#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "checks.h"

int main (int argc, char *argv[]) {
  pid_t childpid = 0;
  int i, j;
  int n, k, m;

  int errno;
  static char usage[] = "%s: Usage: %s [-n # of processes] [-k # of prints] [-m seconds to sleep]\n";
  int optErr = 0;
  int c;
  extern char *optarg;
  extern int optind;

  //Check for argument count
  if (argc != 7) {
    optErr = 1;
  }
  while ((c = getopt(argc, argv, "n:k:m:")) != -1) {
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
      case 'k':
        ;
        if (isNumber(optarg)) {
          k = (int)strtol(optarg, NULL, 0);
        }
        else {
          optErr = 2;
          errno = EINVAL;
        }
        break;
      case 'm':
        ;
        if (isNumber(optarg)) {
          m = (int)strtol(optarg, NULL, 0);
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

  for (j = 0; j < k; j++){
    fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
    sleep(m);
  }
  return 0;
}
