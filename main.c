#include <stdio.h>  // printf
#include <string.h> // memset
#include <stdlib.h> // malloc
#include <proc/readproc.h>

// Structure
typedef struct proc_info_s
{
  size_t n;
  proc_t **info;
} proc_info_t;

// Main function for sensor
proc_info_t *sensor(void)
{
  // Define which information we want
  PROCTAB *tab = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);

  // Counter of running process
  int count = 0;

  // Read all process
  proc_t **info = readproctab(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);

  // Close
  closeproc(tab);

  // Init structure
  proc_info_t *p = malloc(sizeof(proc_info_t));
  p->info = info;
  while (p->info[count] != NULL)
    {
      count++;
    }
  p->n = count;

  // Return the structure p
  return p;
}

// Main function for display
void free_info(proc_info_t *p)
{
  // Free all proc_t *
  for (int i = 0; p->info[i] != NULL && i < p->n; i++)
    {
      freeproc(p->info[i]);
    }

  // Free struct
  free(p);
}

void display(proc_info_t *p)
{
  // Test if pointer is not NULL
  if (!p)
    exit(1);

  // Print info
  printf("Process: %ld\n", p->n);
  printf("%20s: \t%5s\t%5s\t%5s\n", "COMMAND", "PPID", "TID", "RSS");

  for (int i = 0; p->info[i] != NULL && i < p->n; i++)
    {
      printf("%20s: \t%5d\t%5d\t%5ld\n", p->info[i]->cmd, p->info[i]->ppid,
             p->info[i]->tid, p->info[i]->rss);
    }
}

// Main
int main(int argc, char **argv)
{
  //
  proc_info_t *p = sensor();
  display(p);
  free_info(p);

  //
  return 0;
}
