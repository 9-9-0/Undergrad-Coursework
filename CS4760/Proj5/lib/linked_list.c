#include "linked_list.h"

Node* createnode(pid_t pid){
  Node* newNode = malloc(sizeof(Node));
  newNode->PID = pid;
  newNode->next = NULL;
  return newNode;
}

List* new_list(){
  List* list = malloc(sizeof(List));
  list->head = NULL;
  return list;
}

void display(List* list) {
  Node* current = list->head;
  if(list->head == NULL)
    return;
  while(current->next != NULL){
    printf("%d,", current->PID);
    current = current->next;
  }
  printf("%d\n", current->PID);
}

void add(pid_t pid, List* list){
  Node * current = NULL;
  if(list->head == NULL){
    list->head = createnode(pid);
  }
  else {
    current = list->head;
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = createnode(pid);
  }
}

/* Delete the first occurrence of PID. */
int delete(pid_t pid, List* list){
  Node* current = list->head;
  Node* previous = current;

  int deleted = 0;

  while(current != NULL){
    if(current->PID == pid){

      previous->next = current->next;

      if(current == list->head)
        list->head = current->next;

      deleted += 1;
      free(current);
      return deleted;
    }
    previous = current;
    current = current->next;
  }
  return deleted;
}

void reverse(List* list){
  Node* reversed = NULL;
  Node* current = list->head;
  Node* temp = NULL;
  while(current != NULL){
    temp = current;
    current = current->next;
    temp->next = reversed;
    reversed = temp;
  }
  list->head = reversed;
}

void destroy(List* list){
  Node* current = list->head;
  Node* next = current;

  while(current != NULL){
    next = current->next;
    free(current);
    current = next;
  }
  free(list);
}

/* Returns -1 on failure. */
//NOTE: Needs testing.
pid_t get_pid_at(List* list, int index) {
  if (index < -1) {
    return -1;
  }

  Node* current = list->head;
  int position = 0;

  while (position < index) {
    position += 1;
    current = current->next;

    if (current == NULL) {
      return -1;
    }
  }

  return current->PID;
}

/* Used for removal of all occurrences of pid. */
//NOTE: TEST THIS.
int del_all_of_PID( pid_t pid, List* list ) {
  int freed_count = 0;
  Node* current = list->head;
  Node* previous = current;

  //Multiple occurrences @ head.
  while (current != NULL && current->PID == pid) {
    list->head = current->next;
    free(current);
    freed_count += 1;
    current = list->head;
  }

  //The rest
  while (current != NULL) {
    //Iterate until one is hit.
    while (current != NULL && current->PID != pid) {
      previous = current;
      current = current->next;
    }

    //End is hit
    if (current == NULL) return freed_count;

    //Found an occurrence
    previous->next = current->next;
    free(current);
    freed_count += 1;
    current = previous->next;
  }

  return freed_count;
}

/* Returns the number of PIDs in the list */
uint32_t size( List* list ) {
  uint32_t list_size = 0;
  Node* current = list->head;

  while(current != NULL) {
    current = current->next;
    list_size += 1;
  }

  return list_size;
}

/* Return and delete the first PID in the list */
pid_t del_first(List* list) {
  Node* current = list->head;
  Node* next = current->next;
  pid_t return_val = 0;

  if (current != NULL) {
    return_val = current->PID;
    free(current);
    list->head = next;
  }

  return return_val;
}
