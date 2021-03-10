#include <stdio.h>   // printf
#include <stdlib.h>  // NULL, malloc...
#include <string.h>  // memset
#include <unistd.h>  // close
#include <pthread.h> // pthread
#include <errno.h>   // errno

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// inet_ntoa
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "io.h"

static void handle_client_socket(server_t *serv, int sock, int index)
{
  // read from client
  //  read(sock, serv->client[index].machine_info, sizeof(machine_info_t));
}

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

static void server_accept(const int listen_sock, const int max_users)
{
  fd_set active_fd_set, read_fd_set;
  /* Initialize the set of active sockets */
  FD_ZERO(&active_fd_set);
  FD_SET(listen_sock, &active_fd_set);
  
  server_t *serv = init_server(max_users);

  while (1) /* infinite loop */
    {
      read_fd_set = active_fd_set;

      // Block until data arrive on one or more socket
      if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL ) < 0 )
        {
          perror("select");
          exit(EXIT_FAILURE);
        }

      // Handle all client
      for (int i = 0; i < FD_SETSIZE; i++)
        if (FD_ISSET(i, &read_fd_set))
          {
            if (i == listen_sock)
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

                // To test
                if (FD_ISSET(client_socket, &active_fd_set))
                  {
                    fprintf(stderr, "Client already connected %s:%d\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));
                    close(client_socket);
                    continue;
                  }

                // Test if we have place for new user
                if (serv->nb_users >= max_users)
                  {
                    fprintf(stderr, "Error: a user try to connect but we are full %d/%d users\n", serv->nb_users, serv->max_users);
                    close(client_socket);
                    continue;
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
                FD_SET(client_socket, &active_fd_set);

                // Increment user
                serv->nb_users++;

                // Print number of users
                fprintf(stderr, "We are now %d/%d users\n", serv->nb_users, serv->max_users);
              }
            else
              {
                // Search
                int index = search_socket(serv, i);

                if (index == -1)
                  {
                    fprintf(stderr, "Error: canno't find socket\n");
                    exit(EXIT_FAILURE);
                  }

                // Handle
                machine_info_t machine_buff;
                int nbytes = safe_read(i, &machine_buff, sizeof(machine_info_t));

                if (nbytes < 0)
                  {
                    perror("read");
                    exit(EXIT_FAILURE);
                  }
                else if (nbytes == -1)
                  {
                    // Print deconnection message
                    fprintf(stderr, "Deconnection from %s:%d\n", inet_ntoa(serv->client[index].info.sin_addr), ntohs(serv->client[index].info.sin_port));

                    // Deconnect
                    close(i);
                    FD_CLR(i, &active_fd_set);

                    // Decrement user
                    serv->nb_users--;
                    serv->client[index].active = 0;
                    serv->client[index].client_socket = -1;

                    // Print number of users
                    fprintf(stderr, "We are now %d/%d users\n", serv->nb_users, serv->max_users);
                  }
                else
                  {
                    // Copy info from buff
                    memcpy(&(serv->client[index].machine_info), &machine_buff, sizeof(machine_info_t));
                  }
              }

            // Send message of all active socket (~ all client)
            for (int i = 0; i < serv->max_users; i++)
              {
                if (serv->client[i].active)
                  {
                    safe_write(serv->client[i].client_socket, serv, sizeof(server_t));
                  }
              }
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
