//
//  main.c
//  hvm
//
//  Created by Patrick Stevens on 04/06/2017.
//  Copyright © 2017 Patrick Stevens. All rights reserved.
//

#include <stdio.h>

#include "hvm.h"

int main(int argc, const char * argv[]) {
    int initial_mem[3] = {105,13,98};
    execute_program("10>0<<0:1+1-26*?0<1+0>055*-g0<p", initial_mem, 3);
    return 0;
}
