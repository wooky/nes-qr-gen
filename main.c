#include "neslib.h"
#include "qrcodegen.h"
#include <string.h>

#define NEW_STRING(var, val) unsigned char var[sizeof(val)-1] = val
#define MAX_VERSION 12
#define BUFFER_SIZE qrcodegen_BUFFER_LEN_FOR_VERSION(MAX_VERSION)

const NEW_STRING(text, "What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. You are nothing to me but just another target. I");

struct
{
  uint8_t qr[BUFFER_SIZE];
  uint8_t dataAndTemp[BUFFER_SIZE];
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
      uint8_t fine_y, fine_x;
      uint8_t pixel_row;
    };
  };
} data;


void main()
{
  memcpy(data.dataAndTemp, text, sizeof(text));
  data.state = qrcodegen_encodeBinary(data.dataAndTemp, sizeof(text), data.qr, qrcodegen_Ecc_LOW, 1, MAX_VERSION, qrcodegen_Mask_0, false);
  if (!data.state)
  {
    pal_col(0, 0x16);
  }
  else
  {
    pal_col(0, 0x30);
    vram_adr(0x0010);
    vram_fill(0xff, 8);
    data.size = qrcodegen_getSize(data.qr);
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
        for (data.fine_y = data.coarse_y; data.fine_y < data.coarse_y + 8; ++data.fine_y)
        {
          data.pixel_row = 0;
          for (data.fine_x = data.coarse_x; data.fine_x < data.coarse_x + 8; ++data.fine_x)
          {
            data.pixel_row <<= 1;
            data.pixel_row |= qrcodegen_getModule(data.qr, data.fine_x, data.fine_y);
          }
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

// idfk
size_t __fastcall__ write () { return 0; }