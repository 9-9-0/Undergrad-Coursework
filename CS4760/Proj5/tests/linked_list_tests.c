#include <stdlib.h>
#include <stdio.h>
#include "../lib/linked_list.h"

int main() {
  /*
  List* myList = new_list();

  add(123, myList);
  display(myList);
  fprintf(stderr, "Size: %d\n", size(myList));

  add(123, myList);
  add(123, myList);
  add(1234, myList);
  display(myList);
  fprintf(stderr, "Size: %d\n", size(myList));

  del_all_of_PID(123, myList);
  display(myList);
  fprintf(stderr, "Size: %d\n", size(myList));

  destroy(myList);
  */

  /*
  List* myList2 = new_list();
  add(1234, myList2);
  add(12345, myList2);
  del_first(myList2);
  display(myList2);
  fprintf(stderr, "Size: %d\n", size(myList2));

  add(123456, myList2);
  display(myList2);
  fprintf(stderr, "Size: %d\n", size(myList2));

  del_first(myList2);
  display(myList2);
  fprintf(stderr, "Size: %d\n", size(myList2));

  add(1234567, myList2);
  add(1234567, myList2);
  add(12345678, myList2);
  display(myList2);
  fprintf(stderr, "Size: %d\n", size(myList2));

  delete(1234567, myList2);
  display(myList2);
  fprintf(stderr, "Size: %d\n", size(myList2));

  destroy(myList2);
  */

  /*
  List* myList3 = new_list();
  add(1234, myList3);
  add(1234, myList3);
  add(1234, myList3);
  add(1234, myList3);
  add(1234, myList3);

  fprintf(stderr, "Num deleted: %d\n", del_all_of_PID(1234, myList3));
  display(myList3);
  fprintf(stderr, "Size: %d\n", size(myList3));

  destroy(myList3);
  */

  List* myList4 = new_list();
  add(1, myList4);
  add(12, myList4);
  add(123, myList4);
  add(1234, myList4);
  add(12345, myList4);

  fprintf(stderr, "Val @ 0 : %d\n", get_pid_at(myList4, 0));
  fprintf(stderr, "Val @ 1 : %d\n", get_pid_at(myList4, 1));
  fprintf(stderr, "Val @ 2 : %d\n", get_pid_at(myList4, 2));
  fprintf(stderr, "Val @ 3 : %d\n", get_pid_at(myList4, 3));
  fprintf(stderr, "Val @ 4 : %d\n", get_pid_at(myList4, 4));
  fprintf(stderr, "Val @ 5 : %d\n", get_pid_at(myList4, 5));
  fprintf(stderr, "Val @ 6 : %d\n", get_pid_at(myList4, 6));
  fprintf(stderr, "Val @ 7 : %d\n", get_pid_at(myList4, 7));

  destroy(myList4);

  return 0;
}
