#ifndef LINKED_LIST_H
#define LINKED_LIST_H

//Taken from https://github.com/skorks/c-linked-list

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct node {
  pid_t PID;
  struct node* next;
} Node;

typedef struct list {
  Node* head;
} List;

List* new_list();

void add(pid_t pid, List* list);

int delete(pid_t pid , List* list);

void display(List* list);

void reverse(List* list);

void destroy(List* list);

//Added Functionality
pid_t get_pid_at(List* list, int index);

int del_all_of_PID( pid_t pid, List* list );

uint32_t size( List* list );

pid_t del_first(List* list);

#endif
