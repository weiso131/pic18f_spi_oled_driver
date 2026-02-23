#include <font_8x8.h>
#include <oled_driver.h>

#define RST LATDbits.LATD6
#define DC LATDbits.LATD7

typedef union {
    struct {
        unsigned x : 4;
        unsigned y : 3;
        unsigned dirty : 1;
    };
} oled_pos_t;

oled_pos_t oled_pos = {0};

#define CHAR_MEMORY_NEWLINE 0x80
#define CHAR_MEMORY_NUM 0x500

char char_mem[CHAR_MEMORY_NUM] __at(0x100);
unsigned int memory_ptr = 0;
unsigned int oled_show_start = 0;
unsigned int oled_show_end = 0;
char memory_ptr_out = 0;

unsigned char start_page = 0;

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

void update_char_mem(char c)
{
    if (c == '\n')
        char_mem[(memory_ptr + CHAR_MEMORY_NUM - 1) % CHAR_MEMORY_NUM] |=
            CHAR_MEMORY_NEWLINE;
    else if (c >= 0x20 && c <= 0x7E) {
        char_mem[memory_ptr] = c;
        memory_ptr = (memory_ptr + 1) % CHAR_MEMORY_NUM;
        char_mem[memory_ptr] = '\0';
    }
}

void oled_put_char(char c)
{
    if (!memory_ptr_out) {
        if (c == '\n') {
            if (oled_pos.y == 7) {
                memory_ptr_out = 1;
                return;
            } else {
                oled_pos.y++;
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

void oled_next_line()
{
    start_page = (start_page + 1) & 0x7;
    oled_cmd(0x40 | (start_page * 8));

    unsigned char i;

    for (i = 0; i < 16;
         i++, oled_show_start = (oled_show_start + 1) % CHAR_MEMORY_NUM) {
        if (!(char_mem[oled_show_start] | CHAR_MEMORY_NEWLINE)) {
            oled_show_start = (oled_show_start + 1) % CHAR_MEMORY_NUM;
            break;
        }
    }

    oled_pos.y = 7;
    oled_pos.x = 0;
    oled_set_pos(7, 0);

    for (i = 0; i < 16 && char_mem[oled_show_end]; i++, oled_show_end++) {
        oled_put_char(char_mem[oled_show_end]);
        if (!(char_mem[oled_show_end] | CHAR_MEMORY_NEWLINE)) {
            oled_show_end = (oled_show_end + 1) % CHAR_MEMORY_NUM;
            break;
        }
    }

    /* clean line */
    for (i = i * 8; i < 128; i++)
        oled_send_data(0x00);

    oled_set_pos(oled_pos.y, oled_pos.x);
}

void put_char(char c)
{
    oled_put_char(c);
    update_char_mem(c);

    if (memory_ptr_out) {
        oled_next_line();
        memory_ptr_out = 0;
    }
}
