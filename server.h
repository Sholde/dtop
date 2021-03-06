#ifndef _server_h_
#define _server_h_

typedef struct client_info_s
{
  int client_socket;
  struct sockaddr *info;
} client_info_t;

typedef struct server_s
{
  int nb_users;
  char active;
  client_info_t *client;
} server_t;

void server(int, char *, int);

#endif
