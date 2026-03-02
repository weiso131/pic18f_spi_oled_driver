#include <char_mem.h>

void update_char_mem(char_mem_t *self, char c)
{
    if (c == '\n')
        self->mem[(self->end_ptr + CHAR_MEMORY_NUM - 1) % CHAR_MEMORY_NUM] |=
            CHAR_MEMORY_NEWLINE;
    else if (c >= 0x20 && c <= 0x7E) {
        self->mem[self->end_ptr] = c;
        self->end_ptr = (self->end_ptr + 1) % CHAR_MEMORY_NUM;
        if (self->end_ptr == self->start_ptr)
            self->start_ptr = (self->start_ptr + 1) % CHAR_MEMORY_NUM;
        self->mem[self->end_ptr] = '\0';
    }
}
