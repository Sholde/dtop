#include <stdio.h>   // printf
#include <stdlib.h>  // NULL, malloc...
#include <string.h>  // memset
#include <unistd.h>  // close
#include <pthread.h> // pthread

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// inet_ntoa
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"

static void *client_loop(void *arg)
{
  // Recast argument
  server_t *serv = (server_t *)arg;

  // Increment user
  serv->nb_users++;

  // Connexion
  client_info_t *client = serv->client;
  struct sockaddr_in *addr = (struct sockaddr_in *)client->info;
  printf("New connection from %s:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));

  //

  // Deconnexion
  close(serv->client->client_socket);
  printf("Deconnection from %s:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));

  // Decrement user
  serv->nb_users--;

  // Disable
  close(serv->client->client_socket);
  serv->client->active = 0;


  return NULL;
}

static server_t *init_server(const int max_users)
{
  server_t *serv = malloc(sizeof(server_t));

  if (!serv)
    {
      fprintf(stderr, "Error: canno't allocate memory\n");
      exit(EXIT_FAILURE);
    }
  
  serv->client = malloc(sizeof(client_info_t) * max_users);
  
  if (!serv->client)
    {
      fprintf(stderr, "Error: canno't allocate memory\n");
      exit(EXIT_FAILURE);
    }

  serv->nb_users = 0;
  serv->max_users = max_users;

  for (int i = 0; i < serv->max_users; i++)
    {
      serv->client[i].id = -1;
      serv->client[i].active = 0;
      serv->client[i].client_socket = -1;
      serv->client[i].info = NULL;
      serv->client[i].machine_info = NULL;
    }

  return serv;
}

static void destroy_server(server_t *serv)
{
  free(serv->client);
  free(serv);
}

static int search_place(const server_t *serv)
{
  for (int i = 0; i < serv->max_users; i++)
    {
      if (!serv->client[i].active)
        {
          return i;
        }
    }
  
  return -1;
}

static void server_check_argument(const int ipv, const int max_users)
{
  if (ipv != 4 && ipv != 6 && ipv != 0)
    {
      fprintf(stderr, "Error: bad ip version %d\n", ipv);
      exit(1);
    }

  if (max_users < 0)
    {
      fprintf(stderr, "Error: bad number of max users %d\n", max_users);
      exit(1);
    }
}

static int server_bind(const int ipv, const char *port)
{
  // Init address info variable
  struct addrinfo *addr_info  = NULL;
  struct addrinfo  hints;

  memset(&hints, 0, sizeof(hints));

  if (ipv == 4)
    {
      hints.ai_family = AF_INET;
    }
  else if (ipv == 6)
    {
      hints.ai_family = AF_INET6;
    }
  else
    {
      hints.ai_family = AF_UNSPEC;
    }

  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;
  
  // Call getaddinfo to get address info
  int ret = getaddrinfo(NULL, port, &hints, &addr_info);

  if (ret < 0)
    {
      herror("getaddrinfo");
      exit(EXIT_FAILURE);
    }

  // Variable to connect to soket and listen
  struct addrinfo *tmp_ai  = NULL;
  int listen_sock = -1;
  int binded = 0;
  
  for (tmp_ai = addr_info; tmp_ai != NULL; tmp_ai = tmp_ai->ai_next)
    {
      listen_sock = socket(tmp_ai->ai_family, tmp_ai->ai_socktype, tmp_ai->ai_protocol);

      if (listen_sock < 0)
        {
          perror("socket");
          continue;
        }

      int ret = bind(listen_sock, tmp_ai->ai_addr, tmp_ai->ai_addrlen);

      if (ret < 0)
        {
          perror("bind");
          continue;
        }

      binded = 1;
      break;
    }

  // Check if we are bind
  if (!binded)
    {
      fprintf(stderr, "Failed to bind on 0.0.0.0:%s\n", port);
      exit(EXIT_FAILURE);
    }

  return listen_sock;
}

static void server_listen(const int listen_sock, const int max_users)
{
  int ret = listen(listen_sock, max_users);

  if (ret < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }
}

static void server_accept(const int listen_sock, const int max_users)
{
  struct sockaddr client_info;
  socklen_t addrlen;

  server_t *serv = init_server(max_users);

  while (1) /* infinite loop */
    {
      // Accept client
      int client_socket = accept(listen_sock, &client_info, &addrlen);

      // Checking accept
      if (client_socket < 0)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }

      // Test if we have place for new user
      if (serv->nb_users >= max_users)
        {
          close(client_socket);
        }
      else // If we have place for new user
        {
          // Search place
          int index = search_place(serv);

          if (index == -1)
            {
              fprintf(stderr, "Error: canno't find place\n");
              exit(EXIT_FAILURE);
            }

          // Fill structure
          serv->max_users = max_users;
          serv->client[index].id = index;
          serv->client[index].active = 1;
          serv->client[index].client_socket = client_socket;
          serv->client[index].info = &client_info;

          // Throw thread
          pthread_t th;
          pthread_create(&th, NULL, client_loop, serv);
          pthread_detach(th);
        }
    }

  // Clean
  destroy_server(serv);
}

void server(int ipv, char *port, int max_users)
{
  // Check argument
  server_check_argument(ipv, max_users);
  
  // Bind
  int listen_sock = server_bind(ipv, port);
              
  // Listen
  server_listen(listen_sock, max_users);
  
  // Accept
  server_accept(listen_sock, max_users);

  // Close listen socket
  close(listen_sock);
}
