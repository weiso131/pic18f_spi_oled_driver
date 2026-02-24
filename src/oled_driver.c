#include <char_mem.h>
#include <font_8x8.h>
#include <hal.h>
#include <oled_driver.h>

typedef union {
    struct {
        unsigned x : 4;
        unsigned y : 3;
        unsigned dirty : 1;
    };
} oled_pos_t;

oled_pos_t oled_pos = {0};

unsigned int oled_show_start = 0;
unsigned int oled_show_end = 0;
char memory_ptr_out = 0;

unsigned char start_page = 0;

char_mem_t char_mem = {0};

// Set position
void oled_set_pos(uint8_t page, uint8_t col)
{
    col += 2;
    oled_cmd(0xB0 + ((page + start_page) & 0x7));
    oled_cmd(0x00 | (col & 0x0F));
    oled_cmd(0x10 | (col >> 4));
}

// Clear screen
void oled_clear(void)
{
    for (uint8_t p = 0; p < 8; p++) {
        oled_set_pos(p, 0);
        for (uint8_t c = 0; c < 128; c++)
            oled_send_data(0x00);
    }
}

void oled_put_char(char c)
{
    if (!(memory_ptr_out & 1)) {
        if (c == '\n') {
            if (oled_pos.y == 7) {
                memory_ptr_out |= 1;
                return;
            } else {
                oled_pos.y++;
                oled_pos.x = 0;
                oled_pos.dirty = 1;
            }
        } else if (c >= 0x20 && c <= 0x7E) {
            oled_show_end = (oled_show_end + 1) % CHAR_MEMORY_NUM;

            oled_send_data(font_8x8[c - 0x20][0]);
            oled_send_data(font_8x8[c - 0x20][1]);
            oled_send_data(font_8x8[c - 0x20][2]);
            oled_send_data(font_8x8[c - 0x20][3]);
            oled_send_data(font_8x8[c - 0x20][4]);
            oled_send_data(font_8x8[c - 0x20][5]);
            oled_send_data(font_8x8[c - 0x20][6]);
            oled_send_data(font_8x8[c - 0x20][7]);

            if (oled_pos.x == 15) {
                if (oled_pos.y == 7) {
                    memory_ptr_out = 1;
                    return;
                } else {
                    oled_pos.y++;
                    oled_pos.x = 0;
                    oled_pos.dirty = 1;
                }
            } else
                oled_pos.x++;
        } else
            return;

        if (oled_pos.dirty) {
            oled_set_pos(oled_pos.y, oled_pos.x * 8);
            oled_pos.dirty = 0;
        }
    }
}

unsigned char i;

void oled_next_line()
{
    start_page = (start_page + 1) & 0x7;
    oled_cmd(0x40 | (start_page * 8));

    for (i = 0; i < 16;
         i++, oled_show_start = (oled_show_start + 1) % CHAR_MEMORY_NUM) {
        if (char_mem_is_new_line(char_mem, oled_show_start)) {
            oled_show_start = (oled_show_start + 1) % CHAR_MEMORY_NUM;
            break;
        }
    }

    oled_pos.y = 7;
    oled_pos.x = 0;
    oled_set_pos(7, 0);

    memory_ptr_out = (char) (memory_ptr_out << 1);
    for (i = 0; i < 16 && char_mem.mem[oled_show_end]; i++) {
        oled_put_char(remove_newline_mark(char_mem, oled_show_end));
        if (char_mem_is_new_line(char_mem, oled_show_end)) {
            oled_put_char(remove_newline_mark(char_mem, oled_show_end));
            i += 2;
            break;
        }
    }
    memory_ptr_out = memory_ptr_out >> 1;


    /* clean line */
    for (i = i * 8; i < 128; i++)
        oled_send_data(0x00);

    oled_set_pos(oled_pos.y, oled_pos.x);
}

void oled_prev_line() {}

void put_char(char c)
{
    oled_put_char(c);
    uart_putchar(c);
    if (c == '\n')
        uart_putchar('\r');
    update_char_mem(&char_mem, c);
}
