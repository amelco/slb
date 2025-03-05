/**************************************************************************
Single head library that implements String type and some string functions.
Author: Andre Bezerra (2025)
License: do whatever you want with this. It's YOUR responsability :).

This library follows the Sean Barret's libraries.
You have to define its implementation to use it:
#define AHB_STRING_IMPLEMENTATION

If you want to use memory arenas:
#define AHB_STRING_ARENA
Using an arena has the advantage of not worring to call string_free() as
all newly created strings will be deallocated when you free your arena.

All definitions must come before #include "ahb_string.h"
***************************************************************************/


#ifndef AHB_STRING_H
#define AHB_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 8
#define GIGABYTE (1024 * 1024 * 1024)

#ifdef AHB_STRING_ARENA
#include "ahb_arena.h"
#endif

typedef struct {
  char* content;
  size_t capacity;
  size_t length;
  size_t index;     // Used in string_index_of() and string_next_index()
                    // If >= 0, it's the index 
                    // If   -1, IDX_NEVER_CALLED
                    // If   -2, IDX_NO_MORE_ENTRIES
  char last_searched_char;
} String;

typedef struct {
  String* items;
  size_t size;
} StringList;

/*********************
  cstring functions
**********************/
size_t cstring_len(char* text);

/*********************
  String functions
**********************/
#ifdef AHB_STRING_ARENA
String string_new(char* text, void* arena);
void   string_append(String* str, char* text, void* arena);
#else
String string_new(char* text);
void   string_append(String* str, char* text);
void   string_free(String* str);
#endif

void   string_printf(const char* format, ...);
size_t string_index_of(String* str, char c);
size_t string_next_index(String* str);                        // Returns the next index of the previously called string_index_of(). Calls string_index_of() if it wasn't called before.
                                                              // No more characters: returns IDX_NO_MORE_ENTRIES
String string_substring(String str, size_t ini, size_t end);  // ini (inclusive), end (exclusive)
//String string_concat(String str1, ...);  TODO: concatenation function with variadic arguments (all String type)
char*  string_to_cstr(String str);


/**************************************************************************/
#ifdef AHB_STRING_IMPLEMENTATION

#define IDX_NEVER_CALLED (-1l)
#define IDX_NO_MORE_ENTRIES (-2l)

void panic(char* message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

size_t cstring_len(char* text) {
  size_t i = 0;
  while (text[i++] != '\0') { }
  return i - 1;
}

#ifdef AHB_STRING_ARENA
String string_new(char* text, void* a) {
  Arena* arena = (Arena*)a;
#else
String string_new(char* text) {
#endif
  size_t len = cstring_len(text);
  size_t capacity = INITIAL_CAPACITY;
  while (len > capacity) {
    capacity *= 2;
    if (capacity > GIGABYTE) panic("String is too long (> 1 GB)");
  }
  
  String str = {
    .length = len,
    .capacity = capacity,
#ifdef AHB_STRING_ARENA
    .content = arena_add((Arena*)arena, INITIAL_CAPACITY * sizeof(char)),
#else    
    .content = calloc(INITIAL_CAPACITY, sizeof(char)),
#endif    
    .index = IDX_NEVER_CALLED,
    .last_searched_char = '\0',
  };
  assert(str.content && "Buy more RAM");
  memcpy(str.content, text, cstring_len(text));
  return str;
}

void string_free(String* str) {
  str->length = 0;
  str->capacity = 0;
  str->index = IDX_NEVER_CALLED;
  free(str->content);
  str->content = NULL;
}

#ifdef AHB_STRING_ARENA
void string_append(String* str, char* text, void* a) {
  Arena* arena = (Arena*)a;
  size_t old_length = str->length;
#else
void string_append(String* str, char* text) {
#endif
  size_t text_len = cstring_len(text);
  size_t new_len = text_len + str->length;
  bool need_realloc = new_len > str->capacity;
  while (new_len > str->capacity) {
    // printf("len: %zu  > cap: %zu\n", new_len, str->capacity);
    str->capacity *= 2;
    if (str->capacity > GIGABYTE) panic("String is too long (> 1 GB)");
  }

  if (need_realloc) {
#ifdef AHB_STRING_ARENA
    void* old_text = str->content;
    str->content = arena_add(arena, str->capacity);
    assert(str->capacity < arena->size && "Arena overflow. Maybe you forgot to free it.");
    memcpy(str->content, old_text, old_length);
    }
  memcpy(str->content + old_length, text, cstring_len(text));
#else
    str->content = realloc(str->content, str->capacity);
    assert(str->content && "Buy more RAM");
  }
  memcpy(str->content + str->length, text, cstring_len(text));
#endif
  str->length = new_len;
}

size_t __private_string_index_of(String str, size_t start, char c) {
  for (size_t i = start; i < str.length; ++i) {
    if (str.content[i] == c)
      return i;
  }
  return IDX_NO_MORE_ENTRIES;
}

// must be called first when searching for the same char several times
size_t string_index_of(String* str, char c) {
  str->index = IDX_NEVER_CALLED;
  str->last_searched_char = c;
  str->index = __private_string_index_of(*str, 0, c);
  return str->index;
}

// must be called after calling string_index_of when wanted to look for other occurrencies of the same char in the same string. If search function never called before, call it.
size_t string_next_index(String* str) {
  if (str->index == IDX_NEVER_CALLED)
    return IDX_NEVER_CALLED;
  str->index = __private_string_index_of(*str, str->index + 1, str->last_searched_char);
  return str->index;
}

// returns a substring given a initial (inclusive) and end (exclusive) indices.
// returns NULL if fails
// must be free'd
// TODO: it is not memory sanitized when compiling with memory sanitization. Fix it!
String string_substring(String str, size_t ini, size_t end) {
  if (ini >= end || ini < 0 || end > str.length) 
#ifdef AHB_STRING_ARENA    
    return string_new("", NULL);
#else
    return string_new("");
#endif
  size_t size = end - ini;

  String new_string = {0};
  new_string.length = size;
  new_string.capacity = INITIAL_CAPACITY;
  new_string.content = calloc(INITIAL_CAPACITY, sizeof(char));
  assert(new_string.content && "Buy more RAM");

  memcpy(new_string.content, str.content + ini, size);
  new_string.content[size] = '\0';
  return new_string;
}

char* string_to_cstr(String str) {
  return str.content;
}

void string_printf(const char* format, ...) {
  va_list args;
  va_start(args, format);

  while(*format) { // while format is not NULL
    if(*format == '%') {
      format++;
      switch(*format) {
      case 'd':
        // int value = va_arg(args, int);
        printf("%d", va_arg(args, int));
        break;
      case 'f':
        // float value = va_arg(args, float);
        printf("%f", va_arg(args, double));
        break;
      case 's':
        // char* value = va_arg(args, char*);
        printf("%s", va_arg(args, char*));
        break;
      case 'S':
        printf("%s", va_arg(args, String).content);
        break;
      default:
        putchar('%');
        putchar(*format);
        break;
      }
    }
    else {
      putchar(*format);
    }
    format++;
  }

  va_end(args);
}

#endif // AHB_STRING_IMPLEMENTATION
#endif // AHB_STRING_H