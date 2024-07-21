#ifndef __ROUTES_H__
#define __ROUTES_H__
#include <string.h>
#include <sys/socket.h>
#include "ioops.h"

#define ROOT_P "/"
#define ECHO_P "/echo/"
#define UA_P   "/user-agent"
#define FILE_P "/files/"

static const char* ok_stat = "HTTP/1.1 200 OK\r\n";
static const char* created_stat = "HTTP/1.1 201 Created\r\n";
static const char* user_agent_header = "User-Agent: ";
static const char* content_length_header = "Content-Length: ";
static const char* content_type_header = "Content-Type: ";
static const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\n";

typedef struct {
    int headers_len;
    int wants_gzip;
    char* path;
    char* base_dir;
    char* buffer;
    char** headers;
} request;

typedef struct {
    int fd;
    int is_octet_stream;
    int is_gzip;
    char* body;
} response;

void send_response(response);

void four04(int);
void root(int, request);
void echo(int, request);
void ua(int, request);
void file_get(int, request);
void file_post(int, request);

typedef enum { STATIC, DYNAMIC } route_type;

typedef enum { GET, POST } route_verb;

typedef struct route {
    char* name;
    void (*fun)(int, request);
    route_type rt;
    route_verb verb;
} route;

static route routes[] = {{ECHO_P, echo, DYNAMIC, GET},
                         {UA_P, ua, STATIC, GET},
                         {FILE_P, file_get, DYNAMIC, GET},
                         {FILE_P, file_post, DYNAMIC, POST},
                         {ROOT_P, root, STATIC, GET}};

#endif
