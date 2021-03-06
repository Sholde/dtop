#include <stdio.h>

#include "display.h"

static char *get_time(unsigned long long t)
{
  char *str_time = malloc(sizeof(char) * 32);
  str_time[31] = '\0';

  int sec = t % 60;
  int min = t / 60;
  int hou = 0;

  if (min >= 60)
    {
      hou = min / 60;
      min = min % 60;
    }

  sprintf(str_time, "%02d:%02d:%02d", hou, min, sec);
  
  return str_time;
}

void display(machine_info_t *m)
{
  // Test if pointer is not NULL
  if (!m)
    exit(1);

  // Compute info
  char *str_time = NULL;
  
  // Print info
  printf("Process: %ld\n", m->nprocess);
  printf("%5s %10s %10s %5s %5s %5s %10s %10s %8s %s\n", "TID", "USER", "GROUP",
         "PPID", "CPU", "CPU\%", "RES", "VIRT", "TIME", "COMMAND");

  for (int i = 0; m->proc_info[i] != NULL && i < m->nprocess; i++)
    {
      // Transform time
      str_time = get_time(m->proc_info[i]->utime);

      printf("%5d %10s %10s %5d %5d %5.2lf %10ld %10ld %9s %s\n", m->proc_info[i]->tid, m->proc_info[i]->ruser,
             m->proc_info[i]->rgroup, m->proc_info[i]->ppid, m->proc_info[i]->processor, (double)m->proc_info[i]->pcpu / 100,
             m->proc_info[i]->rss, m->proc_info[i]->size, str_time, m->proc_info[i]->cmd);

      // Clean
      free(str_time);
    }

}
