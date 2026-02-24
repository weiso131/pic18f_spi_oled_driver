#include <hal.h>

#define RST LATDbits.LATD6
#define DC LATDbits.LATD7

// Send byte via SPI
void oled_write(uint8_t data)
{
    SSPBUF = data;
    while (!SSPSTATbits.BF)
        ;
}

// Send command
void oled_cmd(uint8_t cmd)
{
    DC = 0;
    oled_write(cmd);
}

// Send data
void oled_send_data(uint8_t data)
{
    DC = 1;
    oled_write(data);
}

void spi_init(void)
{
    // Setup
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC5 = 0;
    TRISDbits.TRISD6 = 0;
    TRISDbits.TRISD7 = 0;

    // SPI Mode 0
    SSPSTATbits.CKE = 1;
    SSPCON1bits.CKP = 0;
    SSPCON1bits.SSPM = 0;
    SSPCON1bits.SSPEN = 1;
}

// Initialize display
void oled_init(void)
{
    spi_init();

    RST = 0;
    __delay_ms(10);
    RST = 1;
    __delay_ms(10);
    oled_cmd(0xAD);
    oled_cmd(0x8B);  // Charge pump ON
    oled_cmd(0x33);  // VPP = 9V
    oled_cmd(0xA1);  // Flip horizontal
    oled_cmd(0xC8);  // Flip vertical
    oled_cmd(0x81);
    oled_cmd(0xFF);  // Max contrast
    oled_cmd(0xAF);  // Display ON
}

void uart_init(void)
{
    unsigned long baud = 9600;
    unsigned long spbrg;

    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 1;

    spbrg = (_XTAL_FREQ / (4UL * baud)) - 1;

    SPBRG = spbrg & 0xFF;
    SPBRGH = (spbrg >> 8) & 0xFF;

    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
}


inline void uart_putchar(char c)
{
    while (!PIR1bits.TXIF)
        ;
    TXREG = c;
}
