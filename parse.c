#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "parse.h"
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

/*
	Extract the first line from request_buffer.
    Split the first line by spaces.
*/
void parse_request_line(char *request_buffer, int bytes_received, struct HttpRequest *req) {
    char request_line[BUFFER_SIZE];
    int line_index = 0;
    
    // Loop through bytes_received, and copy the request buffer
    for (int index = 0; index < bytes_received; index++) {
        if (request_buffer[index] == '\r' && request_buffer[index + 1] == '\n') {
            request_line[line_index] = '\0';
            break;
        }
        request_line[line_index] = request_buffer[line_index];
        line_index++;
    }

    // Split into respective parts, and copy into httpRequest
    int count = 0;
    char* delimiter = strtok(request_line, " ");
    while (delimiter != NULL) {
        if (count == 0) {
            strcpy(req->method, delimiter);
        } else if (count == 1) {
            strcpy(req->path, delimiter);
        } else if (count == 2) {
            strcpy(req->protocol, delimiter);
        }

        count++;
        delimiter = strtok(NULL, " ");
    }

}