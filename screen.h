#if !defined(SCREEN_H_)
#define SCREEN_H_

#include "qrcodegen.h"
#include <stdbool.h>

extern uint8_t tempBuffer[];
extern size_t dataLen;
#pragma zpsym ("dataLen")
extern uint8_t qrcode[];
extern enum qrcodegen_Ecc ecl;
#pragma zpsym ("ecl")
extern enum qrcodegen_Mask mask;
#pragma zpsym ("mask")
extern bool boostEcl;
#pragma zpsym ("boostEcl")

void screen_about (void);
void screen_editor (void);
void screen_qr (void);

#endif // SCREEN_H_
