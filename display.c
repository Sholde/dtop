#include <stdio.h>

#include "display.h"

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
