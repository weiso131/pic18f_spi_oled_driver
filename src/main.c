#include <oled_driver.h>

oled_control_t oled_control;

void picos_comm()
{
    while (1)
        put_char(&oled_control);
}

void __interrupt(high_priority) high_isr(void)
{
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

void __interrupt(low_priority) low_isr(void)
{
    if (INTCON3bits.INT1IF && INTCON3bits.INT1IE) {
        oled_next_line(&oled_control);
        INTCON3bits.INT1IF = 0;
    }

    if (INTCON3bits.INT2IF && INTCON3bits.INT2IE) {
        oled_prev_line(&oled_control);
        INTCON3bits.INT2IF = 0;
    }
}

void main(void)
{
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 0b00;

    while (!OSCCONbits.IOFS)
        ;

    TRISBbits.TRISB1 = 1;
    INTCON2bits.INTEDG1 = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IP = 0;  // set as low priority

    TRISBbits.TRISB2 = 1;
    INTCON2bits.INTEDG2 = 1;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IP = 0;  // set as low priority

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;

    uart_init();

    oled_init();
    oled_control_init(&oled_control);

    picos_comm();

    while (1)
        ;
}