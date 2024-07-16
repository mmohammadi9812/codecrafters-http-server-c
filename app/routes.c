#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "routes.h"

void
root(int fd) {
    char* ok_response = calloc(31, sizeof(char));
    sprintf(ok_response, "%s\r\n\r\n", ok_stat);
    send(fd, ok_response, strlen(ok_response), 0);
}

void
echo(int fd, char** path) {
    char* echo_str = *path + (int)strlen(echo_path);
    char echo_headers[255];
    sprintf(echo_headers, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(echo_str));
    char echo_response[4095];
    sprintf(echo_response, "%s%s%s", ok_stat, echo_headers, echo_str);
    send(fd, echo_response, strlen(echo_response), 0);
}

void
ua(int fd, char** path, char** headers, int headers_len) {
    char* user_agent = NULL;
    for (int i = 0; i < headers_len; ++i) {
        if (strncasecmp(headers[i], user_agent_header, strlen(user_agent_header)) == 0) {
            user_agent = headers[i] + (int)strlen(user_agent_header);
            break;
        }
    }
    char ua_len[255];
    sprintf(ua_len, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(user_agent));
    char ua_response[4095];
    sprintf(ua_response, "%s%s%s", ok_stat, ua_len, user_agent);
    send(fd, ua_response, strlen(ua_response), 0);
}
