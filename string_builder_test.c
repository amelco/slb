#define STRING_BUILDER_IMPLEMENTATION
#include "string_builder.h"

#include <stdio.h>

int main()
{
  String teste = string_new("Hello, World!");
  printf("content:  %s\n", teste.content);
  printf("capacity: %zu\n", teste.capacity);
  printf("length:   %zu\n", teste.length);

  string_append(&teste, " Ola Mundo!");
  printf("content:  %s\n", teste.content);
  printf("capacity: %zu\n", teste.capacity);
  printf("length:   %zu\n", teste.length);
  
  string_free(&teste);
  printf("content:  %s\n", teste.content == NULL ? "" : teste.content);
  printf("capacity: %zu\n", teste.capacity);
  printf("length:   %zu\n", teste.length);
  
  String abc = string_new("ABC,D,EFKJH,alo,,dsa");
  StringList sl = string_split(abc, ',');
  for (size_t i=0; i < sl.length; ++i) {
    printf("\ncontent:  %s\n",  sl.elements[i].content);
    printf("length:   %zu\n", sl.elements[i].length);
  }

  string_free(&abc);
  stringlist_free(&sl);

  return 0;
}
