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
static const char* user_agent_header = "User-Agent: ";
static const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\n";

void four04(int);
void root(int, ...);
void echo(int, ...);
void ua(int, ...);
void file(int, ...);

typedef enum { STATIC, DYNAMIC } route_type;

typedef struct route {
    char* name;
    void (*fun)(int, ...);
    route_type rt;
} route;

static route routes[4] = {{ECHO_P, echo, DYNAMIC}, {UA_P, ua, STATIC}, {FILE_P, file, DYNAMIC}, {ROOT_P, root, STATIC}};

#endif
