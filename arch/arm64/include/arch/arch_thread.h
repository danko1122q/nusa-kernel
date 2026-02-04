#pragma once

#include <sys/types.h>

struct fpstate {
    uint64_t    regs[64];
    uint32_t    fpcr;
    uint32_t    fpsr;
    uint        current_cpu;
};

struct arch_thread {
    vaddr_t sp;
    struct fpstate fpstate;
};

