#include "../lib/oss_management.h"
#include "../lib/data_structs/stack.h"

//gcc hash_table_stack_tests.c ../lib/oss_management.c ../lib/common_management.c ../lib/linked_list.c ../lib/data_structs/granted_stack.c

Stack freeIndexes;
entry* map = NULL;
child_id_entry* child_id_map = NULL;

int main() {
  int child_ind;
  entry* holder;
  int pid_temp;

  //Testing child_id_entry map
  for (int i = 0; i < max_child_procs; i++) {
    pid_temp = i + 1000;
    add_child_id(&child_id_map, i, pid_temp);
  }
  int num_ids = HASH_COUNT(child_id_map);
  printf("there are %u child index\n", num_ids);
  print_child_ids(&child_id_map);


  //Main Test
  Stack_Init(&freeIndexes);
  for (int k = max_child_procs - 1; k >= 0; k--) {
    Stack_Push(&freeIndexes, k);
  }

  for (int i = 100; i < 118; i++) {
    child_ind = Stack_Top(&freeIndexes);
    fprintf(stderr, "Child Index Popped: %d\n", child_ind);
    Stack_Pop(&freeIndexes);
    add_pid(&map, i, child_ind);
  }
  unsigned int num_users;
  num_users = HASH_COUNT(map);
  printf("there are %u users\n", num_users);

  for (int i = 100; i < 118; i++) {
    holder = find_pid(&map, i);
    if (holder != NULL) {
      fprintf(stderr, "PID %d has child ID: %d\n", i, holder->procInd);
      Stack_Push(&freeIndexes, holder->procInd);
    }
  }

  entry* temp = find_pid(&map, 100);
  delete_pid(&map, temp);

  num_users = HASH_COUNT(map);
  printf("there are %u users\n", num_users);
  print_pids(&map);

  for (int k = max_child_procs - 1; k >= 0; k--) {
    child_ind = Stack_Top(&freeIndexes);
    fprintf(stderr, "Child Index Popped: %d\n", child_ind);
    Stack_Pop(&freeIndexes);
  }


  delete_all(&map);
  delete_all_child_ids(&child_id_map);

}
