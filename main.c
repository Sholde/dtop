#include <stdio.h>    // printf
#include <unistd.h>   // getopt
#include <stdlib.h>   // exit
#include <sys/stat.h> // open
#include <fcntl.h>    // open

#include "struct.h"
#include "sensor.h"
#include "display.h"

#define RESET "\033[0m"
#define BOLD "\033[1m"

void print_help(int argc, char **argv)
{
  printf(BOLD "Usage: " RESET "%s [OPTION] filename\n", argv[0]);

  printf(BOLD "    " RESET ", normal case\n");
  printf(BOLD "  -h" RESET ", print this help\n");
  printf(BOLD "  -o" RESET ", redirect the actual state of all cpu in the given file\n");
}

// Main
int main(int argc, char **argv)
{
  // Declare varible
  proc_info_t *p = NULL;
  int fd = -1;
  
  // Check argument with getopt
  int opt = 0;
  while ((opt = getopt(argc, argv, "o:h")) != -1)
    {
    switch (opt)
      {
      case 'h': /* help */

        print_help(argc, argv);
        exit(EXIT_SUCCESS);
        
        break;
      case 'o': /* output */

        // Redirect printf
        fd = open(optarg, O_CREAT | O_WRONLY, 0666);
        dup2(fd, STDOUT_FILENO);

        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [OPTION] filename\n", argv[0]);
        exit(EXIT_FAILURE);
      }
    }

  // Program
  p = sensor();
  display(p);
  free_info(p);

  // Close file descriptor
  if (fd != -1)
    close(fd);
        
  //
  return 0;
}
