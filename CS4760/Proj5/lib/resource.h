#ifndef RESOURCE_H
#define RESOURCE_H

#include "linked_list.h"

//No need for an actual "resource" unless the allocated pids go here.
typedef struct {
  int holder;
} resource;

typedef struct {
  uint32_t resource_class_ind;
  uint32_t num_instances;
  int is_shareable;

  List* pid_allocations;
  List* pid_requests;

} resource_control;

#endif
