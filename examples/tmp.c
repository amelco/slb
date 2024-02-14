#include <stdio.h>

#define SLB_STRING_IMPLEMENTATION
#include "slb_string.h"

int main(int argc, char** argv)
{
    Slb_string *s = slb_read_entire_file("tmp.c");
    if (!s) {
        printf("Failed to read file\n");
        return 1;
    }
    printf("Capacity: %zu, Size: %zu\nContent: %s\n\n", s->capacity, s->size, s->content);

    slb_string_close(s);
    return 0;

    char *a = "hello ";
    char *b = "world";
    Slb_string as = slb_string_create_from_cstr(a);
    Slb_string bs = slb_string_create_from_cstr(b);

    printf("Capacity: %zu, Size: %zu\nContent: %s\n\n", as.capacity, as.size, as.content);
    printf("Capacity: %zu, Size: %zu\nContent: %s\n\n", bs.capacity, bs.size, bs.content);

    slb_string_close(&bs);
    return 0;

    slb_string_append(&as, &bs);

    return 0;
}
