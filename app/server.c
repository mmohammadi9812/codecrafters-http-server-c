#include "utils.h"

int conn_list[max_clients];  /* Array of connected sockets so we know who we are talking to */
fd_set read_fds;      /* Socket file descriptors we want to wake up for, using select() */
int max_sd;     /* Highest #'d file descriptor, needed for select() */

int
main() {
    // Disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    setup();

    while (1) {
        FD_ZERO(&read_fds); /* clear the socket set */

        FD_SET(server_fd, &read_fds); /* set the master socket in the set */
        max_sd = server_fd;

        for (int i = 0; i < max_clients; ++i) { /* add child sockets to set */
            int sd = conn_list[i];
            if (sd > 0) {
                FD_SET(sd, &read_fds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        int activity = select(
            max_sd + 1, &read_fds, NULL, NULL,
            NULL); /* wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely */

        if (activity < 0 && errno != EINTR) {
            perror("[-] select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) { /* handle incoming connections */
            int fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
            if (fd < 0) {
                printf("Accept failed: %s \n", strerror(errno));
                return 1;
            }
            printf("Client connected\n");

            for (int i = 0; i < max_sd; ++i) { /* add the new socket to set */
                if (conn_list[i] == 0) {
                    conn_list[i] = fd;
                    break;
                }
            }
        }

        for (int i = 0; i < max_sd; ++i) { /* handle other sockets */
            int sd = conn_list[i];
            if (FD_ISSET(sd, &read_fds)) {
                char *buf = calloc(1025, sizeof(char));
                int recv_bytes = recv(sd, buf, 1025, 0);
                if (recv_bytes < 0) {
                    printf("Recv failed: %s \n", strerror(errno));
                    exit(EXIT_FAILURE);
                } else if (recv_bytes == 0) { /* client just got disconnected */
                    int status = close(sd);
                    if (status < 0) {
                        printf("Close failed: %s \n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                    conn_list[i] = 0;
                } else {
                    handle_request(sd, &buf);
                    free(buf);
                }
            }
        }
    }

    return 0;
}
