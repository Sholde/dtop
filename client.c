#include <stdio.h>  // printf
#include <stdlib.h> // NULL
#include <string.h> // memset
#include <unistd.h> // close

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "struct.h"
#include "sensor.h"
#include "display.h"
#include "server.h"
#include "client.h"

static void handle_standard()
{
  proc_info_t *p = NULL;

  p = sensor();
  display(p);
  free_info(p);
}

static void handle_interactif()
{
  fprintf(stderr, "Not yet implemented!\n");
}

void client(int ipv, enum mode_client mode)
{
  //
  if (mode != INTERACTIF && mode != STANDARD)
    {
      fprintf(stderr, "Error: bad mode\n");
      exit(EXIT_FAILURE);
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
      fprintf(stderr, "Failed to connect on 0.0.0.0:%s\n", SERVER_PORT);
      exit(EXIT_FAILURE);
    }

  //
  switch (mode)
    {
    case STANDARD:
      handle_standard();
      break;
      
    case INTERACTIF:
      handle_interactif();
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
