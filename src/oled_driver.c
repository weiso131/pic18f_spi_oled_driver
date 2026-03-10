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

void oled_put_char(oled_control_t *self, char c)
{
    if (c == '\n') {
        if (self->oled_status & 0x4)
            self->oled_status &= 0xFB;
        else if (self->oled_pos.y == 7) {
            self->oled_status |= 1;
            return;
        } else {
            self->oled_pos.y++;
            self->oled_pos.x = 0;
            self->oled_pos.dirty = 1;
        }
    } else if (c == '\b' && !(self->oled_pos.x == 0 && self->oled_pos.y == 0)) {
        self->oled_show_end =
            (self->oled_show_end - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
        /* This should be done in update_char_mem, but in here */
        /* I don't need to deal with other problem */
        self->char_mem.mem[self->char_mem.end_ptr] = '\0';
        self->char_mem.end_ptr =
            (self->char_mem.end_ptr - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
        if (self->oled_status & 1) {
            for (i = 1; i < 15 && !char_mem_is_new_line(
                                      self->char_mem, (self->oled_show_end - i +
                                                       CHAR_MEMORY_NUM) %
                                                          CHAR_MEMORY_NUM);
                 i++)
                ;
            self->oled_pos.x = i;
        } else if (self->oled_pos.x == 0) {
            self->oled_pos.y--;
            for (i = 1; i < 15 && !char_mem_is_new_line(
                                      self->char_mem, (self->oled_show_end - i +
                                                       CHAR_MEMORY_NUM) %
                                                          CHAR_MEMORY_NUM);
                 i++)
                ;
            self->oled_pos.x = i;
        } else
            self->oled_pos.x--;
        self->oled_pos.dirty = 1;
        self->oled_status &= 0xFC;
    } else if (c >= 0x20 && c <= 0x7E) {
        oled_send_data(font_8x8[c - 0x20][0]);
        oled_send_data(font_8x8[c - 0x20][1]);
        oled_send_data(font_8x8[c - 0x20][2]);
        oled_send_data(font_8x8[c - 0x20][3]);
        oled_send_data(font_8x8[c - 0x20][4]);
        oled_send_data(font_8x8[c - 0x20][5]);
        oled_send_data(font_8x8[c - 0x20][6]);
        oled_send_data(font_8x8[c - 0x20][7]);
        self->oled_status &= 0xFB;
        if (self->oled_pos.x == 15) {
            if (self->oled_pos.y == 7) {
                self->oled_status |= 1;
                return;
            } else {
                self->oled_status |= 0x4;  // the next newline will ignore
                self->oled_pos.y++;
                self->oled_pos.x = 0;
                self->oled_pos.dirty = 1;
            }
        } else
            self->oled_pos.x++;
    } else
        return;

    if (self->oled_pos.dirty) {
        oled_set_pos(self, self->oled_pos.y, self->oled_pos.x * 8);
        self->oled_pos.dirty = 0;
    }
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

    /* interrupt will disable here */
    if (!(self->oled_status & 1)) {
        i = self->char_mem.mem[self->oled_show_end];
        uart_putchar(':');
        oled_put_char(self, i);
        self->oled_show_end = (self->oled_show_end + 1) % CHAR_MEMORY_NUM;
    }
    INTCONbits.GIE = 1;
}
