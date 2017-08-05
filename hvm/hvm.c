/*
 *  hvm.c
 *  hvm interpreter: see www.hacker.org/hvm
 *
 *  Created by Patrick Stevens on 04/06/2017.
 *  Copyright © 2017 Patrick Stevens. All rights reserved.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hvm.h"

/*
 * PRINT_BUFFER: the length of the buffer for an HVM print statement
 */
#define PRINT_BUFFER     50
#define ERR_IS_OK        0
#define ERR_PC_EOB       1
#define ERR_UNKNOWN_INST 2
/*
 * ERR_EMPTY_STACK: tried to pop from empty stack
 */
#define ERR_EMPTY_STACK  3
/*
 * ERR_STACK: tried to pop from nonexistent stack location
 */
#define ERR_STACK        4
#define ERR_DIV_0        5
/*
 * ERR_MEM: tried to access invalid memory location
 */
#define ERR_MEM          6
#define ERR_UNEXPECTED_OUTPUT 7
#define ERR_OTHER        255

#define MEMORY_SIZE 16384

/*
 * HVM interpreted program's current stack depth
 */
size_t stack_depth;

/*
 * HVM stack's currently-allocated max depth
 */
size_t curr_allocated;

/*
 * set_memory
 *
 * Copies n integers from the given array into program memory, then fills the
 * remaining memory with zeros. Only copies the first MEMORY_SIZE entries.
 *
 * Argument: initialise
 *     IN  - array whose values will be the program's memory
 *
 * Argument: memory
 *     IN  - pointer to the program's memory
 *
 * Argument: n
 *     IN  - number of values to copy from `initialise` into the program
 */
