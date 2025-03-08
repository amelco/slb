#ifndef AHB_ARENA
#define AHB_ARENA
#include <stdlib.h>
#include <assert.h>

#define ARENA_SIZE 10*1024*1024 // 10 MB

typedef struct {
	void* bytes;
	size_t size;
	size_t index;
} Arena;

void  arena_alloc(Arena* a);
void  arena_alloc_size(Arena* a, size_t size_in_bytes);
void* arena_add(Arena* a, size_t size);
void  arena_free(Arena* a);



// -----------------------------------------------------------
#ifdef AHB_ARENA_IMPLEMENTATION

void arena_alloc(Arena* a) {
	a->size = ARENA_SIZE;
	a->bytes = calloc(a->size, sizeof(char*));
	assert(a->bytes && "Buy more RAM.");
	a->index = 0;
}

void  arena_alloc_size(Arena* a, size_t size_in_bytes) {
	a->size = size_in_bytes;
	a->bytes = calloc(a->size, sizeof(char*));
	assert(a->bytes && "Buy more RAM.");
	a->index = 0;
}

void* arena_add(Arena* a, size_t size) {
	assert(size + a->index < ARENA_SIZE && "Arena overflow.");
	void* p = a->bytes + a->index;
	a->index += size;
	return p;
}

void arena_free(Arena* a) {
	free(a->bytes);
	a->index = 0;
	a->size = 0;
}

#endif // AHB_ARENA_IMPLEMENTATION
#endif // AHB_ARENA
