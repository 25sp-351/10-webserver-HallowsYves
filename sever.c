#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.h"


void parse_arguments(int argc, char* argv[], int *port, int *verbose) {
    for (int index = 0; index < argc; index++) {
        // Check for "-p" and "-v"
        if (strcmp(argv[index], "-p") == 0 && index + 1 < argc) {
            
            // Convert String to int
            char *end_pointer;
            long port_long = strtol(argv[index + 1], &end_pointer, 10);

            if (*end_pointer != '\0' || port_long <= 0 || port_long > 65535) {
                fprintf(stderr, "Invalid port number.");
                exit(EXIT_FAILURE);
            }
            port = (int)port_long;
            index++;
        } else if (strcmp(argv[index], "-v") == 0) {
            verbose = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[index]);
            fprintf(stderr, "Usage: %s [-p port] [-v]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }


}