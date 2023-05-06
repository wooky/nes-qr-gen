#include "neslib.h"
#include "screen.h"
#include "keyboard.h"

struct
{
  uint8_t size;
  uint8_t coarse_y, coarse_x;
  
  union
  {
    uint8_t state;
    struct
    {
      uint8_t tile_id, tile_count;
    };
    struct
    {
      uint8_t fine_y;
      uint16_t qr_adr;
      uint8_t pixel_row;
    };
  };
} data;


void screen_qr (void)
{
  vram_adr(0x0000);
  vram_fill(0, NAMETABLE_B);

  // Set upper 16KB PRG to bank 4
  *(unsigned char*)0xe000 = 2 | 0;
  *(unsigned char*)0xe000 = 4 | 0;
  *(unsigned char*)0xe000 = 8 | 1;
  *(unsigned char*)0xe000 = 16 | 0;
  *(unsigned char*)0xe000 = 32 | 0;

  data.state = qrcodegen_encodeBinary();
  if (!data.state)
  {
    pal_col(0, 0x16);
  }
  else
  {
    pal_col(0, 0x30);
    vram_adr(0x0010);
    vram_fill(0xff, 8);
    data.size = qrcodegen_getSize();
    // round to next multiple of 8
    if (data.size != 0)
    {
      data.size = (data.size & ~7) + 8;
    }

    data.tile_id = 1;
    data.tile_count = data.size / 8;
    for (data.coarse_y = 1; data.coarse_y <= data.tile_count; ++data.coarse_y)
    {
      vram_adr(NTADR_A(1, data.coarse_y));
      for (data.coarse_x = 1; data.coarse_x <= data.tile_count; ++data.coarse_x, ++data.tile_id)
      {
        vram_put(data.tile_id);
      }
    }
    
    vram_adr(0x0010);
    for (data.coarse_y = 0; data.coarse_y < data.size; data.coarse_y += 8)
    {
      for (data.coarse_x = 0; data.coarse_x < data.size; data.coarse_x += 8)
      {
        // FIXME this will read out of bounds for the last row of tiles
        for (data.fine_y = data.coarse_y; data.fine_y < data.coarse_y + 8; ++data.fine_y)
        {
          data.qr_adr = data.fine_y << 8;
          data.qr_adr += data.coarse_x;
          data.qr_adr >>= 3;
          ++data.qr_adr;
          data.pixel_row = qrcode[data.qr_adr];
          data.pixel_row = (data.pixel_row & 0xF0) >> 4 | (data.pixel_row & 0x0F) << 4;
          data.pixel_row = (data.pixel_row & 0xCC) >> 2 | (data.pixel_row & 0x33) << 2;
          data.pixel_row = (data.pixel_row & 0xAA) >> 1 | (data.pixel_row & 0x55) << 1;
          vram_put(data.pixel_row);
        }
        vram_fill(0x00, 8);
      }
    }
  }
  
  ppu_on_all();
  while (1)
  {
    if (keyboard_poll() != KEYBOARD_NO_KEY)
    {
      break;
    }
  }
  
  ppu_off();
  screen_editor();
}
