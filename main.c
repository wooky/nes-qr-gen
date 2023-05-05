#include "neslib.h"
#include "qrcodegen.h"
#include <string.h>

#define NEW_STRING(var, val) unsigned char var[sizeof(val)-1] = val
const NEW_STRING(text, "What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. You are nothing to me but just another target. I will wipe you the fuck out with precision the likes of which has never been seen before on this Earth, mark my fucking words. You think you can get away with saying that shit to me over the Internet? Think again, fucker. As we speak I am contacting my secret network of spies across the USA and your IP is being traced right now so you better prepare for the storm, maggot. The storm that wipes out the pathetic little thing you call your life. You're fucking dead, kid. I can be anywhere, anytime, and I can kill you in over seven hundred ways, and that's just with my bare hands. Not only am I extensively trained in unarmed combat, but I have access to the entire arsenal of the United States Marine Corps and I will use it to its full extent to wipe your miserable ass off the face of the continent, you little shit. If only you could have known what unholy retribution your little \"clever\" comment was about to bring down upon you, maybe you would have held your fucking tongue.");

extern uint8_t tempBuffer[];
extern size_t dataLen;
extern uint8_t qrcode[];
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
  // Set upper 16KB PRG to bank 4
  *(unsigned char*)0xe000 = 2 | 0;
  *(unsigned char*)0xe000 = 4 | 0;
  *(unsigned char*)0xe000 = 8 | 1;
  *(unsigned char*)0xe000 = 16 | 0;
  *(unsigned char*)0xe000 = 32 | 0;

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
    /* code */
  }
  
}
