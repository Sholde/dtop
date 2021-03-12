#ifndef _display_h_
#define _display_h_

#include "struct.h"

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BOLDRED "\033[1m\033[31m"

void display(machine_info_t *p);

#endif
