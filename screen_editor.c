#include "keyboard.h"
#include "build/chr.h"
#include "screen.h"
#include <string.h>

#define CHR_CURSOR 0x7f

static const char palette[] = {
  0x0f, 0x0f, 0x0f, 0x30,
};
static const uint8_t status_bar_nametable[32 * 3 + 1] =
  "F1 ECL ? F2 MASK ? F3 bECL ?    "
  "F8 RUN PAGE 1 CHAR 0000/????    "
  "________________________________"
  "\x7f"
  ;
static const uint8_t ecl_values[4] = "LMQH";
static const uint8_t bool_values[2] = "FT";
static const uint8_t max_text_size[4][4] = {
  "1465",
  "1125",
  "0805",
  "0625",
};

#define ECL_VRAM NTADR_A(7, 0)
#define MASK_VRAM NTADR_A(17, 0)
#define BECL_VRAM NTADR_A(27, 0)
#define TEXT_SIZE_VRAM NTADR_A(19, 1)
#define MAX_TEXT_SIZE_VRAM NTADR_A(24, 1)

static uint8_t key, last_key;
static uint8_t vram_buf[16];
static uint16_t vram_ptr;
static uint8_t key_debounce;
static uint8_t *buf_ptr;
static char text_size[4];
static char *text_size_ptr;

void main (void)
{
  ecl = qrcodegen_Ecc_LOW;
  mask = qrcodegen_Mask_0;
  boostEcl = false;
  keyboard_init();

  screen_editor();
}

uint8_t _mask_char (uint8_t delta)
{
  mask += delta;
  if (mask > qrcodegen_Mask_7)
  {
    mask = qrcodegen_Mask_AUTO;
  }
  return (mask == qrcodegen_Mask_AUTO) ? 'A' : mask + '0';
}

void screen_editor (void)
{
  pal_bg(palette);
  set_vram_update(vram_buf);
  chr_data_ascii_vram_write();

  vram_adr(NTADR_A(0, 0));
  vram_write(status_bar_nametable, sizeof(status_bar_nametable));
  vram_adr(ECL_VRAM);
  vram_put(ecl_values[ecl]);
  vram_adr(MASK_VRAM);
  vram_put(_mask_char(0));
  vram_adr(BECL_VRAM);
  vram_put(bool_values[boostEcl]);
  vram_adr(MAX_TEXT_SIZE_VRAM);
  vram_write(max_text_size[ecl], sizeof(max_text_size[0]));
  vram_adr(NTADR_A(0, 0) + sizeof(status_bar_nametable));
  vram_fill(0, NAMETABLE_B - NAMETABLE_A - sizeof(status_bar_nametable));

  vram_ptr = NTADR_A(0, 3);
  vram_buf[0] = NT_UPD_EOF;

  buf_ptr = tempBuffer;
  memfill(text_size, '0', sizeof(text_size));

  ppu_on_all();

  while (1)
  {
    key = keyboard_poll();
    if (key != last_key || key_debounce == 12)
    {
      switch (key)
      {
      case KEYBOARD_NO_KEY:
        break;

      case KEYBOARD_F1:
        ++ecl;
        if (ecl == sizeof(ecl_values))
        {
          ecl = 0;
        }
        
        vram_buf[0] = MSB(ECL_VRAM);
        vram_buf[1] = LSB(ECL_VRAM);
        vram_buf[2] = ecl_values[ecl];
        vram_buf[3] = MSB(MAX_TEXT_SIZE_VRAM) | NT_UPD_HORZ;
        vram_buf[4] = LSB(MAX_TEXT_SIZE_VRAM);
        vram_buf[5] = sizeof(max_text_size[0]);
        memcpy(&vram_buf[6], max_text_size[ecl], sizeof(max_text_size[0]));
        vram_buf[6 + sizeof(max_text_size[0])] = NT_UPD_EOF;
        break;

      case KEYBOARD_F2:
        vram_buf[0] = MSB(MASK_VRAM);
        vram_buf[1] = LSB(MASK_VRAM);
        vram_buf[2] = _mask_char(1);
        vram_buf[3] = NT_UPD_EOF;
        break;

      case KEYBOARD_F3:
        boostEcl ^= 1;
        vram_buf[0] = MSB(BECL_VRAM);
        vram_buf[1] = LSB(BECL_VRAM);
        vram_buf[2] = bool_values[boostEcl];
        vram_buf[3] = NT_UPD_EOF;
        break;

      case KEYBOARD_F8:
        goto generate_qr;

      case KEYBOARD_BACKSPACE:
        if (buf_ptr == tempBuffer)
        {
          break;
        }
        
        --vram_ptr;
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = CHR_CURSOR;
        vram_buf[4] = ' ';

        for (text_size_ptr = &text_size[3]; ; --text_size_ptr)
        {
          if (*text_size_ptr != '0')
          {
            --*text_size_ptr;
            break;
          }
          *text_size_ptr = '9';
        }
        vram_buf[5] = MSB(TEXT_SIZE_VRAM) | NT_UPD_HORZ;
        vram_buf[6] = LSB(TEXT_SIZE_VRAM);
        vram_buf[7] = 4;
        memcpy(&vram_buf[8], text_size, sizeof(text_size));
        vram_buf[8 + sizeof(text_size)] = NT_UPD_EOF;

        --buf_ptr;
        break;
      
      default:
        if (strcmp(text_size, (const char*)max_text_size[ecl]) >= 0)
        {
          break;
        }
        
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = key;
        vram_buf[4] = CHR_CURSOR;
        ++vram_ptr;

        for (text_size_ptr = &text_size[3]; ; --text_size_ptr)
        {
          if (*text_size_ptr != '9')
          {
            ++*text_size_ptr;
            break;
          }
          *text_size_ptr = '0';
        }
        vram_buf[5] = MSB(TEXT_SIZE_VRAM) | NT_UPD_HORZ;
        vram_buf[6] = LSB(TEXT_SIZE_VRAM);
        vram_buf[7] = 4;
        memcpy(&vram_buf[8], text_size, sizeof(text_size));
        vram_buf[8 + sizeof(text_size)] = NT_UPD_EOF;

        *buf_ptr = key;
        ++buf_ptr;
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

generate_qr:
  ppu_off();
  set_vram_update(NULL);
  dataLen = buf_ptr - tempBuffer;
  screen_qr();
}
