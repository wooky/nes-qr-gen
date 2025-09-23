#include "keyboard.h"
#include "screen.h"
#include "neslib.h"

#define xstr(s) str(s)
#define str(s) #s

static const uint8_t about_nametable1[] = "NES QR Code Generator";
static const uint8_t about_nametable2[] =
  " Copyright 2024 Yakov Lipkovich "
  " Licensed under MIT";
static const uint8_t about_nametable3[] =
  " If you need instructions on how"
  " to use this software, check out"
  " the enclosed instruction book.";
static const uint8_t about_nametable4[] = "Press any key to return";
static const uint8_t about_nametable5[] = "Version: " xstr(NES_QR_VERSION);

#define ABOUT1_VRAM NTADR_A(6, 4)
#define ABOUT2_VRAM NTADR_A(0, 8)
#define ABOUT3_VRAM NTADR_A(0, 12)
#define ABOUT4_VRAM NTADR_A(5, 24)
#define ABOUT5_VRAM NTADR_A(1, 28)

void screen_about (void)
{
  // No need to initialize graphics, already done in screen_editor()

#define TOPLEFT NTADR_A(0, 0)
#define PAD_WRITE(nt, end, start, size) \
  vram_fill(0, end - start - size + 1); \
  vram_write(nt, sizeof(nt) - 1);

  vram_adr(TOPLEFT);
  PAD_WRITE(about_nametable1, ABOUT1_VRAM, TOPLEFT, 1)
  PAD_WRITE(about_nametable2, ABOUT2_VRAM, ABOUT1_VRAM, sizeof(about_nametable1))
  PAD_WRITE(about_nametable3, ABOUT3_VRAM, ABOUT2_VRAM, sizeof(about_nametable2))
  PAD_WRITE(about_nametable4, ABOUT4_VRAM, ABOUT3_VRAM, sizeof(about_nametable3))
  PAD_WRITE(about_nametable5, ABOUT5_VRAM, ABOUT4_VRAM, sizeof(about_nametable4))

  ppu_on_all();
  while (1)
  {
    keyboard_poll();
    if (keyboard_key_pressed != KEYBOARD_NO_KEY)
    {
      break;
    }
    ppu_wait_nmi();
  }

  ppu_off();
  screen_editor();
}
