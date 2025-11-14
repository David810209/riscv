// --- 模擬器 I/O ---
#define CSR_SIM_CTRL_EXIT (0u << 24)
#define CSR_SIM_CTRL_PUTC (1u << 24)

static inline void sim_exit(int code){
    unsigned int arg = CSR_SIM_CTRL_EXIT | (code & 0xFF);
    __asm__ volatile ("csrw dscratch, %0" :: "r"(arg));
}
static inline void sim_putc(char ch){
    unsigned int arg = CSR_SIM_CTRL_PUTC | (ch & 0xFF);
    __asm__ volatile ("csrw dscratch, %0" :: "r"(arg));
}
static inline void sim_puts(const char *s){
    while (*s) sim_putc(*s++);
}
static inline void sim_putu(unsigned x){
    unsigned d = 1000000000u, started = 0;
    for(; d; d/=10){
        unsigned q = x / d;
        if(q || started || d==1){ sim_putc('0'+q); started = 1; }
        x -= q*d;
    }
}

extern char _stack_top[];
__attribute__((naked)) void _start(void){
    __asm__ volatile (
        "la   sp, _stack_top \n"
        "call main          \n"
        "1: ebreak          \n"
        "   j 1b            \n"
    );
}

// 單筆測試（shamt 必須是編譯期常數）
#define CHECK_CASE_IMM(IDX, IN, SHAMT, EXP) do {                 \
    unsigned _out;                                               \
    __asm__ volatile ("bseti %0, %1, %2"                          \
                      : "=r"(_out)                               \
                      : "r"( (unsigned)(IN) ), "I"( (int)(SHAMT) )); \
    if (_out == (unsigned)(EXP)) {                               \
        sim_puts("OK"); sim_putu((IDX)); sim_putc('\n');         \
    } else {                                                     \
        sim_puts("ERR"); sim_putu((IDX)); sim_putc(' ');         \
        sim_puts("got="); sim_putu(_out);                        \
        sim_puts(" exp="); sim_putu((unsigned)(EXP));            \
        sim_putc('\n');                                          \
        fail |= 1;                                               \
    }                                                            \
} while(0)

int main(void){
    sim_puts("BSETI test\n");
    int fail = 0;
    CHECK_CASE_IMM(1, 0x00000000u,  5, 0x0000020u);
    CHECK_CASE_IMM(2, 0x00000001u,  0, 0x00000001u);
    CHECK_CASE_IMM(3, 0x00000001u,  3, 0x00000009u);
    CHECK_CASE_IMM(4, 0x00000001u,  4, 0x00000011u);
    CHECK_CASE_IMM(5, 0x00000001u,  8, 0x00000101u);

    if (fail == 0){ sim_puts("ALL OK\n"); sim_exit(0); }
    else          { sim_exit(1); }
    for(;;); /* 不會到這裡 */
}
