#ifndef _struct_h_
#define _struct_h_

#include <stdlib.h>
#include <proc/readproc.h>

#define PROC_FLAGS (PROC_FILLMEM | PROC_FILLCOM | PROC_FILLENV | PROC_FILLUSR | \
                    PROC_FILLGRP | PROC_FILLSTAT | PROC_FILLSTATUS |            \
                    PROC_EDITCMDLCVT)

#define MAX_PROCESS 300

// Structure
typedef struct machine_info_s
{
  // Proc
  size_t nprocess;
  size_t nproc;
  proc_t proc_info[MAX_PROCESS];

  // Machine info
  size_t mem_size;
  char name[128];
} machine_info_t;

#endif
