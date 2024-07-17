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

typedef struct route {
    char* name;
    void (*fun)(int, ...);
} route;

static route routes[5] = {{ECHO_P, echo}, {UA_P, ua}, {FILE_P, file}, {ROOT_P, root}};

#endif
