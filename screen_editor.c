#include "keyboard.h"
#include "build/chr.h"

#define CHR_CURSOR 0x7f

static char palette[] = {
  0x0f, 0x0f, 0x0f, 0x30,
};

static uint8_t key, last_key;
static uint8_t vram_buf[8];
static uint16_t vram_ptr;
static uint8_t key_debounce;

void main (void)
{
  pal_bg(palette);
  set_vram_update(vram_buf);
  chr_data_ascii_vram_write();

  vram_ptr = NTADR_A(0, 0);
  vram_buf[0] = NT_UPD_EOF;
  ppu_on_all();

  while (1)
  {
    key = keyboard_poll();
    if (key != last_key || key_debounce == 12)
    {
      if (key == KEYBOARD_BACKSPACE)
      {
        --vram_ptr;
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = CHR_CURSOR;
        vram_buf[4] = ' ';
        vram_buf[5] = NT_UPD_EOF;
      }
      else if (key != KEYBOARD_NO_KEY)
      {
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = key;
        vram_buf[4] = CHR_CURSOR;
        vram_buf[5] = NT_UPD_EOF;
        ++vram_ptr;
      }
      key_debounce = 0;
    }
    else
    {
      ++key_debounce;
    }
    last_key = key;

    ppu_wait_nmi();
  }
}
