#ifndef __UTILS_H__
#define __UTILS_H__
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#include "routes.h"

#define PORT 4221

static const int max_clients = 5;
static const int max_headers = 16;
static const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";

int server_fd;       /* The socket file descriptor for our "listening" socket */

int client_addr_len;
int headers_len; /* #' of headers in the request */
struct sockaddr_in master_address, client_addr;


void handle_request(int,char**);
void parse_req(char*, char**, char**);
void set_nonblocking(int);
void setup(void);

#endif
