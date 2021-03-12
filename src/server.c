#include <stdio.h>   // printf
#include <stdlib.h>  // NULL, malloc...
#include <string.h>  // memset
#include <unistd.h>  // close
#include <pthread.h> // pthread
#include <errno.h>   // errno
#include <signal.h>  // signal

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// inet_ntoa
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "client.h"
#include "io.h"

int stop_server = 0;

static server_t *init_server(const int max_users)
{
  server_t *serv = malloc(sizeof(server_t));

  if (!serv)
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
      memset(&(serv->client[i].machine_info), 0, sizeof(machine_info_t));
    }

  return serv;
}

static void destroy_server(server_t *serv)
{
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

static int search_socket(const server_t *serv, int sock)
{
  for (int i = 0; i < serv->max_users; i++)
    {
      if (serv->client[i].active && serv->client[i].client_socket == sock)
        {
          return i;
        }
    }
 
  return -1;
}

static void server_check_argument(const int ipv, const int max_users)
{
  if (ipv != 4 && ipv != 6)
    {
      fprintf(stderr, "Error: bad ip version %d\n", ipv);
      exit(1);
    }

  if (max_users < 0 || max_users >= MAX_CLIENT)
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
      hints.ai_family = AF_INET;
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

static void handle_stop(int sig)
{
  stop_server = 1;
  printf("\n");
}

static void server_deconnect_all_client(server_t *serv, fd_set *active_fd_set)
{
  int fd = 0;
  
  for (int i = 0; i < serv->max_users; i++)
    {
      if (serv->client[i].active)
        {
          // Deconnect
          fd = serv->client[i].client_socket;
          close(fd);
          FD_CLR(fd, active_fd_set);

          // Decrement user
          serv->nb_users--;
          serv->client[i].active = 0;
          serv->client[i].client_socket = -1;
        }
    }
}

static void server_deconnect_client(server_t *serv, int index, fd_set *active_fd_set)
{
  // Print deconnection message
  fprintf(stderr, "Deconnection from %s:%d\n", inet_ntoa(serv->client[index].info.sin_addr), ntohs(serv->client[index].info.sin_port));

  // Deconnect
  int fd = serv->client[index].client_socket;
  close(fd);
  FD_CLR(fd, active_fd_set);

  // Decrement user
  serv->nb_users--;
  serv->client[index].active = 0;
  serv->client[index].client_socket = -1;

  // Print number of users
  fprintf(stderr, "We are now %d/%d users\n", serv->nb_users, serv->max_users);
}

static void server_select_client(server_t *serv, int listen_sock, int i, fd_set *active_fd_set)
{
  // Event of listen socket
  struct sockaddr_in client_info;
  unsigned int addr_size = sizeof(struct sockaddr_in);
  int client_socket = accept(listen_sock, (struct sockaddr *)&client_info, &addr_size);

  if (client_socket < 0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }

  // If client are already connected
  if (FD_ISSET(client_socket, active_fd_set))
    {
      fprintf(stderr, "Client already connected %s:%d\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));
      close(client_socket);
      return;
    }

  // Test if we have place for new user
  if (serv->nb_users >= serv->max_users)
    {
      fprintf(stderr, "Server: user %s:%d try to connect but we are full %d/%d users\n",
              inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port), serv->nb_users, serv->max_users);
      close(client_socket);
      return;
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
      serv->client[index].id = index;
      serv->client[index].active = 1;
      serv->client[index].client_socket = client_socket;
      memcpy(&(serv->client[index].info), &client_info, sizeof(struct sockaddr_in));
    }

  // Print deconnection message
  fprintf(stderr, "New connection from %s:%d\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

  // Adding new client socket
  FD_SET(client_socket, active_fd_set);

  // Increment user
  serv->nb_users++;

  // Print number of users
  fprintf(stderr, "We are now %d/%d users\n", serv->nb_users, serv->max_users);
}

static inline void server_handle_read(message_client_t *msg_client,
                                      server_t *serv, int fd, fd_set *active_fd_set,
                                      int tab_of_deconnect_sock[])
{
  // Search
  int index = search_socket(serv, fd);

  if (index == -1)
    {
      fprintf(stderr, "Error: canno't find socket\n");
      exit(EXIT_FAILURE);
    }

  // Handle
  int ret = safe_read(fd, msg_client, sizeof(message_client_t));

  if (ret == -1)
    {
      server_deconnect_client(serv, fd, active_fd_set);
    }
  else
    {
      if (msg_client->deconnect)
        {
          tab_of_deconnect_sock[index] = 1;
        }
      
      // Copy info from buff
      memcpy(&(serv->client[index].machine_info), &(msg_client->machine), sizeof(machine_info_t));
    }
}

static void server_accept(const int listen_sock, const int max_users)
{
  fd_set active_fd_set, read_fd_set;
  /* Initialize the set of active sockets */
  FD_ZERO(&active_fd_set);
  FD_SET(listen_sock, &active_fd_set);

  int tab_of_deconnect_sock[MAX_CLIENT] = { 0 };
  
  server_t *serv = init_server(max_users);

  message_client_t msg_client;
  message_server_t msg_server;

  while (!stop_server) /* infinite loop */
    {
      // Deconnect socket
      for (int i = 0; i < MAX_CLIENT; i++)
        {
          if (tab_of_deconnect_sock[i])
            {
              server_deconnect_client(serv, i, &active_fd_set);
              tab_of_deconnect_sock[i] = 0;
            }
        }
      
      // Set reading sock
      read_fd_set = active_fd_set;

      // Block until data arrive on one or more socket
      if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL ) < 0)
        {
          if (!stop_server)
            {
              perror("select");
              exit(EXIT_FAILURE);
            }
          else
            {
              // Case we have teminate server with a sigint signal
              break;
            }
        }

      // Handle all client
      for (int i = 0; i < FD_SETSIZE; i++)
        {
          if (FD_ISSET(i, &read_fd_set))
            {
              if (i == listen_sock)
                {
                  server_select_client(serv, listen_sock, i, &active_fd_set);
                }
              else
                {
                  server_handle_read(&msg_client, serv, i, &active_fd_set, tab_of_deconnect_sock);
                }
            }
        }

      // Send message of all active socket (~ all client)
      for (int i = 0; i < serv->max_users; i++)
        {
          if (serv->client[i].active)
            {
              if (tab_of_deconnect_sock[i])
                {
                  msg_server.deconnect = 1;
                }
              else
                {
                  msg_server.deconnect = 0;
                }
              
              memcpy(&(msg_server.serv), serv, sizeof(server_t));
              
              int ret = safe_write(serv->client[i].client_socket, &msg_server, sizeof(message_server_t));

              if (ret == -1)
                {
                  server_deconnect_client(serv, i, &active_fd_set);
                }
            }
        }
    }

  // Deconnected all client
  server_deconnect_all_client(serv, &active_fd_set);
  
  // Clean
  destroy_server(serv);

  fprintf(stderr, "Server stop\n");
}

void server(int ipv, char *port, int max_users)
{
  // Check argument
  server_check_argument(ipv, max_users);

  // Handle signal
  signal(SIGINT, handle_stop);

  // Bind
  int listen_sock = server_bind(ipv, port);
              
  // Listen
  server_listen(listen_sock, max_users);
  
  // Accept
  server_accept(listen_sock, max_users);

  // Close listen socket
  close(listen_sock);
}
