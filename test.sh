#!/bin/bash

set -e

[ "$(./hvm_execute "123451^2v5:4?9p2g8pppppp" "")" = "945321" ]
[ "$(./hvm_execute "123451v2vppppp" "")" = "34521" ]
[ "$(./hvm_execute '0924809693937820909181938+68283390909580634**+P01^:87+*g' '')" = 'Hello, Small World!' ]
[ "$(./hvm_execute '7c8p!  9p$' '')" = '98' ]
[ ! "$(./hvm_execute '07-c8p!  9p$' '')" ]