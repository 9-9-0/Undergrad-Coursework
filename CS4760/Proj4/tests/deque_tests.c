#include <stdio.h>
#include <assert.h>
#include "../lib/deque.h"

int main() {
	deque_type *d = deque_alloc();
	assert(d != NULL);

	/*
	for (long i = 1; i <= 20; i++) {
		deque_push_back(d, i);
    printf("Size: %d\n", deque_get_size(d));
	}
	*/
	for (long i = 1; i <= 3; i++) {
		deque_push_back(d, i);
    printf("Size: %d\n", deque_get_size(d));
	}

	deque_val_type test_val;

	test_val = deque_pop_pid(d, 20);
	printf("Value Popped: %ld\n", test_val);
  printf("Size: %d\n", deque_get_size(d));

	test_val = deque_pop_pid(d, 2);
	printf("Value Popped: %ld\n", test_val);
  printf("Size: %d\n", deque_get_size(d));
	
	printf("DEQUE POP 1 %ld\n", deque_pop_pid(d, 1));
	printf("Size: %d\n", deque_get_size(d));

	printf("DEQUE POP 3 %ld\n", deque_pop_pid(d, 3));
	printf("Size: %d\n", deque_get_size(d));

	/*
	while (deque_is_empty(d) != true) {
		printf("%ld\n", deque_pop_front(d));
	}
	*/

	deque_free(d);





	return 0;
}
