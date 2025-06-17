#include "server.h"

#include <stdio.h>      // printf, perror, etc.
#include <stdlib.h>     // exit, malloc, etc.
#include <string.h>     // memset, memcpy, etc.
#include <unistd.h>     // close, read, write, etc.
#include <stdbool.h>    // bool, true, false
#include <sys/types.h>  // data types used in system calls
#include <sys/socket.h> // socket(), bind(), listen(), accept()
#include <netinet/in.h> // struct sockaddr_in, htons(), htonl()
#include <arpa/inet.h>  // inet_ntoa(), inet_addr(), etc.
#include <errno.h>      // errno and strerror()
#include <fcntl.h>
#include <poll.h>

static void fd_set_nonblocking(int fd)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
    {
        perror("fcntl");
        close(fd);
        exit(1);
    }
}

void init_conn(Conn *conn, int fd, bool read_val, bool write_val, bool close_val)
{
    conn->fd = fd;
    conn->read = read_val;
    conn->write = write_val;
    conn->close = close_val;

    conn->in_buffer = malloc(BUFFER_SIZE);
    if (conn->in_buffer == NULL)
    {
        perror("malloc");
        close(fd);
        exit(1);
    }

    conn->out_buffer = malloc(BUFFER_SIZE);
    if (conn->out_buffer == NULL)
    {
        perror("malloc");
        free(conn->in_buffer);
        close(fd);
        exit(1);
    }
}

void free_conn(Conn *conn)
{
    if (conn->in_buffer)
    {
        free(conn->in_buffer);
    }
    if (conn->out_buffer)
    {
        free(conn->out_buffer);
    }
    close(conn->fd);
}

int server_setup()
{
    int socket_fd;
    struct sockaddr_in server_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    int val = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsockopt");
        close(socket_fd);
        return -1;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(socket_fd);
        return -1;
    }

    const int conn_backlog = 5;

    // Set it up as a passive socket
    if (listen(socket_fd, conn_backlog) < 0)
    {
        perror("listen");
        close(socket_fd);
        return -1;
    }

    // Set the server socket to non-blocking mode
    fd_set_nonblocking(socket_fd);
    return socket_fd;
}

void server_run(int socket_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;

    printf("Server listening on port %d...\n", PORT);

    Conn *connections[MAX_CLIENTS];

    while (1)
    {
        int poll_count = poll(fds, nfds, -1);
        errno = 0;
        if (poll_count < 0 && errno == EINTR)
        {
            continue;
        }
        if (poll_count < 0)
        {
            perror("poll");
            break; // exit the loop
        }

        // Check for new connections
        if (fds[0].revents & POLLIN)
        {
            int new_fd;
            while ((new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len)) > 0)
            {
                fd_set_nonblocking(new_fd);
                if (nfds >= MAX_CLIENTS)
                {
                    fprintf(stderr, "Max clients reached\n");
                    close(new_fd);
                    break; // exit the loop
                }
                else
                {
                    fds[nfds].fd = new_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    init_conn(connections[nfds], new_fd, 1, 0, 0);
                    printf("New client connected: fd=%d\n", new_fd);
                }
            }

            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                perror("accept");
                close(new_fd);
                break;
            }
        }

        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int cli_fd = fds[i].fd;
            }
        }
    }
}
