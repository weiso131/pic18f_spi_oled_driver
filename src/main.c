#include <oled_driver.h>

void test_number()
{
    uint8_t cnt = 0;
    while (1) {
        put_char('0' + cnt);
        cnt++;
        if (cnt == 10) {
            cnt = 0;
            put_char('\r');
            put_char('\n');
        }
        __delay_ms(100);
    }
}

void test_alpha()
{
    uint8_t cnt = 0;
    while (1) {
        put_char('A' + cnt);
        cnt = (cnt + 1) % 26;
        __delay_ms(100);
    }
}

void main(void)
{
    oled_init();
    oled_clear();

    oled_set_pos(0, 0);

    test_alpha();
}
