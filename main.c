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
  // Usage
  printf(BOLD "Usage: " RESET "%s [OPTION] [ARGUMENT]...\n", argv[0]);
  printf(BOLD "Options:\n" RESET);
  printf(BOLD "  -h," RESET " print this help\n");

  printf("\n");

  // Server
  printf(BOLD "  SERVER:\n" RESET);
  printf(BOLD "    -s           " RESET " use such server\n");
  printf(BOLD "    -p <port>    " RESET " select a port\n");
  printf(BOLD "    -u <number>  " RESET " select the number of max users in server\n");
  printf(BOLD "    -4           " RESET " use only ipv4\n");
  printf(BOLD "    -6           " RESET " use only ipv6\n");
  printf(BOLD "    -0           " RESET " use ipv4 and ipv6\n");


  printf("\n");

  // Client
  printf(BOLD "  CLIENT:\n" RESET);
  printf(BOLD "    -c           " RESET " use such client\n");
  printf(BOLD "    -o <filename>" RESET " redirect output in the given file\n");
  printf(BOLD "    -n           " RESET " select interactif output (with ncurse)\n");
  printf(BOLD "    -i <ip>      " RESET " select the ip address of server\n");
  printf(BOLD "    -p <port>    " RESET " select the port of server\n");
  printf(BOLD "    -4           " RESET " use only ipv4\n");
  printf(BOLD "    -6           " RESET " use only ipv6\n");
  printf(BOLD "    -0           " RESET " use ipv4 and ipv6\n");
}

void handle_client(int argc, char **argv)
{
  // Declare varible
  int opt = 0;
  int fd = -1;
  int ipv = -1;
  char *ip = NULL;
  char *port = NULL;
  int mode = 0;
  int error = 0;

  // Parse argument
  while ((opt = getopt(argc, argv, "046ni:p:o:")) != -1)
    {
      switch (opt)
        {
        case '0': /* ipv4 and ipv6 */
        case '4': /* ipv4 */
        case '6': /* ipv6 */
          ipv = opt - '0';
          break;
          
        case 'o': /* output */
          // Redirect printf
          fd = open(optarg, O_CREAT | O_WRONLY, 0666);
          dup2(fd, STDOUT_FILENO);
          break;

        case 'i': /* ip */
          ip = optarg;
          break;
          
        case 'p': /* port */
          port = optarg;
          break;
          
        case 'n': /* intercatif */
          mode = 1;
          break;

        default: /* print */
          fprintf(stderr, "For more information, try: %s -h\n", argv[0]);
          exit(EXIT_FAILURE);
          break;
        }
    }

  // Checking argument
  if (ip == NULL)
    {
      fprintf(stderr, "Error: expected option -i with an valid ip address\n");
      error = 1;
    }

  if (port == NULL)
    {
      fprintf(stderr, "Error: expected option -p with a POSITIVE number\n");
      error = 1;
    }

  if (ipv == -1)
    {
      fprintf(stderr, "Error: expected option [-0|-4|-6]\n");
      error = 1;
    }

  if (error)
    {
      exit(EXIT_FAILURE);
    }
  
  // Select mode
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
  int ipv = -1;
  char *port = NULL;
  int max_users = -1;
  int error = 0;

  // Parse argument
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
          fprintf(stderr, "For more information, try: %s -h\n", argv[0]);
          exit(EXIT_FAILURE);
          break;
        }
    }

  // Checking argument
  if (port == NULL)
    {
      fprintf(stderr, "Error: expected option -p with a POSITIVE number\n");
      error = 1;
    }

  if (ipv == -1)
    {
      fprintf(stderr, "Error: expected option [-0|-4|-6]\n");
      error = 1;
    }

  if (max_users == -1)
    {
      fprintf(stderr, "Error: expected option -u with a POSITIVE number\n");
      error = 1;
    }

  if (error)
    {
      exit(EXIT_FAILURE);
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
        fprintf(stderr, "For more information, try: %s -h\n", argv[0]);
        exit(EXIT_FAILURE);
        break;
      }
    }

  // Check if all arg are parse
  if (optind >= argc)
    {
      fprintf(stderr, "Error: needed at least one argument [-h|-s|-c]\n");
      fprintf(stderr, "For more information, try: %s -h\n", argv[0]);
      exit(EXIT_FAILURE);
    }

  //
  return 0;
}
