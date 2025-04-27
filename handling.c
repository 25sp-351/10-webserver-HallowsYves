#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.h"
#include "parse.h"
#include "handling.h"

int handle_image(int client_fd, struct HttpRequest *req)
{
    char *get_path = req->path;
    char *path = get_path + 7;

    char filepath[BUFFER_SIZE];
    snprintf(filepath, sizeof(filepath), "./static%s", path);

    FILE *file_pointer = fopen(filepath, "rb");

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