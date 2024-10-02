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
const stack_elem_t STACK_POISON_ELEM = -666.13;

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

DBG (struct test_str
{
    char executioner_1[1];
    struct stack_str stack;
    char executioner_2[1];
};  )

enum stack_condition
{
    STACK_IS_OK,
    STACK_IS_BAD
};

enum errors
{
    STACK_OK                    = 0,
    STACK_IS_NULL               = 1,
    STACK_DATA_IS_NULL          = 2,
    STACK_BAD_SIZE              = 4,
    STACK_SIZE_IS_LESS_CAPACITY = 8,
    STACK_CANARY_PROBLEM        = 16,
    STACK_DATA_CANARY_PROBLEM   = 32
};

const int CANARY_VALUE = (0xBAD);
const int CANARY_BUFFER_VALUE = (0xEDA);

int stack_error (const struct stack_str* stack);

int error_code_output (int err);

int convert_to_binary (int n);

enum stack_condition stack_ctor (struct stack_str* stack, int capacity DBG(, const char* file, int line, const char* func));

int STACK_ASSERT (struct stack_str* stack DBG(, const char* file, int line, const char* func));

int stack_dtor (struct stack_str* stack);

int stack_push (struct stack_str* stack, stack_elem_t elem DBG(, const char* file, int line, const char* func));

int stack_pop (struct stack_str* stack, stack_elem_t* x DBG(, const char* file, int line, const char* func));

int stack_dump (struct stack_str* stack DBG(, const char* file, int line, const char* func));

bool stack_ok (const struct stack_str* stack);

int torture_stack (struct stack_str* stack, int number_canary_check DBG(, const char* file, int line, const char* func));

void printf_place_info (DBG(const char* file, int line, const char* func, const char* string));

