#!/bin/bash
set -e

mkdir -p bin/

PROG_NAME="test"
FILES="ahb_sb_arena-test.c "
LIBS="./"
INCLUDES="./"

# compile with address sanitization (asan)
gcc -g -fsanitize=address -Wall -Werror -std=c99 -L $LIBS  -o bin/$PROG_NAME $FILES -I $INCLUDES
./bin/$PROG_NAME
echo ""

# check for memory leaks
valgrind bin/$PROG_NAME
