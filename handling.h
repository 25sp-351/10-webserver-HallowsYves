#ifndef HANDLING_H
#define HANDLING_H
#include "parse.h"

int handle_image(int client_fd, struct HttpRequest *req);
#define DEFAULT_CONTENT_TYPE "application/octet-stream";
const char *get_extension(const char *filepath);

#endif 