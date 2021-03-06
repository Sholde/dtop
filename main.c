#include <stdio.h>    // printf
#include <unistd.h>   // getopt
#include <stdlib.h>   // exit
#include <sys/stat.h> // open
#include <fcntl.h>    // open
#include <string.h>   // string

#include "server.h"
#include "client.h"

#define RESET "\033[0m"
#define BOLD "\033[1m"

void print_help(int argc, char **argv)
{
  printf(BOLD "Usage: " RESET "%s [OPTION] filename...\n", argv[0]);

  printf(BOLD "  -h" RESET ", print this help\n");
  printf(BOLD "  -c" RESET ", use such client\n");
  printf(BOLD "  -s" RESET ", use such server\n");
  printf(BOLD "  -o" RESET ", redirect the actual state of all cpu in the given file\n");
  printf(BOLD "  -i" RESET ", select the ip address of server\n");
  printf(BOLD "  -p" RESET ", select a port\n");
  printf(BOLD "  -4" RESET ", use only ipv4\n");
  printf(BOLD "  -6" RESET ", use only ipv6\n");
  printf(BOLD "  -0" RESET ", use ipv4 and ipv6\n");
  printf(BOLD "  -n" RESET ", select ncurses output\n");
  printf(BOLD "  -u" RESET ", select the numbe of max users in server\n");
}

void handle_client(int argc, char **argv)
{
  // Declare varible
  int fd = -1;
  int opt = 0;
  int ipv = 0;
  char *ip = NULL;
  char *port = NULL;
  int mode = 0;
  
  while ((opt = getopt(argc, argv, "046ni:p:o:")) != -1)
    {
      switch (opt)
        {
        case '0':
        case '4':
        case '6':
          ipv = opt - '0';
        case 'o': /* output */

          // Redirect printf
          fd = open(optarg, O_CREAT | O_WRONLY, 0666);
          dup2(fd, STDOUT_FILENO);

        case 'i': /* ip */
          ip = optarg;
          break;
          
        case 'p': /* port */
          port = optarg;
          break;
          
        case 'n': /* intercatif */
          mode = 1;
          break;

        case '?': /* '?' */
          fprintf(stderr, "Usage: %s [OPTION] filename\n", argv[0]);
          exit(EXIT_FAILURE);
          break;

        default: /* print */
          mode = 0;
          break;
        }
    }

  if (mode)
    {
      client(ipv, INTERACTIF, ip, port);
    }
  else
    {
      client(ipv, STANDARD, ip, port);
    }


  // Close file descriptor
  if (fd != -1)
    close(fd);
}

void handle_server(int argc, char **argv)
{
  // Declare variable
  int opt = 0;
  int ipv = 0;
  char *port = NULL;
  int max_users = -1;
  
  while ((opt = getopt(argc, argv, "046p:u:")) != -1)
    {
      switch (opt)
        {
        case '0': /* ipv4 and ipv6 */
        case '4': /* ipv4 */
        case '6': /* ipv6 */
          ipv = opt - '0';
          break;

        case 'p': /* port */
          port = optarg;
          break;

        case 'u': /* users */
          max_users = atoi(optarg);
          break;

        default: /* '?' */
          fprintf(stderr, "Error: bad argument, needed [-0|-4|-6]\n");
          exit(EXIT_FAILURE);
          break;
        }
    }

  server(ipv, port, max_users);
}

// Main
int main(int argc, char **argv)
{
  // Check argument with getopt
  int opt = 0;
  
  while ((opt = getopt(argc, argv, "hsc")) != -1)
    {
    switch (opt)
      {
      case 'h': /* help */
        print_help(argc, argv);
        exit(EXIT_SUCCESS);
        break;

      case 'c': /* client */
        handle_client(argc, argv);
        break;
        
      case 's': /* server */
        handle_server(argc, argv);
        break;

      default: /* '?' */
        fprintf(stderr, "Error: bad argument, needed [-h|-s|-c]\n");
        exit(EXIT_FAILURE);
        break;
      }
    }

  //
  return 0;
}