void
set_memory (const int    *const initialise,
                  int    *const memory,
            size_t              n)
{
    size_t i;

    if (n > MEMORY_SIZE) {
        n = MEMORY_SIZE;
    }
    memcpy(memory, initialise, n * sizeof(int));

    for (i = n; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

/*
 * pop
 *
 * Removes the top element of the input stack, and returns it in an out arg.
 *
 * Argument: output
 *     OUT   - integer which was at the top of the stack
 *
 * Argument: stack
 *     INOUT - stack to pop from.
 *
 * Returns: integer return code
 */
int
pop (int *const output,
     int *const stack)
{
    int rc = ERR_IS_OK;

    if (stack_depth == 0) {
        rc = ERR_EMPTY_STACK;
    }

    if (rc == ERR_IS_OK) {
        *output = stack[stack_depth - 1];
        stack_depth -= 1;
    }

    return (rc);
}

/*
 * delete
 *
 * Deletes the `index`th element of arr, and shifts all further elements left.
 *
 * Argument: arr
 *     INOUT - array from which to delete an element
 *
 * Argument: index
 *     IN    - index which will be deleted
 *
 * Argument: len
 *     INOUT - length of arr
 */
void
delete (int    *const arr,
        size_t        index,
        size_t *const len)
{
    size_t i;

    for (i = index; i < *len; i++) {
        arr[i] = arr[i+1];
    }

    *len -= 1;
}

/*
 * push
 *
 * Pushes the given value to a stack.
 *
 * Argument: val
 *     IN    - value to push to the stack
 *
 * Argument: stack
 *     INOUT - stack to push to.
 */
int
push (int   val,
      int **stack)
{
    int rc = ERR_IS_OK;

    if (stack_depth >= curr_allocated) {
        /*
         * More storage required for the stack, since it's full at the moment
         */
        *stack = realloc(*stack, 2*curr_allocated);
        if (*stack == NULL) {
            rc = ERR_OTHER;
        }

        if (rc == ERR_IS_OK) {
            curr_allocated *= 2;
        }
    }

    if (rc == ERR_IS_OK) {
        (*stack)[stack_depth] = val;
        stack_depth++;

    }

    return (rc);
}

/*
 * Returns true iff the specified int is <= the size_t.
 */
bool
int_leq_sizet (int a,
               size_t b)
{
    bool result;

    if (a < 0 || (size_t)a <= b) {
        result = true;
    } else {
        result = false;
    }

    return (result);
}

/*
 * Returns true iff the specified int is >= the size_t.
 */
bool
int_geq_sizet (int a,
               size_t b)
{
    bool result;

    if (a < 0) {
        result = false;
    } else if ((size_t)(a) >= b) {
        result = true;
    }

    return (result);
}

/*
 * execute_program
 *
 * Executes a HVM program.
 *
 * Argument: program
 *     IN    - string program to execute
 *
 * Argument: initial_mem
 *     IN    - the HVM instance's memory. Only the first MEMORY_SIZE ints will
 *             be considered; the rest will never be encountered by the HVM.
 *             This is copied to an internal array. If there are not
 *             MEMORY_SIZE entries in initial_mem, the internal memory will be
 *             padded with zeros.
 *
 * Argument: mem_len
 *     IN    - length of the initial memory array
 */
int
execute_program (const char *const program,
                 const int  *const initial_mem,
                 const int         mem_len)
{
    int     program_counter = 0;
    size_t  end             = strlen(program);
    bool    done            = false;
    int     rc              = 0;
    int     s0;   /* variable into which we pop from stack */
    int     s1;   /* variable into which we pop from stack */
    int     val_to_push;
    int    *stack;
    int     memory[MEMORY_SIZE]; /* HVM interpreted program's memory buffer */

    /*
     * A stack of depth 32 is a reasonable start.
     */
    curr_allocated = 32;
    stack = malloc(curr_allocated * sizeof(int));
    if (stack == NULL) {
        rc = ERR_OTHER;
    }

    set_memory(initial_mem, memory, mem_len);

    /*
     * The result of program_counter < end
     */
    for (program_counter = 0;
         int_leq_sizet(program_counter, end) && !done && rc == ERR_IS_OK;
         program_counter++)
    {
        if (program_counter < 0) {
            /*
             * Counter is out of bounds
             */
            rc = ERR_PC_EOB;
        }

        if (rc == ERR_IS_OK) {
            switch (program[program_counter]) {
                case ' ':
                    break;
                case 'p':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        printf("%i", s0);
                    }
                    break;
                case 'P':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        printf("%c", s0 % 256);
                    }
                    break;
                case '0':
                    rc = push(0, &stack);
                    break;
                case '1':
                    rc = push(1, &stack);
                    break;
                case '2':
                    rc = push(2, &stack);
                    break;
                case '3':
                    rc = push(3, &stack);
                    break;
                case '4':
                    rc = push(4, &stack);
                    break;
                case '5':
                    rc = push(5, &stack);
                    break;
                case '6':
                    rc = push(6, &stack);
                    break;
                case '7':
                    rc = push(7, &stack);
                    break;
                case '8':
                    rc = push(8, &stack);
                    break;
                case '9':
                    rc = push(9, &stack);
                    break;
                case '+':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        rc = push(s1 + s0, &stack);
                    }
                    break;
                case '-':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        rc = push(s1 - s0, &stack);
                    }
                    break;
                case '*':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        rc = push(s1 * s0, &stack);
                    }
                    break;
                case '/':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        if (s0 == 0) {
                            rc = ERR_DIV_0;
                        }
                    }
                    if (rc == ERR_IS_OK) {
                        rc = push(s1 / s0, &stack);
                    }
                    break;
                case ':':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        if (s1 < s0) {
                            val_to_push = -1;
                        } else if (s1 == s0) {
                            val_to_push = 0;
                        } else {
                            val_to_push = 1;
                        }
                        rc = push(val_to_push, &stack);
                    }
                    break;
                case 'g':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        program_counter += s0;
                    }
                    break;
                case '?':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        if (s1 == 0) {
                            program_counter += s0;
                        }
                    }
                    break;
                case 'c':
                    break;
                case '$':
                    break;
                case '<':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        if (s0 < 0 || s0 >= MEMORY_SIZE) {
                            rc = ERR_MEM;
                        }
                    }
                    if (rc == ERR_IS_OK) {
                        rc = push(memory[s0], &stack);
                    }
                    break;
                case '>':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        rc = pop(&s1, stack);
                    }
                    if (rc == ERR_IS_OK) {
                        if (s0 < 0 || s0 >= MEMORY_SIZE) {
                            rc = ERR_MEM;
                        }
                    }
                    if (rc == ERR_IS_OK) {
                        memory[s0] = s1;
                    }
                    break;
                case '^':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        if (s0 < 0 || int_geq_sizet(s0, stack_depth)) {
                            rc = ERR_STACK;
                        }
                    }
                    if (rc == ERR_IS_OK) {
                        val_to_push = stack[stack_depth - s0 - 1];
                        rc = push(val_to_push, &stack);
                    }
                    break;
                case 'v':
                    rc = pop(&s0, stack);
                    if (rc == ERR_IS_OK) {
                        if (s0 < 0 || int_geq_sizet(s0, stack_depth)) {
                            rc = ERR_STACK;
                        }
                    }
                    if (rc == ERR_IS_OK) {
                        val_to_push = stack[stack_depth - s0 - 1];
                        delete(stack, stack_depth - s0 - 1, &stack_depth);
                        rc = push(val_to_push, &stack);
                    }
                    break;
                case 'd':
                    rc = pop(&s0, stack);
                    break;
                case '!':
                    done = true;
                    break;
                default:
                    /*
                     * Unknown instruction
                     */
                    rc = ERR_UNKNOWN_INST;
                    break;
            }
        }
    }

    free(stack);

    return (rc);
}
