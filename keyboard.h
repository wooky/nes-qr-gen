#if !defined(KEYBOARD_H_)
#define KEYBOARD_H_

#include <stdint.h>

#define KEYBOARD_NO_KEY '\000'
#define KEYBOARD_F1 '\001'
#define KEYBOARD_F2 '\002'
#define KEYBOARD_F3 '\003'
#define KEYBOARD_F8 '\004'
#define KEYBOARD_F6 '\006'
#define KEYBOARD_BACKSPACE '\b'

extern uint8_t keyboard_key_pressed;

void fastcall keyboard_init (void);
void fastcall keyboard_poll (void);

#endif // KEYBOARD_H_
