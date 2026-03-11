#include <xc.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = OFF
#pragma config MCLRE = ON

#define _XTAL_FREQ 4000000ULL

// Send byte via SPI
void oled_write(uint8_t data);

// Send command
void oled_cmd(uint8_t cmd);

// Send data
void oled_send_data(uint8_t data);

void spi_init(void);

// Initialize display
void oled_init(void);

void uart_init(void);

inline void uart_putchar(char c);

inline char uart_getchar(void);
