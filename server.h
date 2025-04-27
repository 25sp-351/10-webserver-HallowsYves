#ifndef SERVER_H
#define SERVER_H
#include "parse.h"


#define BUFFER_SIZE 1024
#define DEFAULT_PORT 2345

void parse_arguments(int argc, char* argv[], int *port, int *verbose);
int server_setup(int port, int verbose);
void start_server(int server_fd, int verbose);
int send_error_response(int client_fd, int status_code);
int validate_request(int client_fd, struct HttpRequest *req);

#endif