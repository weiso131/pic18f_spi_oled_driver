#include <char_mem.h>

char char_cnt = 0;

void update_char_mem(char_mem_t *self, char c)
{
    if (c == '\n') {
        char_cnt = 15;
        c = 0x20;
    }
    if (c >= 0x20 && c <= 0x7E) {
        char_cnt++;
        if (char_cnt == 16) {
            char_cnt = 0;
            c |= CHAR_MEMORY_NEWLINE;
        }
        self->mem[self->end_ptr] = c;
        self->end_ptr = (self->end_ptr + 1) % CHAR_MEMORY_NUM;
        if (self->end_ptr == self->start_ptr)
            self->start_ptr = (self->start_ptr + 1) % CHAR_MEMORY_NUM;
        self->mem[self->end_ptr] = '\0';
    } else if (c == '\b') {
        /** This is base on picos shell
         * backspace only use when command input
         */
        char_cnt = (char_cnt + 15) & 0xF;
        self->end_ptr = (self->end_ptr - 1 + CHAR_MEMORY_NUM) % CHAR_MEMORY_NUM;
    }
    self->char_mem_update = 1;
}

void reset_char_mem(char_mem_t *self)
{
    self->start_ptr = 0;
    self->end_ptr = 0;
    for (unsigned int i = 0; i < CHAR_MEMORY_NUM; i++)
        self->mem[i] = '\0';
    char_cnt = 0;
    self->char_mem_update = 0;
}
