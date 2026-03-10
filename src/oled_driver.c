#include <font_8x8.h>
#include <hal.h>
#include <oled_driver.h>

unsigned char i;

// Set position
void oled_set_pos(oled_control_t *self, uint8_t page, uint8_t col)
{
    col += 2;
    oled_cmd(0xB0 + ((page + self->start_page) & 0x7));
    oled_cmd(0x00 | (col & 0x0F));
    oled_cmd(0x10 | (col >> 4));
}

// Clear screen
void oled_clear(oled_control_t *self)
{
    for (uint8_t p = 0; p < 8; p++) {
        oled_set_pos(self, p, 0);
        for (uint8_t c = 0; c < 128; c++)
            oled_send_data(0x00);
    }
}

void oled_put_char(oled_control_t *self, char _c)
{
#define c i
    self->oled_pos.dirty = INTCONbits.GIE;
    INTCONbits.GIE = 1;
    c = _c & 0x7F;

    oled_send_data(font_8x8[c - 0x20][0]);
    oled_send_data(font_8x8[c - 0x20][1]);
    oled_send_data(font_8x8[c - 0x20][2]);
    oled_send_data(font_8x8[c - 0x20][3]);
    oled_send_data(font_8x8[c - 0x20][4]);
    oled_send_data(font_8x8[c - 0x20][5]);
    oled_send_data(font_8x8[c - 0x20][6]);
    oled_send_data(font_8x8[c - 0x20][7]);

    INTCONbits.GIE = 0;

    if (_c & 0x80) {
        if (self->oled_pos.y == 7) {
            self->oled_status |= 1;
            return;
        } else {
            self->oled_pos.y++;
            self->oled_pos.x = 0;
        }
    } else
        self->oled_pos.x++;
    oled_set_pos(self, self->oled_pos.y, self->oled_pos.x * 8);

    INTCONbits.GIE = self->oled_pos.dirty;
#undef c
}

void oled_next_line(oled_control_t *self)
{
    if (self->oled_show_end == self->char_mem.end_ptr)
        return;
    self->start_page = (self->start_page + 1) & 0x7;
    oled_cmd(0x40 | (self->start_page * 8));

    for (i = 0; i < 16; i++,
        self->oled_show_start = (self->oled_show_start + 1) % CHAR_MEMORY_NUM) {
        if (char_mem_is_new_line(self->char_mem, self->oled_show_start)) {
            self->oled_show_start =
                (self->oled_show_start + 1) % CHAR_MEMORY_NUM;
            break;
        }
    }

    oled_set_pos(self, 7, 0);

    for (i = 0; i < 16 && self->char_mem.mem[self->oled_show_end]; i++) {
        oled_put_char(self,
                      remove_newline_mark(self->char_mem, self->oled_show_end));
        self->oled_show_end = (self->oled_show_end + 1) % CHAR_MEMORY_NUM;
        if (char_mem_is_new_line(self->char_mem, self->oled_show_end)) {
            oled_put_char(
                self, remove_newline_mark(self->char_mem, self->oled_show_end));
            self->oled_show_end = (self->oled_show_end + 1) % CHAR_MEMORY_NUM;
            i += 2;  // put char 2 times
            break;
        }
    }

    self->oled_pos.x = i;

    /* clean line */
    for (i = i * 8; i < 128; i++)
        oled_send_data(0x00);

    if (self->char_mem.end_ptr == self->oled_show_end) {
        self->oled_pos.y = 7;
        self->oled_status &= 0xFC;
    }

    oled_set_pos(self, self->oled_pos.y, self->oled_pos.x * 8);
}

void oled_prev_line(oled_control_t *self)
{
    if (self->oled_show_start == self->char_mem.start_ptr)
        return;
    self->start_page = (self->start_page + 0x7) & 0x7;
    oled_cmd(0x40 | (self->start_page * 8));

    self->oled_show_end =
        (self->oled_show_end - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;

    for (i = 0; i < 15; i++) {
        if (char_mem_is_new_line(
                self->char_mem,
                (self->oled_show_end - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM))
            break;
        self->oled_show_end =
            (self->oled_show_end - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
    }

    oled_set_pos(self, 0, 0);

    self->oled_show_start =
        (self->oled_show_start - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
    for (i = 0; i < 15; i++) {
        if (char_mem_is_new_line(self->char_mem,
                                 (self->oled_show_start - 1 + CHAR_MEMORY_NUM) %
                                     CHAR_MEMORY_NUM))
            break;
        self->oled_show_start =
            (self->oled_show_start - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
    }

    for (i = 0; i < 16 && self->char_mem.mem[self->oled_show_start + i]; i++) {
        oled_put_char(self, remove_newline_mark(self->char_mem,
                                                self->oled_show_start + i));
        if (char_mem_is_new_line(self->char_mem,
                                 self->oled_show_start + i + 1)) {
            oled_put_char(self,
                          remove_newline_mark(self->char_mem,
                                              self->oled_show_start + i + 1));
            i += 2;  // put char 2 times
            break;
        }
    }

    /* clean line */
    for (i = i * 8; i < 128; i++)
        oled_send_data(0x00);
    self->oled_status |= 1;
}

void oled_control_init(oled_control_t *self)
{
    self->oled_pos.dirty = 0;
    self->oled_pos.x = 0;
    self->oled_pos.y = 0;
    self->oled_show_start = 0;
    self->oled_show_end = 0;
    self->oled_status = 0;
    self->start_page = 0;
    reset_char_mem(&self->char_mem);
    oled_clear(self);
    oled_cmd(0x40);  // reset start page
    oled_set_pos(self, 0, 0);
}

void put_char(oled_control_t *self)
{
    while (1) {
        INTCONbits.GIE = 0;
        if (self->char_mem.end_ptr > self->oled_show_end)
            break;
        INTCONbits.GIE = 1;
    }
    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    if (self->oled_status & 1) {
        INTCONbits.GIE = 1;
        self->start_page = (self->start_page + 1) & 0x7;
        oled_cmd(0x40 | (self->start_page * 8));

        for (i = 0; i < 16;
             i++, self->oled_show_start =
                      (self->oled_show_start + 1) % CHAR_MEMORY_NUM) {
            if (char_mem_is_new_line(self->char_mem, self->oled_show_start)) {
                self->oled_show_start =
                    (self->oled_show_start + 1) % CHAR_MEMORY_NUM;
                break;
            }
        }

        oled_set_pos(self, 7, 0);

        for (i = 0; i < 128; i++)
            oled_send_data(0x00);

        self->oled_pos.y = 7;
        self->oled_pos.x = 0;
        self->oled_status &= 0xFC;

        oled_set_pos(self, self->oled_pos.y, self->oled_pos.x * 8);

        INTCONbits.GIE = 0;
    }

    i = self->char_mem.mem[self->oled_show_end];
    oled_put_char(self, i);
    self->oled_show_end = (self->oled_show_end + 1) % CHAR_MEMORY_NUM;

    INTCONbits.GIE = 1;
    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;
}
