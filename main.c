#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

typedef double stack_elem_t;

struct stack_t
{
    stack_elem_t* data;
    int size;
    int capacity;
};

int stack_ctor (struct stack_t* stack, int capacity);

int stack_dtor (struct stack_t* stack);

int stack_push (struct stack_t* stack, stack_elem_t elem);

int stack_pop (struct stack_t* stack, stack_elem_t* x);

int stack_dump (struct stack_t* stack);

bool stack_false (struct stack_t* stack);

int main (void)
{
    struct stack_t stack = {};

    if (stack_ctor(&stack, 10) != 0)
        printf("error in stack_ctor, check the data of stack, bro.");

    if (stack_push (&stack, 100) != 0)
        printf("error in stack_push");

    stack_push (&stack, 200);
    stack_push (&stack, 300);
    stack_push (&stack, 400);
    stack_push (&stack, 500);
    stack_push (&stack, 600);
    stack_push (&stack, 700);
    stack_push (&stack, 800);
    stack_push (&stack, 900);
    stack_push (&stack, 1000);

    stack_dump(&stack);

    stack_elem_t x = 0;

    stack_pop(&stack, &x);

    stack_dump(&stack);

    stack_dtor(&stack);

    return 0;
}

int stack_ctor (struct stack_t* stack, int capacity)
{
    assert(stack && "stack == NULL");

    stack->size = 0;

    stack->data = (stack_elem_t*) calloc((size_t) capacity, sizeof(stack->data[0]));

    stack->capacity = capacity;

    if (stack_false(stack))
    {
        stack_dump (stack);
        return 1;
    }


    return 0;
}

int stack_dtor (struct stack_t* stack)
{
    free(stack->data);

    stack->size = 0;

    stack->capacity = 0;

    return 0;
}

int stack_push (struct stack_t* stack, stack_elem_t elem)
{
    assert(stack && "stack == NULL");

    if ((stack->size) >= (stack->capacity))
    {
        stack->capacity *= 2;
        stack->data = (stack_elem_t*) realloc (stack->data, (size_t) stack->capacity * sizeof(stack->data[0]));

        if (stack->data == NULL)
            return 1;
    }

    stack->data[stack->size++] = elem;

    return 0;
}

int stack_pop (struct stack_t* stack, stack_elem_t* x)
{
    if (stack->size == 0)
        return 1;

    *x = stack->data[stack->size-- - 1];

    return 0;
}

int stack_dump (struct stack_t* stack)
{
    printf("the data in the stack with capacity = %d and size = %d looks like this:\n", stack->capacity, stack->size);

    for (int i = 0; i < stack->size; i++)
        printf("%g\n", stack->data[i]);

    return 0;
}

bool stack_false (struct stack_t* stack)
{
    if (stack->data == 0)
        return true;

    if (stack->size == NAN)
        return true;

    if (stack->capacity == NAN)
        return true;

    return false;
}
