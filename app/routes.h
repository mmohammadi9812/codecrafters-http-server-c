#ifndef __ROUTES_H__
#define __ROUTES_H__

#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

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
    char* path;
    char* base_dir;
    char* buffer;
    char** headers;
} route_args;

void four04(int);
void root(int, route_args);
void echo(int, route_args);
void ua(int, route_args);
void file_get(int, route_args);
void file_post(int, route_args);

DIR* my_chdir(char*);
FILE* find_file(char* base_dir, char* name);
void read_file(FILE*, long*, char**);
void write_file(char*, int, char*);

typedef enum { STATIC, DYNAMIC } route_type;

typedef enum { GET, POST } route_verb;

typedef struct route {
    char* name;
    void (*fun)(int, route_args);
    route_type rt;
    route_verb verb;
} route;

static route routes[] = {{ECHO_P, echo, DYNAMIC, GET},
                         {UA_P, ua, STATIC, GET},
                         {FILE_P, file_get, DYNAMIC, GET},
                         {FILE_P, file_post, DYNAMIC, POST},
                         {ROOT_P, root, STATIC, GET}};

#endif
