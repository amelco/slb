#ifndef _SLB_HTTP_H
#define _SLB_HTTP_H

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 1024*1024  // 1 MB

typedef struct {
    int socketfd;
    char baseUrl[1024];
    int port;
} Slb_config;

void           slb_http_config_init (Slb_config* config, char* baseUrl, int port);
unsigned char* slb_http_get         (Slb_config* cfg, char* endpoint, size_t* responseSize);

#endif // _SLB_HTTP_H


#ifdef SLB_HTTP_IMPLEMENTATION

void slb_http_config_init(Slb_config* config, char* baseUrl, int port)
{
    assert(strlen(baseUrl) <= 255);

    config->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(config->socketfd != -1);
    strcpy(config->baseUrl, baseUrl);
    config->port = port;
    
    struct hostent* host = gethostbyname(config->baseUrl);
    if (host == NULL) herror("gethostbyname");
    assert(host != NULL && "ERROR: invalid host");
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(config->port),
        .sin_addr = *(struct in_addr*)host->h_addr,
    };
    int res = connect(config->socketfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(res != -1);
    printf("connected to server %s\n", config->baseUrl);
}

unsigned char* slb_http_get(Slb_config* cfg, char* endpoint, size_t* responseSize)
{
    char message[BUFFER_SIZE] = {'\0'};
    sprintf(message, "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n", endpoint, cfg->baseUrl);
    printf("%s\n", message);
    int total = strlen(message);
    int sent = 0;
    while (sent < total)
    {
        int bytes = write(cfg->socketfd, message + sent, total - sent);
        assert(bytes != -1);
        if (bytes == 0) break;
        sent += bytes;
    }
    assert(sent != -1);

    unsigned char buffer[BUFFER_SIZE];
    static unsigned char response[BUFFER_SIZE*5]; // 5 MB
    int numBytes = 0;
    *responseSize = 0;
    while ((numBytes = read(cfg->socketfd, buffer, BUFFER_SIZE-1)) > 0)
    {
        printf("read %d bytes\n", numBytes);
        printf("buffer: %s\n", buffer);
        memcpy(&response[*responseSize], buffer, numBytes);
        *responseSize += numBytes;
        assert(numBytes != -1);
    }

    return response;
}



#endif // SLB_HTTP_IMPLEMENTATION
