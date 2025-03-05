#define AHB_STRING_IMPLEMENTATION
#include "ahb_string.h"

#define AHB_ARENA_IMPLEMENTATION
#include "../arena/ahb_arena.h"

#include <stdio.h>
#include <assert.h>

int main()
{
  String teste = string_new("Hello, World!");
  assert(teste.length == 13);
  assert(teste.length < teste.capacity);

  char* cstr = string_to_cstr(teste);
  assert(strcmp(cstr, teste.content) == 0);
  assert(cstr[teste.length] == '\0');

  string_append(&teste, " Ola Mundo!");
  assert(teste.length == 24);
  assert(teste.length < teste.capacity);
  
  string_free(&teste);
  assert(teste.content == NULL);
  assert(teste.length == 0);
  assert(teste.capacity == 0);
  
  String abc = string_new("ABC,D,EFKJH,alo,,dsa");
  StringList sl = string_split(abc, ',');
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

  String substr = string_substring(abc, 6, 11);
  assert(strcmp(substr.content, "EFKJH") == 0);
  string_free(&substr);

  String substr_fail = string_substring(abc, 11, 6);
  assert(substr.length == 0);
  string_free(&substr_fail);
  
  string_free(&abc);
  assert(abc.content == NULL);
  assert(abc.length == 0);
  assert(abc.capacity == 0);

  stringlist_free(&sl);
  assert(sl.items == NULL);
  assert(sl.size == 0);

  printf("All tests completed succesfully.\n");
  return 0;
}
