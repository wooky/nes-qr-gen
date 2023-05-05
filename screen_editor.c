#include "keyboard.h"
#include "neslib.h"

static uint8_t key_tmp, key;

void main (void)
{
  while (1)
  {
    key_tmp = keyboard_poll();
    if (key_tmp != KEYBOARD_NO_KEY)
    {
      key = key_tmp;
    }
  }
}
