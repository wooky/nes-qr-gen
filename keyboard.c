#include "keyboard.h"

#define KEYBOARD_INPUT (uint8_t*)0x4016
#define KEYBOARD_OUTPUT (uint8_t*)0x4017
#define KEYBOARD_SHIFT '\t'
#define KEYBOARD_DEBUG '\005'

static const uint8_t keys[9][8] = {
  "][\n\004\000\\\t\000",
  ";:@\000^-/_",
  "klo\0060p,.",
  "jui\00089nm",
  "hgy\00067vb",
  "drt\00345cf",
  "asw\0023ezx",
  "\000q\000\00121\000\t",
  "\000\000\000\000\005\b \000",
};

uint8_t keyboard_key_pressed;
static uint8_t debug;
static uint8_t last_key_pressed;
static uint8_t debounce_timer;

static struct {
  uint8_t shift;
  union {
    struct {
      uint8_t row;
      uint8_t key_idx;
      uint8_t key_polled;
      uint8_t potential_key_pressed;
      uint8_t col;
    };
    uint8_t detection;
  };
} d;

#define RETURN_KEY_PRESS(k) { keyboard_key_pressed = k; return; }

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
        keyboard_key_pressed = d.potential_key_pressed;
      }
    }
  }
}

void fastcall keyboard_init (void)
{
  debug = 0;
}

void fastcall keyboard_poll (void)
{
  keyboard_key_pressed = KEYBOARD_NO_KEY;
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

  if (debug)
  {
    if (keyboard_key_pressed != KEYBOARD_NO_KEY)
    {
      debug = 0;
    }
    else
    {
      debug ^= 0x20;
    }
    RETURN_KEY_PRESS(debug);
  }

  *KEYBOARD_INPUT = 0x04;
  d.detection = *KEYBOARD_OUTPUT & 0x1e;
  if (d.detection != 0x1e)
  {
    RETURN_KEY_PRESS(KEYBOARD_NO_KEY);
  }
  
  *KEYBOARD_INPUT = 0x00;
  d.detection = *KEYBOARD_OUTPUT & 0x1e;
  if (d.detection != 0x00)
  {
    RETURN_KEY_PRESS(KEYBOARD_NO_KEY);
  }

  if (keyboard_key_pressed == KEYBOARD_DEBUG)
  {
    debug = 'X';
    RETURN_KEY_PRESS('X');
  }
  
  if (d.shift)
  {
    if (keyboard_key_pressed >= 0x2c && keyboard_key_pressed <= 0x3b)
    {
      keyboard_key_pressed ^= 0x10;
    }
    else if (keyboard_key_pressed >= 'a' && keyboard_key_pressed <= 'z')
    {
      keyboard_key_pressed ^= 0x20;
    }
  }

  if (keyboard_key_pressed == last_key_pressed && debounce_timer != 12)
  {
    ++debounce_timer;
    RETURN_KEY_PRESS(KEYBOARD_NO_KEY);
  }
  
  last_key_pressed = keyboard_key_pressed;
  debounce_timer = 0;
}
