#include <stdio.h>  // printf
#include <stdlib.h> // NULL
#include <string.h> // memset
#include <unistd.h> // close
#include <errno.h>  // errno
#include <signal.h> // signal

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
  int refresh_counter = 0;
  machine_info_t *m = NULL;
  server_t serv;
  
  while (!stop_client)
    {
      refresh_counter++;

      // monitoring
      m = sensor();

      // write on fd m
      safe_write(sock, m, sizeof(machine_info_t));
      free(m);
      
      // read on fd serv
      int nbytes = 0;

      nbytes = safe_read(sock, &serv, sizeof(server_t));

      if (nbytes == -1)
        {
          exit(EXIT_FAILURE);
        }

      // display
      printf("\n");
      printf("Refresh: %d\n", refresh_counter);
      
      for (int i = 0; i < serv.max_users; i++)
        {
          if (serv.client[i].active)
            display(&(serv.client[i].machine_info));
        }
    }
}

static void handle_stop(int sig)
{
  stop_client = 1;
}

static void handle_interactif(int sock)
{
  fprintf(stderr, "Not yet implemented!\n");
  machine_info_t *m = sensor();
  display(m);
  free(m);
}

static void client_check_arg(int ipv, enum mode_client mode)
{
  // IP version
  if (ipv != 4 && ipv != 6)
    {
      fprintf(stderr, "Error: bad ip version\n");
      exit(EXIT_FAILURE);
    }

  // mode
  if (mode != INTERACTIF && mode != STANDARD)
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

void client(int ipv, enum mode_client mode, char *ip, char *port)
{
  // Checking argument
  client_check_arg(ipv, mode);

  // Handle stop
  signal(SIGINT, handle_stop);

  // Connect
  int sock = client_connect(ipv, ip, port);

  // Select mode
  switch (mode)
    {
    case STANDARD:
      handle_standard(sock);
      break;
      
    case INTERACTIF:
      handle_interactif(sock);
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
