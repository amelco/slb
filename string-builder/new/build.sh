#!/bin/bash
set -xe

mkdir -p bin/

PROG_NAME="test"
FILES="ahb_sb_arena-test.c "
LIBS="./"
INCLUDES="./"

# development version
gcc -g -Wall -Werror -std=c99 -L $LIBS  -o bin/$PROG_NAME $FILES -I $INCLUDES

# release version
#gcc -O3 -L $LIBS  -o $PROG_NAME $FILES -I $INCLUDES
