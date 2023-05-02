#include "neslib.h"
#include "qrcodegen.h"
#include <string.h>

#define NEW_STRING(var, val) unsigned char var[sizeof(val)-1] = val
#define MAX_VERSION 5
#define BUFFER_SIZE qrcodegen_BUFFER_LEN_FOR_VERSION(MAX_VERSION)

const NEW_STRING(text, "Wazzup, Beijing!?");
uint8_t qr[BUFFER_SIZE];
uint8_t dataAndTemp[BUFFER_SIZE];
uint8_t state;

void main()
{
  memcpy(dataAndTemp, text, sizeof(text));
  state = 0xff;
  state = qrcodegen_encodeBinary(dataAndTemp, sizeof(text), qr, qrcodegen_Ecc_LOW, 1, MAX_VERSION, qrcodegen_Mask_0, false);
  if (!state)
  {
    pal_col(0, 0x16);
  }
  else
  {
    int size, x, y;
    pal_col(0, 0x30);
    vram_adr(0x0010);
    vram_fill(0xff, 8);
    size = qrcodegen_getSize(qr);
    for (y = 0; y < size; ++y)
    {
      vram_adr(NTADR_A(1, y + 1));
      for (x = 0; x < size; ++x)
      {
        qrcodegen_getModule(qr, x, y) ? vram_put(1) : vram_put(0);
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
