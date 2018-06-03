#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>

typedef uint32_t uint;
typedef int32_t int32;

typedef struct {
  pid_t PID;  //Sanity checking.
  int page;   //Contains the page of the byte that was requested. Sanity checking
  int status; //For now, this value will only ever be 1, denoting a successful grant.
} req_success_msg_contents;

typedef struct {
  long mtype;
  req_success_msg_contents contents;
} req_success_msg;

#endif
