#include "utils.h"

void
set_nonblocking(int conn_fd) {
    int opts = fcntl(conn_fd, F_GETFL); /* Get the fd status flags and access modes */
    if (opts < 0) {
        perror("[-] fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    opts |= O_NONBLOCK;                    /* set nonblocking flag in options */
    if (fcntl(conn_fd, F_SETFL, opts) < 0) /* Set the fd status flags */
    {
        perror("[-] fcntl(F_SETFL");
        exit(EXIT_FAILURE);
    }
}

void
setup() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // setting SO_REUSEADDR ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    master_address = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(4221),
        .sin_addr = {htonl(INADDR_ANY)},
    };

    if (bind(server_fd, (struct sockaddr*)&master_address, sizeof(master_address)) != 0) {
        printf("Bind failed: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, max_clients) != 0) {
        printf("Listen failed: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a client to connect...\n");
    client_addr_len = sizeof(client_addr);
}

void
parse_req(char* buffer, char** path, char** headers) {
    /*
    * http request example:
    GET                          // HTTP method
    /index.html                  // Request target
    HTTP/1.1                     // HTTP version
    \r\n                         // CRLF that marks the end of the request line
    // Headers
    Host: localhost:4221\r\n     // Header that specifies the server's host and port
    User-Agent: curl/7.64.1\r\n  // Header that describes the client's user agent
    Accept: * / *\r\n              // Header that specifies which media types the client can accept
    \r\n                         // CRLF that marks the end of the headers
    // Request body (can be empty)
    */
    for (int i = 0; i < max_headers; ++i) {
        headers[i] = calloc(127, sizeof(char));
    }
    char *s1 = strtok(buffer, "\r\n"), *s2;
    headers[0] = strtok(NULL, "\r\n");
    int i = 1;
    do {
        s2 = strtok(NULL, "\r\n");
        if (s2 != NULL && strlen(s2) > 0) {
            strcpy(headers[i++], s2);
        }
    } while (s2 != NULL && strlen(s2) > 0);
    headers_len = i;
    strtok(s1, " ");
    *path = strtok(NULL, " ");
}

void
handle_request(int fd, char** buf) {
    char *path = calloc(255, sizeof(char)), **headers = calloc(max_headers, sizeof(char*));
    parse_req(*buf, &path, headers);

    int is_root = strcmp(path, root_path) == 0;
    int is_echo = strncmp(path, echo_path, strlen(echo_path)) == 0;
    int is_ua = strcmp(path, ua_path) == 0;

    if (is_root) {
        root(fd);
    } else if (is_echo) {
        echo(fd, &path);
    } else if (is_ua) {
        ua(fd, headers, headers_len);
    } else {
        send(fd, not_found, strlen(not_found), 0);
    }
}
