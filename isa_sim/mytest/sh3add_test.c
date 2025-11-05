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

static inline int check_case(unsigned idx, unsigned in,unsigned in2, unsigned expect){
    unsigned out;
    __asm__ volatile ("sh3add %0, %1, %2" : "=r"(out) : "r"(in), "r"(in2)); 
    if (out == expect){
        sim_puts("OK"); sim_putu(idx); sim_putc('\n');
        return 0;
    } else {
        sim_puts("ERR"); sim_putu(idx); sim_putc(' ');
        sim_puts("got="); sim_putu(out);
        sim_puts(" exp="); sim_putu(expect);
        sim_putc('\n');
        return 1;
    }
}

int main(void){
    sim_puts("SH3ADD test\n");
    int fail = 0;
    fail |= check_case(1, 0x8524FAC6u, 0x0AF56910u, 0x341D3F40u);

    if (fail == 0){ sim_puts("ALL OK\n"); sim_exit(0); }
    else          { sim_exit(1); }
    for(;;); /* 不會到這裡 */
}
