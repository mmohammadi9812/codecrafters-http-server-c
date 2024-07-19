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
root(int fd, route_args _) {
    char* ok_response = calloc(31, sizeof(char));
    sprintf(ok_response, "%s\r\n\r\n", ok_stat);
    send(fd, ok_response, strlen(ok_response), 0);
    free(ok_response);
}

void
echo(int fd, route_args args) {
    char* echo_str = args.path + (int)strlen(ECHO_P);
    char* echo_headers = calloc(255, sizeof(char));
    sprintf(echo_headers, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(echo_str));
    char* echo_response = calloc(1025, sizeof(char));
    sprintf(echo_response, "%s%s%s", ok_stat, echo_headers, echo_str);
    send(fd, echo_response, strlen(echo_response), 0);
    free(echo_headers);
    free(echo_response);
}

void
ua(int fd, route_args args) {
    char* user_agent = NULL;
    for (int i = 0; i < args.headers_len; ++i) {
        if (strncasecmp(args.headers[i], user_agent_header, strlen(user_agent_header)) == 0) {
            user_agent = args.headers[i] + (int)strlen(user_agent_header);
            break;
        }
    }
    char ua_len[255];
    sprintf(ua_len, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(user_agent));
    char ua_response[4095];
    sprintf(ua_response, "%s%s%s", ok_stat, ua_len, user_agent);
    send(fd, ua_response, strlen(ua_response), 0);
}

DIR* my_chdir(char* directory) {
    DIR* dp;
    if ((dp = opendir(directory)) == NULL) {
        printf("Failed to open %s: %s\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    chdir(directory);
    return dp;
}

FILE* find_file(char* base_dir, char* name) {
    struct dirent* entry;
    DIR* dp = my_chdir(base_dir);
    int found = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (strncmp(entry->d_name, name, strlen(name)) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return NULL;
    }
    return fopen(entry->d_name, "r");
}

void read_file(FILE* f, long* size, char** buf) {
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *buf = calloc(*size + 1, sizeof(char));
    fread(*buf, sizeof(char), *size, f);
    fclose(f);
}

void write_file(char* file_name, int size, char *buffer) {
    FILE* f = fopen(file_name, "w");
    if (f == NULL) {
        printf("Failed to open file %s for writing\n", file_name);
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), size, f);
    fclose(f);
}

void
file_get(int fd, route_args args) {
    char* fname = args.path + (int)strlen(FILE_P);
    FILE *f = find_file(args.base_dir, fname);
    if (f == NULL) {
        four04(fd);
        return;
    }
    char *buf;
    long fsize;
    read_file(f, &fsize, &buf);

    char f_len_header[255];
    sprintf(f_len_header, "Content-Type: application/octet-stream\r\nContent-Length: %lu\r\n\r\n", fsize);
    char* f_response = calloc(fsize + 255, sizeof(char));
    sprintf(f_response, "%s%s%s", ok_stat, f_len_header, buf);
    send(fd, f_response, strlen(f_response), 0);
    free(f_response);
}

void file_post(int fd, route_args args) {
    char* fname = args.path + (int)strlen(FILE_P);
    int buff_len = -1;
    for (int i = 0; i < args.headers_len; ++i) {
        char *curr_head = args.headers[i];
        int is_length = strncmp(curr_head, content_length_header, strlen(content_length_header)) == 0;
        if (!is_length)
            continue;
        buff_len = atoi(args.headers[i] + strlen(content_length_header));
        break;
    }
    if (strlen(args.buffer) != buff_len) {
        four04(fd);
        return;
    }
    my_chdir(args.base_dir);
    char *file_name = args.path + strlen(FILE_P);
    write_file(file_name, buff_len, args.buffer);

    char* f_response = calloc(31, sizeof(char));
    sprintf(f_response, "%s\r\n", created_stat);
    send(fd, f_response, strlen(f_response), 0);
}
