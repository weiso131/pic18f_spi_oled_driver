#define CHAR_MEMORY_NEWLINE 0x80
#define CHAR_MEMORY_NUM 0x500

typedef struct char_mem {
    unsigned int memory_ptr;
    char mem[CHAR_MEMORY_NUM];
} char_mem_t;

void update_char_mem(char_mem_t *self, char c);

#define char_mem_is_new_line(self, pos) (self.mem[pos] & CHAR_MEMORY_NEWLINE)

#define remove_newline_mark(self, pos) (self.mem[pos] & 0x7F)
