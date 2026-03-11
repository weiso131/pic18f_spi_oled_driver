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
    /* | 6 bits NOP | 1 bits full line | 1 bits can't print | */
    unsigned char start_page;
} oled_control_t;

#define STATUS_CANT_PRINT 0x1
#define STATUS_FULL_LINE 0x2

#define STATUS_RESET(status) (0xFF ^ status)

void oled_put_char(oled_control_t *self, char c);

void oled_next_line(oled_control_t *self);

void oled_prev_line(oled_control_t *self);

void put_char(oled_control_t *self);

void oled_control_init(oled_control_t *self);

void oled_set_pos(oled_control_t *self, uint8_t page, uint8_t col);

void oled_clear(oled_control_t *self);
