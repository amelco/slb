#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define INITIAL_CAPACITY 1024
#define GIGABYTE 1024 * 1024 * 1024
#define bool int
#define true 1
#define false 0

typedef struct {
  char* content;
  size_t capacity;
  size_t length;
} String;

typedef struct {
  String* elements;
  size_t length;
} StringList;

size_t cstring_len(char* text);
String string_new(char* text);
void string_free(String* str);
void string_append(String* str, char* text);
StringList string_split(String str, char separator);
void stringlist_free(StringList* str_list);

#ifdef STRING_BUILDER_IMPLEMENTATION

void panic(char* message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

size_t cstring_len(char* text) {
  size_t i = 0;
  while (text[i++] != '\0') { }
  return i - 1;
}

String string_new(char* text) {
  size_t len = cstring_len(text);
  size_t capacity = INITIAL_CAPACITY;
  while (len > capacity) {
    capacity *= 2;
    if (capacity > GIGABYTE) panic("String is too long (> 1 GB)");
  }
  
  String str = {
    .length = len,
    .capacity = capacity,
    .content = malloc(INITIAL_CAPACITY),
  };
  assert(str.content && "Buy more RAM");
  memcpy(str.content, text, cstring_len(text));
  return str;
}

void string_free(String* str) {
  str->length = 0;
  str->capacity = 0;
  free(str->content);
}

void string_append(String* str, char* text) {
  size_t text_len = cstring_len(text);
  size_t new_len = text_len + str->length;
  bool need_realloc = new_len > str->capacity;
  while (new_len > str->capacity) {
    str->capacity *= 2;
    if (str->capacity > GIGABYTE) panic("String is too long (> 1 GB)");
  }

  if (need_realloc) {
    str->content = realloc(str->content, str->capacity);
    assert(str->content && "Buy more RAM");
  }
  memcpy(str->content + str->length, text, cstring_len(text));
  str->length = new_len;
}

StringList string_split(String str, char separator) {
  int num_elems = 1;
  for (int i = 0; i < str.length; ++i) {
    if (str.content[i] == separator) {
      num_elems++;    
    }
  }
  
  String* splits = malloc(num_elems * sizeof(String));
  assert(splits && "Buy more RAM");

  int index = 0;
  int last_index = 0;
  char tmp_text[str.length];
  for (int i = 0; i < str.length; ++i) {
    if (str.content[i] == separator) {
      if (tmp_text[0] == '\0') splits[index] = string_new("");
      else splits[index] = string_new(tmp_text);
      index++;
      tmp_text[0] = '\0';
      last_index = i + 1;
      continue;
    }
    tmp_text[i - last_index] = str.content[i];
    tmp_text[i - last_index + 1] = '\0';
  }

  if (cstring_len(tmp_text) > 0) {
    splits[index] = string_new(tmp_text);
  }

  StringList list = {
    .elements = splits,
    .length = num_elems,
  };
  
  return list;
}

void stringlist_free(StringList* str_list) {
  for (int i = 0; i < str_list->length; ++i) {
    free(str_list->elements[i].content);
  }
  free(str_list->elements);
}

#endif //STRING_BUILDER_IMPLEMENTATION
#endif //STRING_BUILDER_H
