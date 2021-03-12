#ifndef _server_h_
#define _server_h_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "struct.h"

#define MAX_CLIENT 20

typedef struct client_info_s
{
  size_t id;
  char active;
  int client_socket;
  struct sockaddr_in info;
  machine_info_t machine_info;
} client_info_t;

typedef struct server_s
{
  int max_users;
  int nb_users;
  client_info_t client[MAX_CLIENT];
} server_t;

typedef struct message_server_s
{
  int deconnect;
  server_t serv;
} message_server_t;

void server(int, char *, int);

#endif
