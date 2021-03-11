#ifndef _client_h_
#define _client_h_

enum mode_client
  {
    STANDARD,
    INTERACTIF
  };

typedef struct message_client_s
{
  int deconnect;
  machine_info_t machine;
} message_client_t;
  
void client(int, enum mode_client, char *, char *);

#endif
