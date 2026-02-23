#include <font_8x8.h>
#include <oled_driver.h>

typedef union {
    struct {
        unsigned x : 4;
        unsigned y : 3;
        unsigned dirty : 1;
    };
} oled_pos_t;

oled_pos_t oled_pos = {0};

void oled_put_char(char c)
{
    if (c == '\n') {
        if (oled_pos.y == 7)
            oled_pos.y = 0;
        else
            oled_pos.y++;
        oled_pos.dirty = 1;
    } else if (c == '\r') {
        oled_pos.x = 0;
        oled_pos.dirty = 1;
    } else {
        oled_send_data(font_8x8[c - 0x20][0]);
        oled_send_data(font_8x8[c - 0x20][1]);
        oled_send_data(font_8x8[c - 0x20][2]);
        oled_send_data(font_8x8[c - 0x20][3]);
        oled_send_data(font_8x8[c - 0x20][4]);
        oled_send_data(font_8x8[c - 0x20][5]);
        oled_send_data(font_8x8[c - 0x20][6]);
        oled_send_data(font_8x8[c - 0x20][7]);
        oled_pos.x++;

        if (oled_pos.x == 15) {
            if (oled_pos.y == 7)
                oled_pos.y = 0;
            else
                oled_pos.y++;
            oled_pos.x = 0;
            oled_pos.dirty = 1;
        }
    }

    if (oled_pos.dirty) {
        oled_set_pos(oled_pos.y, oled_pos.x * 8);
        oled_pos.dirty = 0;
    }
}

void test_number()
{
    uint8_t cnt = 0;
    while (1) {
        oled_put_char('0' + cnt);
        cnt++;
        if (cnt == 10) {
            cnt = 0;
            oled_put_char('\r');
            oled_put_char('\n');
        }
        __delay_ms(100);
    }
}

void test_alpha()
{
    uint8_t cnt = 0;
    while (1) {
        oled_put_char('A' + cnt);
        cnt = (cnt + 1) % 26;
        __delay_ms(100);
    }
}

void main(void)
{
    oled_init();
    oled_clear();


    oled_pos.x = 0;
    oled_pos.y = 0;
    oled_pos.dirty = 0;

    oled_set_pos(0, 0);

    test_alpha();
}
