#include "struct.h"
#include "sensor.h"
#include "display.h"

// Main
int main(int argc, char **argv)
{
  //
  proc_info_t *p = sensor();
  display(p);
  free_info(p);

  //
  return 0;
}
