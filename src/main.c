#include <oled_driver.h>

oled_control_t oled_control;

void picos_comm()
{
    while (1) {
        put_char(&oled_control);
        __delay_ms(10);
    }
}

void __interrupt(high_priority) high_isr(void)
{
    if (INTCONbits.INT0IF && INTCONbits.INT0IE) {
        oled_next_line(&oled_control);
        INTCONbits.INT0IF = 0;
    }

    if (INTCON3bits.INT1IF && INTCON3bits.INT1IE) {
        oled_prev_line(&oled_control);
        INTCON3bits.INT1IF = 0;
    }

    if (PIR1bits.RCIF) {
        char c = RCREG;
        if (c == 0x3)
            oled_control_init(&oled_control);
        else {
            update_char_mem(&(oled_control.char_mem), c);
        }
        PIR1bits.RCIF = 0;
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
    PIE1bits.RCIE = 1;

    uart_init();

    oled_init();
    oled_control_init(&oled_control);

    picos_comm();

    while (1)
        ;
}
