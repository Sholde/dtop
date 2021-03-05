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

void display(proc_info_t *p)
{
  // Test if pointer is not NULL
  if (!p)
    exit(1);

  // Compute info
  char *str_time = NULL;
  
  // Print info
  printf("Process: %ld\n", p->n);
  printf("%5s %10s %10s %5s %5s %5s %10s %10s %8s %s\n", "TID", "USER", "GROUP",
         "PPID", "CPU", "CPU\%", "RES", "VIRT", "TIME", "COMMAND");

  for (int i = 0; p->info[i] != NULL && i < p->n; i++)
    {
      // Transform time
      str_time = get_time(p->info[i]->utime);

      printf("%5d %10s %10s %5d %5d %5.2lf %10ld %10ld %9s %s\n", p->info[i]->tid, p->info[i]->ruser,
             p->info[i]->rgroup, p->info[i]->ppid, p->info[i]->processor, (double)p->info[i]->pcpu / 100,
             p->info[i]->rss, p->info[i]->size, str_time, p->info[i]->cmd);

      // Clean
      free(str_time);
    }

}
