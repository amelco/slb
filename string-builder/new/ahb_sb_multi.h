/*
 * This library builds string WITH or WITHOUT arenas
 * */
#ifndef AHB_SB_H
#define AHB_SB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// When using arena, the caller is responsable to (de)allocate it properly.
// The caller also has to inform the arena function that adds an element and return a pointer
//   to that element to be used as a callback by this library when it needs to allocate memory. 
#ifdef AHB_SB_USEARENA
  #define ARENA  // for short use, instead of AHB_SB_USEARENA inside this file
  #define AHB_ARENA_IMPLEMENTATION
  #include "ahb_arena.h"
  Arena a = {0};
  void (*cb)(Arena*, size_t);
#else
  #define HAS_ARENA false
#endif // AHB_SB_USEARENA

#define INITIAL_CAPACITY (64 * 1024)         //  64 KB
#define MAX_CAPACITY     (128 * 1024 * 1024) // 128 MB

/* -- Structs ------------------------------------------------------------ */
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

/* -- Functions ---------------------------------------------------------- */
size_t cstring_len(char* text);

String string_new(char* text
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif // ARENA
        );

#ifndef ARENA
void   string_free(String* str);
#endif // ARENA

void   string_append(String* str, char* text
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        );

size_t string_index_of(String* str, char c);
size_t string_next_index(String* str);                        // Returns the next index of the previously called string_index_of(). Calls string_index_of() if it wasn't called before.
                                                              // No more characters: returns IDX_NO_MORE_ENTRIES
String string_substring(String str, size_t ini, size_t end    // ini (inclusive), end (exclusive)
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        );

//String string_concat(String str1, ...);  TODO: concatenation function with variadic arguments (all String type)
char*  string_to_cstr(String str);

StringList string_split(String str, char separator
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        );
void       stringlist_free(StringList* str_list);

#endif //AHB_SB_H

/* -- Function Implementations ------------------------------------------- */
#ifdef AHB_SB_IMPLEMENTATION

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

String string_new(char* text
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        ) {
  size_t len = cstring_len(text);
  size_t capacity = INITIAL_CAPACITY;
  while (len > capacity) {
    capacity *= 2;
    if (capacity > MAX_CAPACITY) panic("String is too long (> 1 GB)");
  }
  
  String str = {
    .length = len,
    .capacity = capacity,
    .index = IDX_NEVER_CALLED,
    .last_searched_char = '\0',
  };
#ifdef ARENA
  str.content = cb(a, INITIAL_CAPACITY * sizeof(char));
#else
  str.content = calloc(INITIAL_CAPACITY, sizeof(char)),
#endif
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

void string_append(String* str, char* text
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        ) {
  size_t text_len = cstring_len(text);
  size_t new_len = text_len + str->length;
  bool need_realloc = new_len > str->capacity;
  while (new_len > str->capacity) {
    printf("len: %zu  > cap: %zu\n", new_len, str->capacity);
    str->capacity *= 2;
    if (str->capacity > MAX_CAPACITY) panic("String is too long");
  }

  if (need_realloc) {
#ifdef ARENA
    str->content = cb(a, str->capacity);
#else
    str->content = realloc(str->content, str->capacity);
#endif
    assert(str->content && "Buy more RAM");
  }
  memcpy(str->content + str->length, text, cstring_len(text));
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
String string_substring(String str, size_t ini, size_t end
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        ) {
  if (ini >= end || ini < 0 || end > str.length) { 
      return string_new(""
#ifdef ARENA
              , a, cb
#endif
          );
  }
  size_t size = end - ini;

  String new_string = {0};
  new_string.length = size;
  new_string.capacity = INITIAL_CAPACITY;
#ifdef ARENA
  new_string.content = cb(a, INITIAL_CAPACITY * sizeof(char));
#else
  new_string.content = calloc(INITIAL_CAPACITY, sizeof(char));
#endif
  assert(new_string.content && "Buy more RAM");

  memcpy(new_string.content, str.content + ini, size);
  new_string.content[size] = '\0';
  return new_string;
}

char* string_to_cstr(String str) {
  return str.content;
}
 
StringList string_split(String str, char separator
#ifdef ARENA
        , void* a, void* (*cb)(Arena*, size_t)
#endif
        ) {
  int num_elems = 1;
  for (int i = 0; i < str.length; ++i) {
    if (str.content[i] == separator) {
      num_elems++;    
    }
  }
  
#ifdef ARENA
  String* splits = cb(a, num_elems * sizeof(String));
#else
  String* splits = malloc(num_elems * sizeof(String));
#endif
  assert(splits && "Buy more RAM");

  int index = 0;
  int last_index = 0;
  char tmp_text[str.length];
  for (int i = 0; i < str.length; ++i) {
    if (str.content[i] == separator) {
      if (tmp_text[0] == '\0') {
          splits[index] = string_new(""
#ifdef ARENA
                  , a, cb
#endif
                  );
      }
      else {
          splits[index] = string_new(tmp_text
#ifdef ARENA
                  , a, cb
#endif
                  );
      }
      index++;
      tmp_text[0] = '\0';
      last_index = i + 1;
      continue;
    }
    tmp_text[i - last_index] = str.content[i];
    tmp_text[i - last_index + 1] = '\0';
  }

  if (cstring_len(tmp_text) > 0) {
    splits[index] = string_new(tmp_text
#ifdef ARENA
                  , a, cb
#endif
            );
  }

  StringList list = {
    .items = splits,
    .size = num_elems,
  };
  
  return list;
}
 
// void stringlist_free(StringList* str_list) {
//   for (int i = 0; i < str_list->size; ++i) {
//     free(str_list->items[i].content);
//     str_list->items[i].content = NULL;
//   }
//   free(str_list->items);
//   str_list->items = NULL;
//   str_list->size = 0;
// }

#endif //STRING_BUILDER_IMPLEMENTATION
