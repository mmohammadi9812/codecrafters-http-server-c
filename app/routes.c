#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "routes.h"

void
four04(int fd) {
    send(fd, not_found, strlen(not_found), 0);
}

void
root(int fd, ...) {
    char* ok_response = calloc(31, sizeof(char));
    sprintf(ok_response, "%s\r\n\r\n", ok_stat);
    send(fd, ok_response, strlen(ok_response), 0);
}

void
echo(int fd, ...) {
    va_list argp;
    va_start(argp, fd);
    char* path = va_arg(argp, char*);
    va_end(argp);

    char* echo_str = path + (int)strlen(ECHO_P);
    char* echo_headers = calloc(255, sizeof(char));
    sprintf(echo_headers, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(echo_str));
    char* echo_response = calloc(1025, sizeof(char));
    sprintf(echo_response, "%s%s%s", ok_stat, echo_headers, echo_str);
    send(fd, echo_response, strlen(echo_response), 0);
}

void
ua(int fd, ...) {
    va_list argp;
    va_start(argp, fd);
    char* path = va_arg(argp, char*);
    char** headers = va_arg(argp, char**);
    int headers_len = va_arg(argp, int);
    va_end(argp);

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

void
file(int fd, ...) {
    va_list argp;
    va_start(argp, fd);
    char* path = va_arg(argp, char*);
    va_arg(argp, char**);
    va_arg(argp, int);
    char* directory = va_arg(argp, char*);
    va_end(argp);

    char* fname = path + (int)strlen(FILE_P);
    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;
    if ((dp = opendir(directory)) == NULL) {
        printf("Failed to open %s: %s\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    chdir(directory);
    int found = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (strncmp(entry->d_name, fname, strlen(fname)) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        four04(fd);
        return;
    }
    FILE* f = fopen(entry->d_name, "r");
    if (f == NULL) {
        printf("Requested file could not be opened: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = calloc(fsize + 1, sizeof(char));
    fread(buf, fsize, 1, f);
    fclose(f);

    char f_len_header[255];
    sprintf(f_len_header, "Content-Type: application/octet-stream\r\nContent-Length: %lu\r\n\r\n", fsize);
    char* f_response = calloc(fsize + 255, sizeof(char));
    sprintf(f_response, "%s%s%s", ok_stat, f_len_header, buf);
    send(fd, f_response, strlen(f_response), 0);
}
