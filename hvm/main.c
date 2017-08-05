//
//  main.c
//  hvm
//
//  Created by Patrick Stevens on 04/06/2017.
//  Copyright © 2017 Patrick Stevens. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "hvm.h"

void
usage () {
    printf("%s\n", "Usage: hvm \"program\" \"comma,separated,list,of,memory\"");
}

/*
 * is_digit
 *
 * Tests whether a character is one of '0', ..., '9'.
 */
bool
is_digit (const char testing) {
    return ('0' <= testing && testing <= '9');
}


/*
 * validate_intlist
 *
 * Check that a given string is a comma-separated list of integers.
 *
 * Argument: intlist
 *    IN   - string to validate
 *
 * Argument: num_of_elts
 *    OUT  - number of elements in the list
 *
 * Return: bool true if the string passes validation; false otherwise.
 *         If returns false, num_of_elts is meaningless.
 */
bool
validate_intlist (const char *const intlist,
                  size_t *num_of_elts) {

    size_t num_memory = 0;
    size_t len = strlen(intlist);
    size_t i;
    for (i = 0; i < len; i++) {
        if (intlist[i] == ',') {
            num_memory++;
        } else if (!is_digit(intlist[i]) && intlist[i] != '-') {
            /*
             * We weren't given an integer; bail out now
             */
            return false;
        }
        else if (intlist[i] == '-') {
            /*
             * The only acceptable place to have a minus sign in a number is at the start
             */
            if (i != 0 && intlist[i-1] != ',') {
                return false;
            }
        }
    }

    *num_of_elts = num_memory;
    return true;
}

/*
 * is_integer
 *
 * Tests whether a string contains an integer represented in base 10.
 */
bool
is_integer (const char *const string) {
    const size_t len = strlen(string);
    size_t i; /* generic iterator variable */

    if (len == 0) {
        return false;
    }
    if (string[0] != '-' && !is_digit(string[0])) {
        return false;
    }
    for (i = 1; i < len; i++) {
        if (!is_digit(string[i])) {
            return false;
        }
    }

    return true;
}

int
main (int   argc,
      char *argv[]) {
    if (argc == 1 || argc > 3) {
        /*
         * Wrong number of arguments; print usage message
         */
        usage();
        return 1;
    }

    const char *program = argv[1];
    int *initial_memory = NULL;
    size_t memory_length = 0; /* number of memory integers passed in */

    if (argc >= 2 && strlen(argv[2]) > 0) {
        /*
         * We were passed in a list of memory locations; parse it
         */
        char *initial_memstr = argv[2];
        bool valid = validate_intlist(argv[2], &memory_length);
        if (!valid) {
            usage();
            printf("%s\n", "List of integers was not formatted correctly.");
            return 2;
        }
        /*
        * The above has guaranteed that we have a list of integers.
        */
        initial_memory = malloc(sizeof(int) * memory_length);
        if (initial_memory == NULL) {
            printf("%s\n", "Malloc failed.");
            return 255;
        }

        char *single_mem_int;

        single_mem_int = strtok(initial_memstr, ",");

        while (single_mem_int != NULL) {
            atoi(single_mem_int);
            single_mem_int = strtok(NULL, ",");
        }
    }

    execute_program(program,
                    initial_memory,
                    memory_length);

    free(initial_memory);
    return 0;
}
