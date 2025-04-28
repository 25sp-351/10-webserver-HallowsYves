#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"
#include "parse.h"
#include "handling.h"

int server_setup(int port, int verbose) {
    // Declare essential variables.
    int server_fd;
    struct sockaddr_in server_addr;

    // Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket \n");
        exit(EXIT_FAILURE);
    }

    // Setup Server Addresses
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start Listening
    if (listen(server_fd, 1) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void start_server(int server_fd, int verbose) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd;

    char buffer[BUFFER_SIZE];

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) 
        {
            perror("accept");
            continue;
        }

        printf("Client connected! \n");

        pthread_t thread_id;

        int *pclient = malloc(sizeof(int));
        *pclient = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, pclient) != 0) 
        {
            perror("pthread_create");
            close(client_fd);
            free(pclient);
            continue;
        }
        pthread_detach(thread_id);
    }

}

int validate_request(int client_fd, struct HttpRequest *req) {
    
    if (strcmp(req->method, "GET") != 0) {
        send_error_response(client_fd, 405);
        return -1;
    }

    if (strcmp(req->protocol, "HTTP/1.1") != 0) {
        send_error_response(client_fd, 400);
        return -1;
    }

    if (strncmp(req->path, "/static", 7) == 0 || strncmp(req->path, "/calc", 5) == 0) {
        printf("Valid. \n");
    } else {
        send_error_response(client_fd, 404);
    }

    return 0;
}

int send_error_response(int client_fd, int status_code) {
    char response[BUFFER_SIZE];
    const char *status_text;
    const char *body_text;

    if (status_code == 400) {
        status_text = "400 Bad Request";
        body_text = "400 Bad Request";
    } else if (status_code == 404) {
        status_text = "404 Not Found";
        body_text = "404 Not Found";
    } else if (status_code == 405) {
        status_text = "405, Method Not Allowed";
        body_text = "405, Method Not Allowed";
    } else if (status_code == 500) {
        status_text = "500, Internal Server Error";
        body_text = "500, Internal Server Error";
    }

    int body_length = strlen(body_text);

    snprintf(response, sizeof(response),
        "HTTP/1.1 %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        status_text, body_length, body_text);

    // send response
    send(client_fd, response, strlen(response), 0);
}