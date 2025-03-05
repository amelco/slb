#ifndef TMP_H
#define TMP_H

#include <stdio.h>
#include <stdbool.h>

#ifdef ARENA
  #define AHB_ARENA_IMPLEMENTATION
  #include "ahb_arena.h"
  Arena a = {0};
  #define HAS_ARENA (a.bytes != NULL)
#else
  #define HAS_ARENA false
#endif //ARENA

void tmp_init();
void tmp_print();


// -----------
void tmp_init() {
#ifdef ARENA
    arena_alloc(&a);
#endif
}

void tmp_print() {
    if (HAS_ARENA) printf("Init called with arena");
    else printf("Init called");
}

#endif // TMP_H
