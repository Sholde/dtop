#include <stdio.h>

#include "display.h"

void display(proc_info_t *p)
{
  // Test if pointer is not NULL
  if (!p)
    exit(1);

  // Print info
  printf("Process: %ld\n", p->n);
  printf("%5s %10s %10s %5s %5s %5s %10s %10s %8s %s\n", "TID", "USER", "GROUP",
         "PPID", "CPU", "\%CPU", "RES", "VM", "TIME", "COMMAND");

  for (int i = 0; p->info[i] != NULL && i < p->n; i++)
    {
      printf("%5d %10s %10s %5d %5d %5d %10ld %10ld %8lld %s\n", p->info[i]->tid, p->info[i]->ruser,
             p->info[i]->rgroup, p->info[i]->ppid, p->info[i]->processor, p->info[i]->pcpu,
             p->info[i]->rss, p->info[i]->size, p->info[i]->utime, p->info[i]->cmd);
    }
}
