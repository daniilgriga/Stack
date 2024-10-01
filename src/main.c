#define CANARY_PROTECTION
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "color_print.h"

#ifdef CANARY_PROTECTION
    #define CNR_PRTCT(...) __VA_ARGS__
#else
    #define CNR_PRTCT(...)
#endif

#ifdef DEBUG
    #define DBG(...) __VA_ARGS__
#else
    #define DBG(...)
#endif

typedef double stack_elem_t;

#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wlarger-than=8192"
#pragma GCC diagnostic ignored "-Wstack-protector"


struct stack_str
{
    CNR_PRTCT(int canary_stack_1;)

    stack_elem_t* data;
    int size;
    int capacity;

    CNR_PRTCT(int canary_stack_2;)
};

DBG (struct test_str            //
{                               //      |----->|
    char executioner_1[1];      //   [----][----------------][-----]               [----------------]
    struct stack_str stack;     //                                                   |----->|
    char executioner_2[1];      //   [----][----------------][-----]---------------[---]-------------[---]-----
};  )                           //      \                        \                   ^                 ^
                                //       \                        \_________________/                 /
enum stack_condition            //        \__________________________________________________________/
{                               //
    STACK_IS_OK,
    STACK_IS_BAD
};

#define CANARY_VALUE (0xBAD)
#define CANARY_BUFFER_VALUE (0xEDA)

enum stack_condition stack_ctor (struct stack_str* stack, int capacity);

int STACK_ASSERT (struct stack_str* stack DBG(, const char* file, int line, const char* func));

int stack_dtor (struct stack_str* stack);

int stack_push (struct stack_str* stack, stack_elem_t elem);

int stack_pop (struct stack_str* stack, stack_elem_t* x);

int stack_dump (struct stack_str* stack DBG(, const char* file, int line, const char* func));

bool stack_ok (const struct stack_str* stack);

int torture_stack (struct stack_str* stack, int number_canaryes_check);

int main (void)
{
          struct stack_str stack = {};
    DBG ( struct test_str test = {};)

    if (stack_ctor(&stack, 4) == STACK_IS_BAD)
        printf("error in stack_ctor, check the data of stack, bro.\n\n");

    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    if (stack_push (&stack, 10) != 0)
        printf("error in stack_push");
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 20);
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 30);
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 40);
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 50);
    stack_push (&stack, 60);
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_elem_t x = 0;

    stack_pop(&stack, &x);
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    torture_stack(&stack, 0);

    stack_dtor(&stack);

    return 0;
}

enum stack_condition stack_ctor (struct stack_str* stack, int capacity)
{
    //STACK_ASSERT(stack);

    CNR_PRTCT ( stack->canary_stack_1 = CANARY_VALUE;
                stack->canary_stack_2 = CANARY_VALUE; )

    stack->size = 0;

    stack->data = 1 + (stack_elem_t*) calloc((size_t) capacity, sizeof(stack->data[0]));

    stack->capacity = capacity;

    CNR_PRTCT ( stack->data[-1] = CANARY_BUFFER_VALUE;
                stack->data[stack->capacity - 2] = CANARY_BUFFER_VALUE; )

    if (!stack_ok(stack))
    {
        stack_dump (stack DBG(,  __FILE__, __LINE__, __FUNCTION__));
        return STACK_IS_BAD;
    }

    return STACK_IS_OK;
}

int stack_dtor (struct stack_str* stack)
{
    free(stack->data - 1);

    stack->size = 0;

    stack->capacity = 0;

    return 0;
}

