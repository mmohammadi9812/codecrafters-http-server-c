#include "routes.h"

void
four04(int fd) {
    send(fd, not_found, strlen(not_found), 0);
}

void
root(int fd, request _) {
    char* ok_response = calloc(31, sizeof(char));
    sprintf(ok_response, "%s\r\n\r\n", ok_stat);
    send(fd, ok_response, strlen(ok_response), 0);
    free(ok_response);
}

void
echo(int fd, request r) {
    char* echo_str = r.path + (int)strlen(ECHO_P);

    response arg = {fd, 0, r.wants_gzip, echo_str};
    send_response(arg);
}

void
ua(int fd, request r) {
    char* user_agent = NULL;
    for (int i = 0; i < r.headers_len; ++i) {
        if (strncasecmp(r.headers[i], user_agent_header, strlen(user_agent_header)) == 0) {
            user_agent = r.headers[i] + (int)strlen(user_agent_header);
            break;
        }
    }

    response arg = {fd, 0, r.wants_gzip, user_agent};
    send_response(arg);
}

void
file_get(int fd, request r) {
    char* fname = r.path + (int)strlen(FILE_P);
    FILE* f = find_file(r.base_dir, fname);
    if (f == NULL) {
        four04(fd);
        return;
    }
    char* buf;
    long fsize;
    read_file(f, &fsize, &buf);

    response arg = {fd, 1, r.wants_gzip, buf};
    send_response(arg);
}

void
file_post(int fd, request r) {
    char* fname = r.path + (int)strlen(FILE_P);
    int buff_len = -1;
    for (int i = 0; i < r.headers_len; ++i) {
        char* curr_head = r.headers[i];
        int is_length = strncmp(curr_head, content_length_header, strlen(content_length_header)) == 0;
        if (!is_length) {
            continue;
        }
        buff_len = atoi(r.headers[i] + strlen(content_length_header));
        break;
    }
    if (strlen(r.buffer) != buff_len) {
        four04(fd);
        return;
    }
    my_chdir(r.base_dir);
    char* file_name = r.path + strlen(FILE_P);
    write_file(file_name, buff_len, r.buffer);

    char* f_response = calloc(31, sizeof(char));
    sprintf(f_response, "%s\r\n", created_stat);
    send(fd, f_response, strlen(f_response), 0);
}

void
send_response(response resp) {
    char* headers = calloc(255, sizeof(char));
    char* encoding = resp.is_gzip ? "Content-Encoding: gzip\r\n" : "";
    char* output_buffer;
    size_t output_size = strlen(resp.body);
    if (resp.is_gzip) {
        int input_size = strlen(resp.body);
        output_buffer = calloc(2 * strlen(resp.body), sizeof(char));
        gzip_compress(resp.body, strlen(resp.body), &output_buffer, &output_size);
    }
    char* type = resp.is_octet_stream ? "application/octet-stream" : "text/plain";
    sprintf(headers, "%sContent-Type: %s\r\nContent-Length: %lu\r\n\r\n", encoding, type, output_size);
    char* response = calloc(8192, sizeof(char));
    if (resp.is_gzip) {
        sprintf(response, "%s%s", ok_stat, headers);
        send(resp.fd, response, strlen(response), 0);
        send(resp.fd, output_buffer, output_size, 0);
    } else {
        sprintf(response, "%s%s%s", ok_stat, headers, resp.body);
        send(resp.fd, response, strlen(response), 0);
    }
}
