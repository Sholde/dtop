#ifndef _client_h_
#define _client_h_

enum mode_client
  {
    STANDARD,
    INTERACTIF
  };

void client(int ipv, enum mode_client);

#endif
