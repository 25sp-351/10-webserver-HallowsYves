#ifndef PARSE_H
#define PARSE_H


void parse_arguments(int argc, char* argv[], int *port, int *verbose);

struct HttpRequest
{
    char method[8];
    char path[1024];
    char protocol[16];
};


void parse_request_line(char *request_buffer, struct HttpRequest *req);

#endif