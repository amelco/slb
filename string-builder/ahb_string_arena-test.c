#define AHB_ARENA_IMPLEMENTATION
#include "ahb_arena.h"
#define AHB_STRING_ARENA
#define AHB_STRING_IMPLEMENTATION
#include "ahb_string.h"


#include <stdio.h>
#include <assert.h>

int main()
{
  Arena arena = {0};
  arena_alloc(&arena);

  String teste = string_new("Hello, World!", &arena);
  assert(teste.length == 13);
  assert(teste.length < teste.capacity);

  char* cstr = string_to_cstr(teste);
  assert(strcmp(cstr, teste.content) == 0);
  assert(cstr[teste.length] == '\0');

  string_append(&teste, " Ola Mundo!", &arena);
  assert(teste.length == 24);
  assert(teste.length < teste.capacity);
  
  String abc = string_new("ABC,D,EFKJH,alo,,dsa", &arena);
 
  size_t index = string_next_index(&abc);
  assert(abc.index == IDX_NEVER_CALLED);
  index = string_index_of(&abc, ',');
  assert(index == 3);
  index = string_next_index(&abc);
  assert(index == 5);
  index = string_next_index(&abc);
  assert(index == 11);
  index = string_next_index(&abc);
  assert(index == 15);
  index = string_next_index(&abc);
  assert(index == 16);
  index = string_next_index(&abc);
  assert(index == IDX_NO_MORE_ENTRIES);
  index = string_next_index(&abc);
  assert(index == IDX_NO_MORE_ENTRIES);

  String substr = string_substring(abc, 6, 11);
  assert(strcmp(substr.content, "EFKJH") == 0);

  arena_free(&arena);

  printf("All tests completed succesfully.\n");
  return 0;
}
