#include "keyboard.h"

#define KEYBOARD_INPUT (uint8_t*)0x4016
#define KEYBOARD_OUTPUT (uint8_t*)0x4017
#define KEYBOARD_SHIFT '\t'

static const uint8_t keys[9][8] = {
  "][\n\a\a\\\t\a",
  ";:@\a^-/_",
  "klo\a0p,.",
  "jui\a89nm",
  "hgy\a67vb",
  "drt\a45cf",
  "asw\a3ezx",
  "\aq\a\a21\a\t",
  "\a\a\a\a\a\b \a",
};

static struct {
  uint8_t key_pressed;
  uint8_t shift;
  uint8_t key_idx;
  union {
    uint8_t row;
    uint8_t detection;
  };
  uint8_t key_polled;
  uint8_t potential_key_pressed;
  uint8_t col;
} d;

static void _poll (void)
{
  d.key_polled = *KEYBOARD_OUTPUT;
  for (d.col = 0; d.col < 4; ++d.col, d.key_polled <<= 1, ++d.key_idx)
  {
    if ((d.key_polled & 0x10) == 0)
    {
      d.potential_key_pressed = ((uint8_t*)keys)[d.key_idx];
      if (d.potential_key_pressed == KEYBOARD_NO_KEY)
      {
        continue;
      }
      if (d.potential_key_pressed == KEYBOARD_SHIFT)
      {
        d.shift = 1;
      }
      else
      {
        d.key_pressed = d.potential_key_pressed;
      }
    }
  }
}

uint8_t fastcall keyboard_poll (void)
{
  d.key_pressed = KEYBOARD_NO_KEY;
  d.shift = 0;
  d.key_idx = 0;
  *KEYBOARD_INPUT = 0x05;
  for (d.row = 0; d.row < sizeof(keys) / sizeof(keys[0]); ++d.row)
  {
    *KEYBOARD_INPUT = 0x04;
    _poll();
    
    *KEYBOARD_INPUT = 0x06;
    _poll();
  }

  *KEYBOARD_INPUT = 0x04;
  d.detection = *KEYBOARD_OUTPUT & 0x1e;
  if (d.detection != 0x1e)
  {
    return KEYBOARD_NO_KEY;
  }
  
  *KEYBOARD_INPUT = 0x00;
  d.detection = *KEYBOARD_OUTPUT & 0x1e;
  if (d.detection != 0x00)
  {
    return KEYBOARD_NO_KEY;
  }
  
  if (d.shift)
  {
    if (d.key_pressed >= 0x2c && d.key_pressed <= 0x3b)
    {
      d.key_pressed ^= 0x10;
    }
    else if (d.key_pressed >= 'a' && d.key_pressed <= 'z')
    {
      d.key_pressed -= 0x20;
    }
  }
  
  return d.key_pressed;
}
