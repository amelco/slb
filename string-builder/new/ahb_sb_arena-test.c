#define AHB_SB_USEARENA
#define AHB_SB_IMPLEMENTATION
#include "ahb_sb_arena.h"

#include <stdio.h>
#include <assert.h>

int main()
{
  Arena a = {0};
  arena_alloc(&a);
  String teste = string_new("Hello, World!", &a, arena_add);
  assert(teste.length == 13);
  assert(teste.length < teste.capacity);

  char* cstr = string_to_cstr(teste);
  assert(strcmp(cstr, teste.content) == 0);
  assert(cstr[teste.length] == '\0');

  string_append(&teste, " Ola Mundo!", &a, arena_add);
  assert(teste.length == 24);
  assert(teste.length < teste.capacity);
   
  String abc = string_new("ABC,D,EFKJH,alo,,dsa",&a, arena_add);
  StringList sl = string_split(abc, ',', &a, arena_add);
  assert(sl.size == 6);
  assert(strcmp(sl.items[0].content, "ABC") == 0);
  assert(strcmp(sl.items[1].content, "D") == 0);
  assert(strcmp(sl.items[2].content, "EFKJH") == 0);
  assert(strcmp(sl.items[3].content, "alo") == 0);
  assert(strcmp(sl.items[4].content, "") == 0);
  assert(strcmp(sl.items[5].content, "dsa") == 0);

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

  String substr = string_substring(abc, 6, 11, &a, arena_add);
  assert(strcmp(substr.content, "EFKJH") == 0);

  String substr_fail = string_substring(abc, 11, 6, &a, arena_add);
  assert(substr_fail.length == 0);

  printf("All tests completed succesfully.\n");
  arena_free(&a);
  return 0;
}
