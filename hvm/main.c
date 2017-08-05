//
//  main.c
//  hvm
//
//  Created by Patrick Stevens on 04/06/2017.
//  Copyright © 2017 Patrick Stevens. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "hvm.h"

int main(int argc, const char * argv[]) {
    /*
     * Example program from www.hacker.org/hvm, should print out 945321
     */
    execute_program("123451^2v5:4?9p2g8pppppp",
                    NULL,
                    0);
    return 0;
}
