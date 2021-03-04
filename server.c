#include <stdio.h>   // printf
#include <stdlib.h>  // NULL, malloc...
#include <string.h>  // memset
#include <unistd.h>  // close
#include <pthread.h> // pthread

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "server.h"

typedef struct client_info_s
{
  int client_socket;
} client_info_t;

void *client_loop(void *arg)
{
  client_info_t *client = (client_info_t *)arg;

  printf("New connection\n");

  write(client->client_socket, "Hello from server!\n", 20);
  close(client->client_socket);

  free(client);

  return NULL;
}

void server(int ipv)
{
  // Check argument
  if (ipv != 4 && ipv != 6 && ipv != 10)
    {
      fprintf(stderr, "Error: bad ip version %d\n", ipv);
      exit(1);
    }
  
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
  int ret = getaddrinfo(NULL, SERVER_PORT, &hints, &addr_info);

  if (ret < 0)
    {
      herror("getaddrinfo");
      exit(EXIT_FAILURE);
    }

  // Varaible to connect to soket and listen
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
      fprintf(stderr, "Failed to bind on 0.0.0.0:%s\n", SERVER_PORT);
      exit(EXIT_FAILURE);
    }

  // Listen
  ret = listen(listen_sock, 2);

  if (ret < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }

  // Server
  struct sockaddr client_info;
  socklen_t addrlen;  

  // Server
  while (1) /* infinite loop */
    {
      int client_socket = accept(listen_sock, &client_info, &addrlen);

      if (client_socket < 0)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }

      client_info_t *client = malloc(sizeof(client_info_t));

      if (!client)
        {
          fprintf(stderr, "Error: canno't allocate memory\n");
          exit(EXIT_FAILURE);
        }
      
      client->client_socket = client_socket;

      pthread_t th;
      
      pthread_create(&th, NULL, client_loop, client);
      pthread_detach(th);
    }

  close(listen_sock);
}
