#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
	void* bytes;
	size_t size;
	size_t index;
} Arena;

#define ARENA_SIZE 1024 // 1kb

void arena_alloc(Arena* a) {
	a->size = ARENA_SIZE;
	a->bytes = malloc(a->size);
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



typedef struct {
	int x;
	int y;
} Vec2;

int main() 
{
	Arena arena = {0};
	arena_alloc(&arena);

	Vec2 v1 = {2, 10};
	Vec2 v2 = {22, 12};
	Vec2 v3 = {0, -1};
	Vec2* vels = arena_add(&arena, 2*sizeof(Vec2));
	printf("%p\n", vels);

	Vec2 p1 = {100, 233};
	Vec2* pos = arena_add(&arena, 2*sizeof(Vec2));
	memcpy(pos, &p1, sizeof(p1));
	printf("p1.x: %d\np1.y: %d\n", pos[0].x, pos[0].y);
	
	memcpy(vels, &v1, sizeof(v1));
	memcpy(vels + 1, &v2, sizeof(v2));
	memcpy(vels + 2, &v3, sizeof(v3));
	printf("v1.x: %d\nv1.y: %d\n", vels[0].x, vels[0].y);
	printf("v2.x: %d\nv2.y: %d\n", vels[1].x, vels[1].y);
	printf("v3.x: %d\nv3.y: %d\n", vels[2].x, vels[2].y);
	printf("p1.x: %d\np1.y: %d\n", pos[0].x, pos[0].y);

	arena_free(&arena);
	return 0;
}