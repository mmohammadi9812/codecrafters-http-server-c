#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef struct sockaddr_in sockaddr_in;

void parse_req(char*, char**, char**);
void root(int);
void echo(int, char**);
void ua(int, char**, char**);
int setup(void);

const int connection_backlog = 5;
const char *ok_response = "HTTP/1.1 200 OK\r\n\r\n\r\n";
const char *ok_stat = "HTTP/1.1 200 OK\r\n";
const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
const char *echo_path = "/echo/";
const char *user_agent_header = "User-Agent: ";

int server_fd, client_addr_len;
int headers_len;
sockaddr_in serv_addr, client_addr;

int main()
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	if (setup() != 0)
		return 1;

	int fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (fd < 0)
	{
		printf("Accept failed: %s \n", strerror(errno));
		return 1;
	}
	printf("Client connected\n");

	char buf[5000];
	int recv_bytes = recv(fd, buf, sizeof(buf), 0);
	if (recv_bytes < 0)
	{
		printf("Recv failed: %s \n", strerror(errno));
		return 1;
	}

	char *path = calloc(255, sizeof(char)), **headers = calloc(4095, sizeof(char *));
	parse_req(buf, &path, headers);

	int is_root = strcmp(path, "/") == 0;
	int is_echo = strncmp(path, echo_path, strlen(echo_path)) == 0;
	int is_ua = strcmp(path, "/user-agent") == 0;

	if (is_root)
	{
		root(fd);
	}
	else if (is_echo)
	{
		echo(fd, &path);
	}
	else if (is_ua)
	{
		ua(fd, &path, headers);
	}
	else
	{
		send(fd, not_found, strlen(not_found), 0);
	}

	int status = close(server_fd);
	if (status < 0)
	{
		printf("Close failed: %s \n", strerror(errno));
		return 1;
	}

	return 0;
}

int setup()
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	serv_addr = (sockaddr_in){
		.sin_family = AF_INET,
		.sin_port = htons(4221),
		.sin_addr = {htonl(INADDR_ANY)},
	};

	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
	{
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	if (listen(server_fd, connection_backlog) != 0)
	{
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);
	return 0;
}

void parse_req(char *buffer, char **path, char **headers)
{
	char *s1 = strtok(buffer, "\r\n");
	for (int i = 0; i < 8; ++i)
		*(headers + i) = calloc(255, sizeof(char));
	*headers = strtok(NULL, "\r\n");
	char *s2 = strtok(NULL, "\r\n");
	if (s2 != NULL)
		*(headers + 1) = s2;
	int i = 2;
	while (s2 != NULL)
	{
		s2 = strtok(NULL, "\r\n");
		if (s2 != NULL)
			strcat(*(headers + (i++)), s2);
	}
	headers_len = i;
	char *verb = strtok(s1, " ");
	*path = strtok(NULL, " ");
}

void root(int fd)
{
	send(fd, ok_response, strlen(ok_response), 0);
}

void echo(int fd, char **path)
{
	char *echo_str = *path + (int)strlen(echo_path);
	char echo_headers[255];
	sprintf(echo_headers, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(echo_str));
	char echo_response[4095];
	sprintf(echo_response, "%s%s%s", ok_stat, echo_headers, echo_str);
	send(fd, echo_response, strlen(echo_response), 0);
}

void ua(int fd, char **path, char **headers)
{
	char *user_agent = NULL;
	for (int i = 0; i < headers_len; ++i)
	{
		if (strncasecmp(headers[i], user_agent_header, strlen(user_agent_header)) == 0)
		{
			user_agent = headers[i] + (int)strlen(user_agent_header);
			break;
		}
	}
	char ua_len[255];
	sprintf(ua_len, "Content-Type: text/plain\r\nContent-Length: %lu\r\n\r\n", strlen(user_agent));
	char ua_response[4095];
	sprintf(ua_response, "%s%s%s", ok_response, ua_len, user_agent);
	send(fd, ua_response, strlen(ua_response), 0);
}
