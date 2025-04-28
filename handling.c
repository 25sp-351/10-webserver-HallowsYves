#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"
#include "parse.h"
#include "handling.h"

int handle_image(int client_fd, struct HttpRequest *req)
{
    char filepath[BUFFER_SIZE];
    
    snprintf(filepath, sizeof(filepath), "./static%s", req->path + 7);


    FILE *file_pointer = fopen(filepath, "rb");
    if(!file_pointer) {
        printf("File not found, Sending 404\n"); 
        send_error_response(client_fd, 404);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;

    if (file_pointer == NULL)
    {
        send_error_response(client_fd, 404);
        return -1;
    }

    // Move pointer to end
    if (fseek(file_pointer, 0, SEEK_END) != 0)
    {
        perror("fseek to end failed");
        fclose(file_pointer);
        send_error_response(client_fd, 500);
        return -1;
    }

    // Get file size
    long file_size_bytes = ftell(file_pointer);
    if (file_size_bytes < 0)
    {
        perror("ftell failed");
        fclose(file_pointer);
        send_error_response(client_fd, 500);
        return -1;
    }

    // Rewind
    if (fseek(file_pointer, 0, SEEK_SET) != 0)
    {
        perror("fseek to beginning failed.");
        fclose(file_pointer);
        send_error_response(client_fd, 500);
        return -1;
    }

    // Get file extension, to determine content type
    const char *file_extension = get_extension(filepath);
    const char *content_type = DEFAULT_CONTENT_TYPE;

    if (file_extension != NULL)
    {
        if (strcmp(file_extension, ".png") == 0)
        {
            content_type = "image/png";
        }
        else if (strcmp(file_extension, ".jpeg") == 0 || strcmp(file_extension, ".jpg") == 0)
        {
            content_type = "image/jpeg";
        }
        else if (strcmp(file_extension, ".gif") == 0)
        {
            content_type = "image/gif";
        }
        else
        {
            content_type = DEFAULT_CONTENT_TYPE;
        }
    }
    // 200 OK header
    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "\r\n",
             content_type, file_size_bytes);

    // SEND HEADER
    send(client_fd, header, strlen(header), 0);

    // Send File Contents
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file_pointer)) > 0)
    {
        send(client_fd, buffer, bytes_read, 0);
    }

    fclose(file_pointer);
    return 0;
}

const char *get_extension(const char *filepath)
{
    int path_length = strlen(filepath);
    const char *extension = NULL;
    for (int index = path_length - 1; index >= 0; index--)
    {
        if (filepath[index] == '.')
        {
            extension = &filepath[index];
            break;
        }
    }
    return extension;
}

int handle_calc(int client_fd, struct HttpRequest *req)
{
    char temp_buffer[BUFFER_SIZE];
    strcpy(temp_buffer, req->path + 6);

    char *operation = strtok(temp_buffer, "/");
    char *num_one_string = strtok(NULL, "/");
    char *num_two_string = strtok(NULL, "/");

    if (!operation || !num_one_string || !num_two_string)
    {
        send_error_response(client_fd, 400);
        return -1;
    }

    char *endpointer_one;
    char *endpointer_two;

    long num_one = strtol(num_one_string, &endpointer_one, 10);
    long num_two = strtol(num_two_string, &endpointer_two, 10);

    int result;

    if (*endpointer_one != '\0' || *endpointer_two != '\0')
    {
        send_error_response(client_fd, 400);
        return -1;
    }

    if (strcmp(operation, "add") == 0)
    {
        result = (int)(num_one + num_two);
    }
    else if (strcmp(operation, "mul") == 0)
    {
        result = (int)(num_one * num_two);
    }
    else if (strcmp(operation, "div") == 0)
    {
        if (num_two == 0)
        {
            send_error_response(client_fd, 400);
            return -1;
        }
        result = (int)(num_one / num_two);
    }
    else
    {
        send_error_response(client_fd, 400);
        return -1;
    }

    char result_body[32];
    int result_length = snprintf(result_body, sizeof(result_body), "%d", result);

    // Build full HTTP response
    char response[BUFFER_SIZE];
    int response_length = snprintf(response, sizeof(response),
                                   "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: %d\r\n"
                                   "\r\n"
                                   "%s",
                                   result_length, result_body);

    send(client_fd, response, response_length, 0);
    return 0;
}

int handle_sleep(int client_fd, struct HttpRequest *req) 
{
    char temp_buffer[BUFFER_SIZE];
    strcpy(temp_buffer, req->path + 7);

    char *seconds_string = strtok(temp_buffer, "/");
    if (!seconds_string) {
        send_error_response(client_fd, 400);
        return -1;
    }

    char *end_pointer;
    long seconds = strtol(seconds_string, &end_pointer, 10);
    if (*end_pointer != '\0' || seconds < 0) {
        send_error_response(client_fd, 400);
        return -1;
    }

    printf("Sleeping for %ld seconds...\n", seconds);
    sleep((unsigned int)seconds);

    // 200 OK
    const char *body = "Slept successfully!\n";
    char response[BUFFER_SIZE];
    int response_length = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        strlen(body), body);

    send(client_fd, response, response_length, 0);
    return 0;
}

void *handle_client(void *arg)
{
    int client_fd = *((int *) arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) 
    {
        close(client_fd);
        pthread_exit(NULL);
    }

    struct HttpRequest req;
    parse_request_line(buffer, bytes_received, &req);
    printf("Request path: %s\n", req.path);

    if (validate_request(client_fd, &req) == 0) 
    {
        if (strncmp(req.path, "/static", 7) == 0) 
        {
            handle_image(client_fd, &req);
        } else if (strncmp(req.path, "/calc", 5) == 0) 
        {
            handle_calc(client_fd, &req);
        } else if (strncmp(req.path, "/sleep", 6) == 0) 
        {
            handle_sleep(client_fd, &req);
        } 
        else 
        {
            send_error_response(client_fd, 404);
        }
    }
    close(client_fd);
    pthread_exit(NULL);
}