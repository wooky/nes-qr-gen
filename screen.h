#if !defined(SCREEN_H_)
#define SCREEN_H_

#include "qrcodegen.h"
#include <stdbool.h>

extern uint8_t tempBuffer[];
extern size_t dataLen;
extern uint8_t qrcode[];
extern enum qrcodegen_Ecc ecl;
extern enum qrcodegen_Mask mask;
extern bool boostEcl;

void screen_editor (void);
void screen_qr (void);

#endif // SCREEN_H_
