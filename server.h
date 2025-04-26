#ifndef SERVER_H
#define SERVER_H


#define BUFFER_SIZE 1024
#define DEFAULT_PORT 2345

void parse_arguments(int argc, char* argv[], int *port, int *verbose);
int server_setup(int port, int verbose);
void start_server(int server_fd, int verbose);

#endif