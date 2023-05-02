#include "neslib.h"
#include "qrcodegen.h"
#include <string.h>

#define NEW_STRING(var, val) unsigned char var[sizeof(val)-1] = val
const NEW_STRING(text, "What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills.");

extern uint8_t tempBuffer[1];
extern size_t dataLen;
extern uint8_t qrcode[1];
extern enum qrcodegen_Ecc ecl;
extern enum qrcodegen_Mask mask;
extern bool boostEcl;

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


void main()
{
  memcpy(tempBuffer, text, sizeof(text));
  dataLen = sizeof(text);
  ecl = qrcodegen_Ecc_LOW;
  mask = qrcodegen_Mask_0;
  boostEcl = false;
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
          data.qr_adr = data.fine_y * data.size;
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
    /* code */
  }
  
}
