#include <char_mem.h>
#include <hal.h>
#include <stdarg.h>

typedef union {
    struct {
        unsigned x : 4;
        unsigned y : 3;
        unsigned dirty : 1;
    };
} oled_pos_t;

typedef struct oled_control {
    oled_pos_t oled_pos;
    char_mem_t char_mem;
    unsigned int oled_show_start;
    unsigned int oled_show_end;
    char oled_status;
    unsigned char start_page;
    /* | 5 bits NOP | 1 bits newline disable | 1 bits memory out save | 1 bits
     * memory out | */
} oled_control_t;

void oled_put_char(oled_control_t *self, char c);

void oled_next_line(oled_control_t *self);

void oled_prev_line(oled_control_t *self);

void put_char(oled_control_t *self);

void oled_control_init(oled_control_t *self);

void oled_set_pos(oled_control_t *self, uint8_t page, uint8_t col);

void oled_clear(oled_control_t *self);
