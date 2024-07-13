#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(4221),
									 .sin_addr = { htonl(INADDR_ANY) },
									};

	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);

	int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
	if (fd < 0) {
		printf("Accept failed: %s \n", strerror(errno));
		return 1;
	}
	printf("Client connected\n");

	char buf[5000];
	int recv_bytes = recv(fd, buf, sizeof(buf), 0);
	if (recv_bytes < 0) {
		printf("Recv failed: %s \n", strerror(errno));
		return 1;
	}

	char *request = strtok(buf, "\r\n");
	char *verb = strtok(request, " ");
	char *path = strtok(NULL, " ");

	char *ok_response = "HTTP/1.1 200 OK\r\n\r\n\r\n";
	char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
	char *echo = "/echo/";

	int is_root = strcmp(path, "/") == 0;
	int is_echo = strncmp(path, echo, strlen(echo)) == 0;
	if (is_root) {
		send(fd, ok_response, strlen(ok_response), 0);
	} else if (is_echo) {
		char *echo_str = path + (int)strlen(echo);
		char *echo_stat = "HTTP/1.1 200 OK\r\n";
		char echo_headers[1024];
		sprintf(echo_headers, "Content-Type: text/plain\r\nContent-Length: %d\r\n\r\n", strlen(echo_str));
		char echo_response[4095];
		strcat(echo_response, echo_stat);
		strcat(echo_response, echo_headers);
		strcat(echo_response, echo_str);
		send(fd, echo_response, strlen(echo_response), 0);
	} else {
		send(fd, not_found, strlen(not_found), 0);
	}

	int status = close(server_fd);
	if (status < 0) {
		printf("Close failed: %s \n", strerror(errno));
		return 1;
	}

	return 0;
}
