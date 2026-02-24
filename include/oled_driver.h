#include <stdarg.h>

#include <hal.h>

void oled_put_char(char c);

void oled_next_line();

void put_char(char c);

void oled_set_pos(uint8_t page, uint8_t col);

void oled_clear(void);
