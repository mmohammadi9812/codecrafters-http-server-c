#ifndef __ROUTES_H__
#define __ROUTES_H__

static const char *ok_stat = "HTTP/1.1 200 OK\r\n";
static const char *user_agent_header = "User-Agent: ";

static const char *root_path = "/";
static const char *echo_path = "/echo/";
static const char *ua_path = "/user-agent";

void root(int);
void echo(int, char**);
void ua(int, char**, char**, int);

#endif