int main (void)
{
    struct stack_str stack = {};

    if (stack_ctor(&stack, 2 DBG(,  __FILE__, __LINE__, __FUNCTION__)) == STACK_IS_BAD)
        printf("error in stack_ctor, check the data of stack, bro.\n\n");

    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    if (stack_push (&stack, 10 DBG(,  __FILE__, __LINE__, __FUNCTION__)) != 0)
        printf("error in stack_push");
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 20 DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 30 DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 40 DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 50 DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_push (&stack, 60 DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_elem_t x = 0;

    stack_pop(&stack, &x DBG(,  __FILE__, __LINE__, __FUNCTION__));
    stack_dump(&stack DBG(,  __FILE__, __LINE__, __FUNCTION__));

    torture_stack(&stack, 0 DBG(,  __FILE__, __LINE__, __FUNCTION__));

    stack_dtor(&stack);

    return 0;
}

// TODO rename stack_condicition to StackCondition
enum stack_condition stack_ctor (struct stack_str* stack, int capacity DBG(, const char* file, int line, const char* func))
{
    assert(stack);
    //STACK_ASSERT(stack);

    CNR_PRTCT ( stack->canary_stack_1 = CANARY_VALUE;
                stack->canary_stack_2 = CANARY_VALUE; )

    stack->size = 0;

    stack->data = 1 + (stack_elem_t*) calloc((size_t) capacity + 2, sizeof(stack->data[0])); // TODO disable canaries

    stack->capacity = capacity;

    CNR_PRTCT ( stack->data[-1] = CANARY_BUFFER_VALUE;
                stack->data[stack->capacity] = CANARY_BUFFER_VALUE; )

    for (int i = 0; i < stack->capacity; i++)
            stack->data[stack->size + i] = STACK_POISON_ELEM;

    if (stack_error(stack))
    {
        stack_dump (stack DBG(, file, line, func));
        return STACK_IS_BAD;
    }

    return STACK_IS_OK;
}

int stack_dtor (struct stack_str* stack)
{
    free(stack->data - 1); stack->data = NULL;

    stack->size = 0;

    stack->capacity = 0;

    return 0;
}

int stack_push (struct stack_str* stack, stack_elem_t elem DBG(, const char* file, int line, const char* func))
{
    STACK_ASSERT(stack DBG(, file, line, func));

    // TODO make func
    if (stack->size >= stack->capacity)
    {
        stack->capacity *= 2;

        stack->data = 1 + (stack_elem_t*) realloc (stack->data - 1, (size_t) (stack->capacity + 2) * sizeof(stack->data[0]));

        for (int i = 0; i < stack->size; i++)
            stack->data[stack->size + i] = STACK_POISON_ELEM;

        CNR_PRTCT ( stack->data[-1] = CANARY_BUFFER_VALUE;
                    stack->data[stack->capacity] = CANARY_BUFFER_VALUE; )

        STACK_ASSERT(stack DBG(, file, line, func));
    }

    stack->data[stack->size++] = elem;

    STACK_ASSERT(stack DBG(, file, line, func));

    return 0;
}

int stack_pop (struct stack_str* stack, stack_elem_t* x DBG(, const char* file, int line, const char* func))
{
    STACK_ASSERT(stack DBG(, file, line, func));

    *x = stack->data[stack->size - 1];
    stack->data[--stack->size] = STACK_POISON_ELEM;

    return 0;
}

int stack_dump (struct stack_str* stack DBG(, const char* file, int line, const char* func))
{
    printf(PURPLE_TEXT("stack_str [%p] called from %s, name \"%s\" born at %s:%d:\n"), stack, file, "stack", func, line);

    if (stack == NULL)
    {
        printf("stack = "BLUE_TEXT("[%p]")", this is all the information I can give\n\n", stack);
        return 0;
    }

    printf("\ncanary_1 in stack   = "BLUE_TEXT("0x%x")"\n\n" , (unsigned int)stack->canary_stack_1);
    printf(  "stack->capacity     = "BLUE_TEXT("%d")"\n",      stack->capacity);
    printf(  "stack->size         = "BLUE_TEXT("%d")"\n\n",    stack->size);
    printf(  "stack->data address = "BLUE_TEXT("[%p]")"\n\n",  stack->data);
    printf(  "canary_2 in stack   = "BLUE_TEXT("0x%x")"\n\n" , (unsigned int)stack->canary_stack_2);

    if (stack->data == NULL)
    {
        printf("stack->data = "BLUE_TEXT("[%p]")", this is all the information I can give", stack->data);
        return 0;
    }

    for (int i = 0; i < stack->capacity; i++)
    {
        if (memcmp(&stack->data[i], &STACK_POISON_ELEM, sizeof(STACK_POISON_ELEM)) == 0)
            printf("stack->data[%d] = "BLUE_TEXT("%g (POISON)")"\n", i, stack->data[i]);
        else
            printf("stack->data[%d] = "BLUE_TEXT("%g")"\n", i, stack->data[i]);
    }

    printf("\ncanary_3 in buffer = "BLUE_TEXT("0x%x")"\n\n", (unsigned int)stack->data[-1]);
    printf("canary_4 in buffer = "BLUE_TEXT("0x%x")"\n\n", (unsigned int)stack->data[stack->capacity]);

    return 0;
}

#if 0
bool stack_ok (const struct stack_str* stack)
{
    if (stack == NULL)
        return false;

    if (stack->data == NULL)
        return false;

    if (stack->size < 0)
        return false;

    if ((stack->size) > (stack->capacity))
        return false;

    CNR_PRTCT ( if (stack->canary_stack_1 != CANARY_VALUE)
                    return false;

                if (stack->canary_stack_2 != CANARY_VALUE)
                    return false;

                if (stack->data[-1] != CANARY_BUFFER_VALUE)
                    return false;

                if (stack->data[stack->capacity] != CANARY_BUFFER_VALUE)
                    return false; )

    return true;
}
#endif

int stack_error (const struct stack_str* stack)
{
    int err = 0;

    if (stack == NULL)
        err |= STACK_IS_NULL;

    if (stack->data == NULL)
        err |= STACK_DATA_IS_NULL;

    if (stack->size < 0)
        err |= STACK_BAD_SIZE;

    if ((stack->size) > (stack->capacity))
        err |= STACK_SIZE_IS_LESS_CAPACITY;

    CNR_PRTCT ( if (stack->canary_stack_1 != CANARY_VALUE || stack->canary_stack_2 != CANARY_VALUE)
                    err |= STACK_CANARY_PROBLEM;

                if (memcmp(&stack->data[-1], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)) || memcmp(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)))
                    err |= STACK_DATA_CANARY_PROBLEM; )

    return err;
}