int stack_push (struct stack_str* stack, stack_elem_t elem)
{
    STACK_ASSERT(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    if (stack->size >= (stack->capacity) - 2)
    {
        stack->capacity *= 2;

        stack->data = 1 + (stack_elem_t*) realloc (stack->data - 1, (size_t) (stack->capacity) * sizeof(stack->data[0]));

        CNR_PRTCT ( stack->data[-1] = CANARY_BUFFER_VALUE;
                    stack->data[stack->capacity - 2] = CANARY_BUFFER_VALUE; )

        STACK_ASSERT(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));
    }

    stack->data[stack->size++] = elem;

    STACK_ASSERT(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    return 0;
}

int stack_pop (struct stack_str* stack, stack_elem_t* x)
{
    STACK_ASSERT(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    *x = stack->data[stack->size-- - 1];

    return 0;
}

int stack_dump (struct stack_str* stack DBG(, const char* file, int line, const char* func))
{
    STACK_ASSERT(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    DBG ( printf(PURPLE_TEXT("stack_str [%p] called from %s, name \"%s\" born at %s:%d:\n"), stack, file, "stk", func, line);

    printf("       data       |   size     |  capacity  "CNR_PRTCT("|  canary_stack_1   |  canary_stack_2   |   canary_buf_1   |   canary_buf_2")"\n"
           "  %p  |    %d(%d)    |     %d      "CNR_PRTCT("|       %x         |       %x         |        %x       |       %x")"\n",
           stack->data, stack->size, stack->size + 2, stack->capacity CNR_PRTCT (,(unsigned int)stack->canary_stack_1, (unsigned int)stack->canary_stack_2,
           (unsigned int)stack->data[-1], (unsigned int)stack->data[stack->capacity - 2]) ); )

    printf(BLUE_TEXT("stack.data: "));
    for (int i = 0; i < stack->size; i++)
        printf(BLUE_TEXT("%g  "), stack->data[i]);

    printf("\n\n");

    return 0;
}

bool stack_ok (const struct stack_str* stack)
{
    if (stack == NULL)
        return false;

    if (stack->data == NULL)
        return false;

    if ((stack->size) > (stack->capacity))
        return false;

    CNR_PRTCT ( if (stack->canary_stack_1 != CANARY_VALUE)
                    return false;

                if (stack->canary_stack_2 != CANARY_VALUE)
                    return false;

                if (stack->data[-1] != CANARY_BUFFER_VALUE)
                    return false;

                if (stack->data[stack->capacity - 2] != CANARY_BUFFER_VALUE)
                    return false; )

    return true;
}

int STACK_ASSERT (struct stack_str* stack DBG(, const char* file, int line, const char* func)) // вывод - файла строки и функции
{
    if (!stack_ok(stack))
    {
        printf(RED_TEXT("STACK IS NOT OKAY!!! -- stack attack detected:\n"));

        if (stack == NULL)
            printf(YELLOW_TEXT("stack = %p\n"), stack);

        if (stack->data == NULL)
            printf(YELLOW_TEXT("stack->data = %p\n"), stack->data);

        if ((stack->size) > (stack->capacity))
            printf(YELLOW_TEXT("stack->size = %d | stack->capacity = %d\n"), stack->size, stack->capacity);

        CNR_PRTCT ( if (stack->canary_stack_1 != CANARY_VALUE || stack->canary_stack_2 != CANARY_VALUE )
                        printf(YELLOW_TEXT("stack->canary_stack_1 = %x | stack->canary_stack_2 = %x\n"),
                              (unsigned int)stack->canary_stack_1, (unsigned int)stack->canary_stack_2);

                    if (stack->data[-1] != CANARY_BUFFER_VALUE || stack->data[stack->capacity - 2] != CANARY_BUFFER_VALUE)
                        printf(YELLOW_TEXT("canary_buffer_3 = %x | canary_buffer_4 = %x\n"),
                              (unsigned int)stack->data[-1], (unsigned int)stack->data[stack->capacity - 2]); )

        assert(0);
    }

    return 0;
}

int torture_stack (struct stack_str* stack, int canary_number_check)
{
    if (canary_number_check == 0)
        return 0;

    if (canary_number_check == 1)
        memset(stack, 0, sizeof(stack));

    if (canary_number_check == 3)
        stack->data[-1] = 193276;

    if (canary_number_check == 4)
        stack->data[stack->capacity - 2] = 5725381;

    printf(LIGHT_BLUE_TEXT("stack after check canaryes:\n"));

    stack_dump(stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    return 0;
}

// TODO: dump - поэтапная распечатка, не может упасть; veryfic - не может упасть; ----> assert();
