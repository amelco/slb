#include <stdio.h>

#define SLB_HTTP_IMPLEMENTATION
#include "../slb_http.h"

int main(void) {
    Slb_config config = {0};
    slb_http_config_init(&config, "dummyjson.com", 80);
    size_t size = 0;
    unsigned char* response = slb_http_get(&config, "/products/1", &size);
    printf("Resposta:\n%s\n", response);

    return 0;
}

