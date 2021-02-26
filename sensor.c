#include <string.h> // memset
#include <stdlib.h> // malloc
#include <proc/readproc.h>

#include "sensor.h"

// Main function for sensor
proc_info_t *sensor(void)
{
  // Define which information we want
  PROCTAB *tab = openproc(PROC_FLAGS);

  // Counter of running process
  int count = 0;

  // Read all process
  proc_t **info = readproctab(PROC_FLAGS);

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

