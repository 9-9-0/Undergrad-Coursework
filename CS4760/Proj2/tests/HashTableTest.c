#include <stdio.h>
#include <string.h>
#include "lib/uthash.h"

struct my_struct {
  int id;
  char name[10];
  UT_hash_handle hh;
};


struct my_struct *users = NULL; //Users is the table. Identify users by ID, return the structure if id found.
void add_user(struct my_struct *s);
struct my_struct* find_user(int user_id);
void delete_user(struct my_struct *user);

int main(int argc, char* argv[]) {
  struct my_struct test1 = { .id = 0, .name = "Max"};
  add_user(&test1);

  test1.id = 2;
  strcpy(test1.name, "John");
  add_user(&test1);

  struct my_struct* result = find_user(2);
  fprintf(stderr, "Name: %s\n", result->name);

  result = find_user(0);
  fprintf(stderr, "Name: %s\n", result->name);

  result = find_user(2);
  fprintf(stderr, "Name: %s\n", result->name);

  struct my_struct* result2 = find_user(3);
  if (result2 != NULL) {
    fprintf(stderr, "Name: %s\n", result2->name);
  }

  return 0;
}

void add_user(struct my_struct *s) {
  HASH_ADD_INT( users, id, s );
}

struct my_struct* find_user(int user_id) {
  struct my_struct *s;

  HASH_FIND_INT( users, &user_id, s);
  return s;
}

void delete_user(struct my_struct *user) {
  HASH_DEL( users, user);
}
