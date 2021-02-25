#include <stdio.h>  // printf
#include <string.h> // memset
#include <proc/readproc.h>

int main(int argc, char **argv)
{
  // Define which information we want
  PROCTAB *tab = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);

  // Buffer for proc information
  proc_t proc_info;

  // Clean the buffer
  memset(&proc_info, 0, sizeof(proc_info));

  // Counter of running process
  int count = 0;

  // Print info
  printf("%20s: \t%5s\t%5s\t%5s\n", "COMMAND", "PPID", "TID", "RSS");
  
  while (readproc(tab, &proc_info) != NULL)
    {
      printf("%20s: \t%5d\t%5d\t%5ld\n", proc_info.cmd, proc_info.ppid,
             proc_info.tid, proc_info.rss);
      count++;
    }

  printf("Process: %d\n", count);

  closeproc(tab);

  return 0;
}