int error_code_output (int err)
{
    printf(RED_TEXT("ERROR: "));
    convert_to_binary(err);

    if (err == STACK_IS_NULL)               printf(RED_TEXT(" - STACK_IS_NULL"));
    if (err == STACK_DATA_IS_NULL)          printf(RED_TEXT(" - STACK_DATA_IS_NULL"));
    if (err == STACK_BAD_SIZE)              printf(RED_TEXT(" - STACK_BAD_SIZE"));
    if (err == STACK_SIZE_IS_LESS_CAPACITY) printf(RED_TEXT(" - STACK_SIZE_IS_LESS_CAPACITY"));
    if (err == STACK_CANARY_PROBLEM)        printf(RED_TEXT(" - STACK_CANARY_PROBLEM"));
    if (err == STACK_DATA_CANARY_PROBLEM)   printf(RED_TEXT(" - STACK_DATA_CANARY_PROBLEM"));

    printf("\n");

    return 0;
}

int convert_to_binary (int n)
{
    if (n > 1) convert_to_binary(n / 2);

    printf(RED_TEXT("%d"), (int) n % 2);

    return 0;
}

int STACK_ASSERT (struct stack_str* stack DBG(, const char* file, int line, const char* func))
{
    if (!stack_error(stack))
        return 0;

    printf_place_info(DBG(file, line, func,) "STACK_ASSERT");

    fprintf(stderr, RED_TEXT("STACK IS NOT OKAY:\n"));

    error_code_output(stack_error(stack));

    if (stack == NULL)
        fprintf(stderr, YELLOW_TEXT("stack = NULL\n"));

    if (stack->data == NULL)
        fprintf(stderr, YELLOW_TEXT("stack->data = NULL\n"));

    if ((stack->size) > (stack->capacity))
        fprintf(stderr, YELLOW_TEXT("stack->size = %d | stack->capacity = %d\n"), stack->size, stack->capacity);

    CNR_PRTCT ( if (stack->canary_stack_1 != CANARY_VALUE || stack->canary_stack_2 != CANARY_VALUE )
                    fprintf(stderr, YELLOW_TEXT("stack->canary_stack_1 = %x | stack->canary_stack_2 = %x\n"),
                            (unsigned int)stack->canary_stack_1, (unsigned int)stack->canary_stack_2);

                if (memcmp(&stack->data[-1], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)) || memcmp(&stack->data[stack->capacity], &CANARY_BUFFER_VALUE, sizeof(CANARY_BUFFER_VALUE)))
                    fprintf(stderr, YELLOW_TEXT("canary_buffer_3 = %x | canary_buffer_4 = %x\n"),
                            (unsigned int)stack->data[-1], (unsigned int)stack->data[stack->capacity]); )

    assert(0);

    return 0;
}

void printf_place_info (DBG(const char* file, int line, const char* func, const char* string))
{
    printf(PURPLE_TEXT("called from %s, name \"%s\" born at %s:%d:\n"),  file, string, func, line);
}

int torture_stack (struct stack_str* stack, int number_canary_check DBG(, const char* file, int line, const char* func))
{
    if (number_canary_check == 0)
        return 0;

    if (number_canary_check == 1)
        memset(stack, 0, sizeof(stack));

    if (number_canary_check == 3)
        stack->data[-1] = 1933276;

    if (number_canary_check == 4)
        stack->data[stack->capacity] = 5725381;

    printf(LIGHT_BLUE_TEXT("stack after check canaries:\n"));

    STACK_ASSERT(stack DBG(, file, line, func));

    return 0;
}
