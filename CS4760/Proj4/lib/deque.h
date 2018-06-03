//https://gist.github.com/Wollw/2318276
/* An implementation of a Double Ended Queue.
 * Pushes, peeks, and pops from both ends can be done and
 * an allocate and free function is provided to instantiate a
 * deque.  As the implementation of the deque struct is in the
 * source file "deque.c" it is required that the deque_alloc function
 * be used to create a new deque providing a form of encapsulation.
 */
#ifndef DEQUE_H
#define DEQUE_H
#include <stdbool.h>

/* deque_type is a deque object that the deque_foo functions
 * act upon.  New deque_type objects can be created with deque_alloc.
 */
typedef struct deque_struct deque_type;

/* This deque_val_type is the type of all values stored in the deque.
 * Change this to the type you want to store in your deque.
 */
typedef long deque_val_type; //long to store child PIDs

/* Instantiates a new deque_type.
 * Returns a pointer to a new deque_type with memory allocated by malloc,
 * returns NULL if it fails.
 */
deque_type * deque_alloc();

/* Frees a deque_type pointed to by *d */
void deque_free(deque_type *d);

/* Returns true if there are no values in the deque, false otherwise. */
bool deque_is_empty(deque_type *d);

/* Adds a new value to the front/back of the deque_type *d */
void deque_push_front(deque_type *d, deque_val_type v);
void deque_push_back(deque_type *d, deque_val_type v);

/* Removes and returns the front/back value of the deque *d */
deque_val_type deque_pop_front(deque_type *d);
deque_val_type deque_pop_back(deque_type *d);
deque_val_type deque_pop_pid(deque_type *d, deque_val_type pid); 

/* Returns the front/back value of the deque *d */
deque_val_type deque_peek_front(deque_type *d);
deque_val_type deque_peek_back(deque_type *d);

/* Returns the value of the nth element from the front of the queue. 0-indexed */
deque_val_type deque_nth_front(deque_type *d, int ind);

/* Returns the size of the deque *d */
int deque_get_size(deque_type *d);


#endif
