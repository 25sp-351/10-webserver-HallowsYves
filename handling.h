#ifndef HANDLING_H
#define HANDLING_H
#include "parse.h"

#define DEFAULT_CONTENT_TYPE "application/octet-stream";
const char *get_extension(const char *filepath);

int handle_image(int client_fd, struct HttpRequest *req);
int handle_calc(int client_fd, struct HttpRequest *req);
void *handle_client(void *arg);

#endif 