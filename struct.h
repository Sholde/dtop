#ifndef _struct_h_
#define _struct_h_

#include <stdlib.h>
#include <proc/readproc.h>

#define PROC_FLAGS (PROC_FILLMEM | PROC_FILLCOM | PROC_FILLENV | PROC_FILLUSR | \
                    PROC_FILLGRP | PROC_FILLSTAT | PROC_FILLSTATUS)

// Structure
typedef struct proc_info_s
{
  size_t n;
  proc_t **info;
} proc_info_t;

#endif
