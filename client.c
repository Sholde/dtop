#include <stdio.h>  // printf
#include <stdlib.h> // NULL
#include <string.h> // memset
#include <unistd.h> // close
#include <errno.h>  // errno
#include <signal.h> // signal

// open
#include <sys/stat.h>
#include <fcntl.h>

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "struct.h"
#include "sensor.h"
#include "display.h"
#include "server.h"
#include "client.h"
#include "io.h"

int stop_client = 0;

static void handle_standard(int sock)
{
  message_client_t msg_client;
  message_server_t msg_server;

  int ret = 0;
    
  // monitoring
  machine_info_t *m = sensor();

  // First time server don't update, i don't know why...
  for (int i = 0; i < 2; i++)
    {
      msg_client.deconnect = stop_client;
      memcpy(&(msg_client.machine), m, sizeof(machine_info_t));
      
      // write on fd m
      safe_write(sock, &msg_client, sizeof(message_client_t));
      
      // read on fd serv
      ret = safe_read(sock, &msg_server, sizeof(message_server_t));

      if (ret == -1 || msg_server.deconnect)
        {
          return;
        }
    }

  // free monitoring
  free(m);

  // display
  for (int i = 0; i < msg_server.serv.max_users; i++)
    {
      if (msg_server.serv.client[i].active)
        display(&(msg_server.serv.client[i].machine_info));
    }

  // Stoping client
  while (1)
    {
      stop_client = 1;
      msg_client.deconnect = stop_client;
  
      safe_write(sock, &msg_client, sizeof(message_client_t));
      ret = safe_read(sock, &msg_server, sizeof(message_server_t));

      if (ret == -1 || msg_server.deconnect)
        {
          break;
        }
    }
}

static void handle_loop(int sock)
{
  int refresh_counter = 0;
  machine_info_t *m = NULL;
  message_client_t msg_client;
  message_server_t msg_server;
  
  while (1)
    {
      refresh_counter++;

      // monitoring
      m = sensor();

      msg_client.deconnect = stop_client;
      memcpy(&(msg_client.machine), m, sizeof(machine_info_t));
      free(m);
      
      // write on fd m
      safe_write(sock, &msg_client, sizeof(message_client_t));
      
      // read on fd serv
      int ret = safe_read(sock, &msg_server, sizeof(message_server_t));

      if (ret == -1 || msg_server.deconnect)
        {
          break;
        }

      // display
      printf("\n");
      printf("Refresh: %d\n", refresh_counter);
      
      for (int i = 0; i < msg_server.serv.max_users; i++)
        {
          if (msg_server.serv.client[i].active)
            display(&(msg_server.serv.client[i].machine_info));
        }
    }
}

static void handle_file(int sock, char *path)
{
  // Redirect printf
  int fd = open(path, O_CREAT | O_WRONLY, 0666);
  dup2(fd, STDOUT_FILENO);

  handle_standard(sock);

  close(fd);
}

static void handle_stop(int sig)
{
  printf("call\n");
  stop_client = 1;
}

static void client_check_arg(int ipv, enum mode_client mode, char *path)
{
  // IP version
  if (ipv != 4 && ipv != 6)
    {
      fprintf(stderr, "Error: bad ip version\n");
      exit(EXIT_FAILURE);
    }

  // mode
  if (mode != OUTPUT_FILE && mode != STANDARD && mode != LOOP)
    {
      fprintf(stderr, "Error: bad mode\n");
      exit(EXIT_FAILURE);
    }

  if (mode == OUTPUT_FILE && !path)
    {
      fprintf(stderr, "Error: bad mode\n");
      exit(EXIT_FAILURE);
    }
}

static int client_connect(const int ipv, const char *ip, const char *port)
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
  int ret = getaddrinfo(ip, port, &hints, &addr_info);

  if (ret < 0)
    {
      herror("getaddrinfo");
      exit(EXIT_FAILURE);
    }

  // Varaible to connect to soket and listen
  struct addrinfo *tmp_ai  = NULL;
  int sock = -1;
  int connected = 0;
  
  for (tmp_ai = addr_info; tmp_ai != NULL; tmp_ai = tmp_ai->ai_next)
    {
      sock = socket(tmp_ai->ai_family, tmp_ai->ai_socktype, tmp_ai->ai_protocol);

      if (sock < 0)
        {
          perror("socket");
          continue;
        }

      int ret = connect(sock, tmp_ai->ai_addr, tmp_ai->ai_addrlen);

      if (ret < 0)
        {
          perror("connect");
          continue;
        }

      connected = 1;
      break;
    }

  // Check if we are connected
  if (!connected)
    {
      fprintf(stderr, "Failed to connect on %s:%s\n", ip, port);
      exit(EXIT_FAILURE);
    }

  return sock;
}

void client(int ipv, enum mode_client mode, char *ip, char *port, char *path)
{
  // Checking argument
  client_check_arg(ipv, mode, path);

  // Connect
  int sock = client_connect(ipv, ip, port);

  // Select mode
  switch (mode)
    {
    case STANDARD:
      handle_standard(sock);
      break;
      
    case LOOP:
      // Handle stop
      signal(SIGINT, handle_stop);

      handle_loop(sock);
      break;
      
    case OUTPUT_FILE:
      handle_file(sock, path);
      break;
      
    default:
      fprintf(stderr, "Error: bad mode\n");
      exit(EXIT_FAILURE);
      break;
    }

  // Clean
  shutdown(sock, SHUT_RDWR);

  close(sock);
}


