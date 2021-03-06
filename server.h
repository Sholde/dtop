#ifndef _server_h_
#define _server_h_

#include "struct.h"

typedef struct client_info_s
{
  size_t id;
  char active;
  int client_socket;
  struct sockaddr *info;
  machine_info_t *machine_info;
} client_info_t;

typedef struct server_s
{
  int nb_users;
  client_info_t *client;
} server_t;

void server(int, char *, int);

#endif
