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

    send_response(fd, args.wants_gzip, echo_str);
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

    send_response(fd, args.wants_gzip, user_agent);
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

    send_response(fd, args.wants_gzip, buf);
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

void send_response(int fd, int is_gzip, char* body) {
    char *headers = calloc(255, sizeof(char));
    char *encoding = is_gzip ? "Content-Encoding: gzip\r\n" : "";
    body = is_gzip ? my_compress(body) : body;
    sprintf(headers, "%sContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", encoding, strlen(body));
    char* response = calloc(strlen(headers)+strlen(body)+31, sizeof(char));
    sprintf(response, "%s%s%s", ok_stat, headers, body);
    send(fd, response, strlen(response), 0);
}
