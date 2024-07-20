#ifndef __UTILS_H__
#define __UTILS_H__
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "routes.h"

static const int max_clients = 5;
static const int max_headers = 16;
static const char* accept_encoding = "Accept-Encoding: ";
static const char* gzip = "gzip";

int server_fd; /* The socket file descriptor for our "listening" socket */

int client_addr_len;
int headers_len; /* #' of headers in the request */
struct sockaddr_in master_address, client_addr;
char* root_directory;

void handle_request(int, char*);
void parse_args(int, char**);
void parse_req(char**, char*, char*, char**);
void set_nonblocking(int);
void setup(void);
int wants_gzip(int,char**);

#endif
