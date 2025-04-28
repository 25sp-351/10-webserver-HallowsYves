#include <stdio.h>
#include <stdlib.h>
#include "handling.h"
#include "server.h"
#include "parse.h"
int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    int verbose = 0;

    parse_arguments(argc, argv, &port, &verbose);
    int server_fd = server_setup(port);
    start_server(server_fd);
}