#include <stdarg.h>
#include <xc.h>

#include <hal.h>

void oled_write(uint8_t data);

void oled_cmd(uint8_t cmd);

void oled_send_data(uint8_t data);

void oled_init(void);

void oled_set_pos(uint8_t page, uint8_t col);

void oled_clear(void);

void update_char_mem(char c);

void oled_put_char(char c);

void oled_next_line();

void put_char(char c);
