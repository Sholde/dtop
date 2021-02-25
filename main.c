#include <stdio.h>  // printf
#include <string.h> // memset
#include <stdlib.h> // malloc
#include <proc/readproc.h>

typedef struct proc_info_s
{
  proc_t info;
  struct proc_info_s *next;
} proc_info_t;

proc_info_t *sensor(void)
{
  proc_info_t *p = NULL;
  
  // Define which information we want
  PROCTAB *tab = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);

  // Buffer for proc information
  proc_t proc_info;

  // Clean the buffer
  memset(&proc_info, 0, sizeof(proc_info));

  // Counter of running process
  int count = 0;

  if (readproc(tab, &proc_info) != NULL)
    {
      p = malloc(sizeof(proc_info_t));
      p->info = proc_info;
      p->next = NULL;
      proc_info_t *tmp = p;
      
      while (readproc(tab, &proc_info) != NULL)
        {
          // Create node
          tmp->next = malloc(sizeof(proc_info_t));
          tmp = tmp->next;

          // Fill node
          tmp->info = proc_info;
          tmp->next = NULL;

          // Inc counter
          count++;
        }
    }

  // Close
  closeproc(tab);

  return p;
}

void free_info(proc_info_t *p)
{
  proc_info_t *tmp = p;
  
  while (tmp != NULL)
    {
      p = tmp;
      tmp = tmp->next;
      free(p);
    }
}

void display(proc_info_t *p)
{
  if (!p)
    exit(1);

  // Counter of running process
  int count = 0;

  // Print info
  printf("%20s: \t%5s\t%5s\t%5s\n", "COMMAND", "PPID", "TID", "RSS");

  proc_info_t *tmp = p;
  
  while (tmp != NULL)
    {
      printf("%20s: \t%5d\t%5d\t%5ld\n", tmp->info.cmd, tmp->info.ppid,
             tmp->info.tid, tmp->info.rss);
      count++;
      tmp = tmp->next;
    }

  printf("Process: %d\n", count);
  
}

int main(int argc, char **argv)
{
  proc_info_t *p = sensor();
  display(p);
  free_info(p);
  
  return 0;
}
