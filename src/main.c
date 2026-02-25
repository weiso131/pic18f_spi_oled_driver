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
    for (unsigned char i = 0; i < 26; i++) {
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('A' + i);
        put_char('\n');
        __delay_ms(1);
    }
}

void __interrupt(high_priority) high_isr(void)
{
    if (INTCONbits.INT0IF) {
        oled_next_line();
        INTCONbits.INT0IF = 0;
    }

    // INT1 External Interrupt
    if (INTCON3bits.INT1IF) {
        oled_prev_line();
        INTCON3bits.INT1IF = 0;
    }
}

void main(void)
{
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = 1;
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;

    TRISBbits.TRISB1 = 1;
    INTCON2bits.INTEDG1 = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IE = 1;

    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;

    uart_init();

    oled_init();
    oled_clear();

    oled_set_pos(0, 0);

    test_alpha();

    while (1)
        ;
}
