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
parse_req(char** buffer, char* verb, char* path, char** headers) {
    /*
    * http request example:
    GET|POST                     // HTTP method
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
    size_t verb_len = strcspn(*buffer, " ");
    strncpy(verb, *buffer, verb_len);
    size_t path_len = strcspn(*buffer+verb_len+1, " ");
    strncpy(path, *buffer+verb_len+1, path_len);
    *buffer = *buffer + strcspn(*buffer, "\r") + 2;
    int i = 0, next_line = strcspn(*buffer, "\r");
    do {
        strncpy(headers[i++], *buffer, next_line);
        *buffer = *buffer + next_line + 2;
        next_line = strcspn(*buffer, "\r");
    } while(next_line);
    *buffer = *buffer + 2; // last line has an extra \r\n
    headers_len = i;
}

void
handle_request(int fd, char* buf) {
    char *verb = calloc(8, sizeof(char)), *path = calloc(255, sizeof(char));
    char **headers = calloc(max_headers, sizeof(char*));
    parse_req(&buf, verb, path, headers);

    int n = sizeof(routes) / sizeof(route), matched = 0, SUCCESS = 0;

    for (int i = 0; i < n; ++i) {
        char *a = path, *b = routes[i].name;
        int path_match = routes[i].rt == DYNAMIC ? strncmp(a, b, strlen(b)) : strcmp(a, b);
        int verb_match = strcmp(verb, routes[i].verb == GET ? "GET" : "POST");
        if (path_match == SUCCESS && verb_match == SUCCESS) {
            route_args arg = {headers_len, path, root_directory, buf, headers};
            routes[i].fun(fd, arg);
            matched = 1;
            break;
        }
    }
    if (!matched) {
        four04(fd);
    }
}

void
parse_args(int argc, char** argv) {
    static struct option long_options[] = {{"directory", required_argument, 0, 'd'}, {0, 0, 0, 0}};
    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, ":d:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'd': root_directory = optarg; break;
            default: printf("Usage: %s (--directory PATH)\n", argv[0]); exit(EXIT_FAILURE);
        }
    }
}
