#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#define CONN_BACKLOG 128
#define MAX_CLIENTS 1024
#define BUFFER_SIZE 8192
#define PORT 6379

typedef struct
{
    int fd;
    bool read;
    bool write;
    bool close;

    int *in_buffer;
    int bytes_in;
    int *out_buffer;
    int bytes_out;
} Conn;

void init_conn(Conn *conn, int fd, bool read_val, bool write_val, bool close_val);
void free_conn(Conn *conn);

int server_setup();
void server_run(int socket_fd);

#endif
