//
//  hvm.h
//  hvm
//
//  Created by Patrick Stevens on 04/06/2017.
//  Copyright © 2017 Patrick Stevens. All rights reserved.
//

#ifndef hvm_h
#define hvm_h

#include <stdio.h>

int
execute_program (char *program,
                 int  *initial_mem,
                 int   mem_len);

#endif /* hvm_h */
