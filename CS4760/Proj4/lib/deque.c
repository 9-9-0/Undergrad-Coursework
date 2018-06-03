//https://gist.github.com/Wollw/2318276
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "deque.h"

struct node_struct {
	struct node_struct *next;
	struct node_struct *prev;
	deque_val_type val;
};

struct deque_struct {
	struct node_struct *head;
	struct node_struct *tail;
};

deque_type * deque_alloc() {
	deque_type *d = malloc(sizeof(deque_type));
	if (d != NULL)
		d->head = d->tail = NULL;
	return d;
}

void deque_free(deque_type *d) {
	free(d);
}

bool deque_is_empty(deque_type *d) {
	return d->head == NULL;
}

void deque_push_front(deque_type *d, deque_val_type v) {
	struct node_struct *n = malloc(sizeof(struct node_struct));
	assert(n != NULL);
	n->val = v;
	n->next = d->head;
	n->prev = NULL;
	if (d->tail == NULL) {
		d->head = d->tail = n;
	} else {
		d->head->prev = n;
		d->head = n;
	}
}

void deque_push_back(deque_type *d, deque_val_type v) {
	struct node_struct *n = malloc(sizeof(struct node_struct));
	assert(n != NULL);
	n->val = v;
	n->prev = d->tail;
	n->next = NULL;
	if (d->head == NULL) {
		d->head = d->tail = n;
	} else {
		d->tail->next = n;
		d->tail = n;
	}
}

deque_val_type deque_pop_front(deque_type *d) {
	deque_val_type v = d->head->val;
	struct node_struct *n = d->head;
	if (d->head == d->tail)
		d->head = d->tail = NULL;
	else
		d->head = n->next;
	free(n);
	return v;
}

deque_val_type deque_pop_back(deque_type *d) {
	deque_val_type v = d->tail->val;
	struct node_struct *n = d->tail;
	if (d->head == d->tail)
		d->head = d->tail = NULL;
	else
		d->tail = n->prev;
	free(n);
	return v;
}


//Searches starting from the head, shit code
deque_val_type deque_pop_pid(deque_type *d, deque_val_type pid) {
	//int index = 0;
	struct node_struct* n = d->head;
	deque_val_type pid_return = -1;

	//Empty queue
	if (n == NULL) {
		return pid_return;
	}

	//If only 1 in queue
	if (d->head == n && d->tail == n) {
		if (n->val == pid) {
			pid_return = n->val;
			d->head = NULL;
			d->tail = NULL;
			free(n);
			return pid_return;
		}
		else {
			return pid_return;
		}
	}

	//More than 1 in queue, check first
	if (n->val == pid) {
		pid_return = n->val;
		d->head = n->next;
		free(n);
		return pid_return;
	}

	n = n->next;
	//Check everything after first through last.
	while (n != NULL) {
		if (n->val == pid) {
			//If the value is the last entry.
			if (n == d->tail) {
				pid_return = n->val;
				n->prev->next = NULL;
				d->tail = n->prev;
				free(n);
				return pid_return;
			}

			//If the value is in between the first and last.
			pid_return = n->val;
			n->prev->next = n->next;
			n->next->prev = n->prev;
			free(n);
			return pid_return;
		}
		n = n->next;
	}

	return pid_return;
}

deque_val_type deque_peek_front(deque_type *d) {
	return d->head->val;
}

deque_val_type deque_peek_back(deque_type *d) {
	return d->tail->val;
}

deque_val_type deque_nth_front(deque_type *d, int ind) {
	if (ind > deque_get_size(d) - 1) {
		fprintf(stderr, "deque_nth_front() out of bounds, index: %d\n", ind);
		fprintf(stderr, "Exiting...\n");
		exit(1); //Duck tape for now.
	}
	else {
		struct node_struct* n = d->head;
		for (int i = 0; i < ind; i++) {
			n = n->next;
		}
		return n->val;
	}
}

int deque_get_size(deque_type *d) {
	struct node_struct *n = d->head;
	int size = 0;
	if (d->head == NULL) {
		return size;
	}

	size = 1;
	while (n != d->tail) {
		size++;
		n = n->next;
	}

	return size;
}
