#ifndef _SLB_DYN_ARRAY
#define _SLB_DYN_ARRAY

typedef char da_type;
typedef struct {
    da_type *content;
    int count;
    int capacity;
} da;

void da_init(da *a);
void da_free(da *a);
void da_push(da *a, da_type value);


#ifdef SLB_DYN_ARRAY_IMPLEMENTATION

void da_init(da *a) {
    a->content = malloc(sizeof(da_type) * 256);
    a->count = 0;
    a->capacity = 0;
}

void da_push(da *a, da_type value) {
    if (a->count == a->capacity) {
        a->capacity = a->capacity == 0 ? 1 : a->capacity * 2;
        a->content = realloc(a->content, sizeof(da_type) * a->capacity);
    }
    a->content[a->count++] = value;
}

void da_free(da *a) {
    free(a->content);
}


#endif //SLB_DYN_ARRAY_IMPLEMENTATION
#endif //_SLB_DYN_ARRAY
