#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdbool.h>

struct stack;
typedef struct stack *stackptr;

stackptr new_stack(void);

void delete_stack(stackptr st);

void stack_push(stackptr st, void *element);

void *stack_pop(stackptr st);

void *stack_get(stackptr st, size_t index);

void *stack_peek(stackptr st);

size_t stack_size(stackptr st);

bool stack_is_empty(stackptr st);

#endif
