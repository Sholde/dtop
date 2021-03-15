#ifndef _client_h_
#define _client_h_

#include <pthread.h>

#include "struct.h"

enum mode_client
  {
    STANDARD,
    OUTPUT_FILE,
    LOOP
  };

typedef struct
{
  pthread_mutex_t * m;
  pthread_cond_t * c;
  client_info_t content[MAX_CLIENT];
  int deb, row, col;
} arg_t;

typedef struct message_client_s
{
  int deconnect;
  machine_info_t machine;
} message_client_t;
  
  
void client(int, enum mode_client, char *, char *, char *);

#endif
