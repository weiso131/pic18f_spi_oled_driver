#include <oled_driver.h>

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

// Set position
void oled_set_pos(uint8_t page, uint8_t col)
{
    col += 2;
    oled_cmd(0xB0 + page);
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
