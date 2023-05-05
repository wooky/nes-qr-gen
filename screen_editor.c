#include "keyboard.h"
#include "build/chr.h"
#include "screen.h"

#define CHR_CURSOR 0x7f

static const char palette[] = {
  0x0f, 0x0f, 0x0f, 0x30,
};
static const uint8_t status_bar_nametable[32 * 3 + 1] =
  "F1 ECL L F2 MASK 0 F3 bECL F    "
  "F8 RUN PAGE 00 CHAR 0000/0000   "
  "________________________________"
  "\x7f"
  ;
static const uint8_t ecl_values[4] = "LMQH";
static const uint8_t bool_values[2] = "FT";

static uint8_t key, last_key;
static uint8_t vram_buf[8];
static uint16_t vram_ptr;
static uint8_t key_debounce;
static uint8_t *buf_ptr;

void main (void)
{
  screen_editor();
}

void screen_editor (void)
{
  pal_bg(palette);
  set_vram_update(vram_buf);
  chr_data_ascii_vram_write();

  vram_adr(NTADR_A(0, 0));
  vram_write(status_bar_nametable, sizeof(status_bar_nametable));
  vram_fill(0, NAMETABLE_B - NAMETABLE_A - sizeof(status_bar_nametable));

  vram_ptr = NTADR_A(0, 3);
  vram_buf[0] = NT_UPD_EOF;

  buf_ptr = tempBuffer;
  ecl = qrcodegen_Ecc_LOW;
  mask = qrcodegen_Mask_0;
  boostEcl = false;

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
        #define ECL_VRAM NTADR_A(7, 0)
        vram_buf[0] = MSB(ECL_VRAM);
        vram_buf[1] = LSB(ECL_VRAM);
        vram_buf[2] = ecl_values[ecl];
        vram_buf[3] = NT_UPD_EOF;
        break;

      case KEYBOARD_F2:
        #define MASK_VRAM NTADR_A(17, 0)
        vram_buf[0] = MSB(MASK_VRAM);
        vram_buf[1] = LSB(MASK_VRAM);
        vram_buf[3] = NT_UPD_EOF;

        ++mask;
        if (mask > qrcodegen_Mask_7)
        {
          mask = qrcodegen_Mask_AUTO;
          vram_buf[2] = 'A';
        }
        else
        {
          vram_buf[2] = mask + '0';
        }
        break;

      case KEYBOARD_F3:
        boostEcl ^= 1;
        #define BECL_VRAM NTADR_A(27, 0)
        vram_buf[0] = MSB(BECL_VRAM);
        vram_buf[1] = LSB(BECL_VRAM);
        vram_buf[2] = bool_values[boostEcl];
        vram_buf[3] = NT_UPD_EOF;
        break;

      case KEYBOARD_F8:
        goto generate_qr;

      case KEYBOARD_BACKSPACE:
        --vram_ptr;
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = CHR_CURSOR;
        vram_buf[4] = ' ';
        vram_buf[5] = NT_UPD_EOF;

        --buf_ptr;
        break;
      
      default:
        vram_buf[0] = MSB(vram_ptr) | NT_UPD_HORZ;
        vram_buf[1] = LSB(vram_ptr);
        vram_buf[2] = 2;
        vram_buf[3] = key;
        vram_buf[4] = CHR_CURSOR;
        vram_buf[5] = NT_UPD_EOF;
        ++vram_ptr;

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
