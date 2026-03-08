#include <oled_driver.h>

oled_control_t oled_control;

void picos_comm()
{
    while (1) {
        char c = uart_getchar();
        INTCONbits.GIE = 0;
        put_char(&oled_control, c);
        INTCONbits.GIE = 1;
    }
}

void __interrupt(high_priority) high_isr(void)
{
    if (INTCONbits.INT0IF) {
        oled_next_line(&oled_control);
        INTCONbits.INT0IF = 0;
    }

    // INT1 External Interrupt
    if (INTCON3bits.INT1IF) {
        oled_prev_line(&oled_control);
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
    oled_control_init(&oled_control);

    picos_comm();

    while (1)
        ;
}
