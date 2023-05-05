#if !defined(KEYBOARD_H_)
#define KEYBOARD_H_

#include <stdint.h>

#define KEYBOARD_NO_KEY '\a'
#define KEYBOARD_BACKSPACE '\b'

uint8_t fastcall keyboard_poll (void);

#endif // KEYBOARD_H_
