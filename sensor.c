#include <stdio.h>
#include <string.h> // memset
#include <stdlib.h> // malloc
#include <proc/readproc.h>

#include "sensor.h"

// Get uptime (time since boot)
static double get_uptime(void)
{
  FILE *f = fopen("/proc/uptime", "r");
  if (!f)
    exit(1);

  char buff[64];

  char *ret = fgets(buff, 64, f);
  if (!ret)
    {
      fclose(f);
      exit(1);
    }

  fclose(f);

  return atoll(buff);
}

// Get pourcentage of utilisation of given process
static unsigned get_cpu_usage(proc_t *info)
{
  double total_time = info->utime + info->stime;

  // if we want to add cumulate time of children
  total_time += info->cutime + info->cstime;

  // Get cpu clock
  double hertz = sysconf(_SC_CLK_TCK);

  double uptime = get_uptime();

  double elapsed = uptime - (info->start_time / hertz);

  double cpu_usage = 100 * ((total_time / hertz) / elapsed);

  if (cpu_usage <= 0)
    cpu_usage = 0;

  return (unsigned) (cpu_usage * 100);
}

// Main function for sensor
machine_info_t *sensor(void)
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
  machine_info_t *m = malloc(sizeof(machine_info_t));
  m->proc_info = info;
  while (m->proc_info[count] != NULL)
    {
      m->proc_info[count]->pcpu = get_cpu_usage(m->proc_info[count]);
      count++;
    }
  m->nprocess = count;

  // Return the structure m
  return m;
}

// Main function for display
void free_info(machine_info_t *m)
{
  // Free all proc_t *
  for (int i = 0; m->proc_info[i] != NULL && i < m->nprocess; i++)
    {
      freeproc(m->proc_info[i]);
    }

  // Free struct
  free(m);
}

