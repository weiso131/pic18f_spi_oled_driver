#include <oled_driver.h>

void main(void) {   
    oled_init();
    oled_clear();
    
    oled_set_pos(0, 0);
    for(uint8_t i = 0; i < 128; i++) oled_send_data(0xFF);
    
    while(1);
}
