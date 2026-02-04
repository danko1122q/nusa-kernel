/*
 * Copyright (c) 2013-2015 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <nusa/compiler.h>
#include <sys/types.h>

__BEGIN_CDECLS

/*
 * NUSA's init system
 */

typedef void (*nusa_init_hook)(uint level);

enum nusa_init_level {
    NUSA_INIT_LEVEL_EARLIEST = 1,

    NUSA_INIT_LEVEL_ARCH_EARLY     = 0x1000,
    NUSA_INIT_LEVEL_PLATFORM_EARLY = 0x2000,
    NUSA_INIT_LEVEL_TARGET_EARLY   = 0x3000,
    NUSA_INIT_LEVEL_HEAP           = 0x4000,
    NUSA_INIT_LEVEL_VM             = 0x5000,
    NUSA_INIT_LEVEL_KERNEL         = 0x6000,
    NUSA_INIT_LEVEL_THREADING      = 0x7000,
    NUSA_INIT_LEVEL_ARCH           = 0x8000,
    NUSA_INIT_LEVEL_PLATFORM       = 0x9000,
    NUSA_INIT_LEVEL_TARGET         = 0xa000,
    NUSA_INIT_LEVEL_APPS           = 0xb000,

    NUSA_INIT_LEVEL_LAST = UINT16_MAX,

    // Legacy LK_ aliases for backward compatibility
    LK_INIT_LEVEL_EARLIEST = NUSA_INIT_LEVEL_EARLIEST,
    LK_INIT_LEVEL_ARCH_EARLY = NUSA_INIT_LEVEL_ARCH_EARLY,
    LK_INIT_LEVEL_PLATFORM_EARLY = NUSA_INIT_LEVEL_PLATFORM_EARLY,
    LK_INIT_LEVEL_TARGET_EARLY = NUSA_INIT_LEVEL_TARGET_EARLY,
    LK_INIT_LEVEL_HEAP = NUSA_INIT_LEVEL_HEAP,
    LK_INIT_LEVEL_VM = NUSA_INIT_LEVEL_VM,
    LK_INIT_LEVEL_KERNEL = NUSA_INIT_LEVEL_KERNEL,
    LK_INIT_LEVEL_THREADING = NUSA_INIT_LEVEL_THREADING,
    LK_INIT_LEVEL_ARCH = NUSA_INIT_LEVEL_ARCH,
    LK_INIT_LEVEL_PLATFORM = NUSA_INIT_LEVEL_PLATFORM,
    LK_INIT_LEVEL_TARGET = NUSA_INIT_LEVEL_TARGET,
    LK_INIT_LEVEL_APPS = NUSA_INIT_LEVEL_APPS,
    LK_INIT_LEVEL_LAST = NUSA_INIT_LEVEL_LAST,
};

/**
 * enum nusa_init_flags - Flags specifying init hook type.
 *
 * Flags passed to NUSA_INIT_HOOK_FLAGS to specify when the hook should be called.
 */
enum nusa_init_flags {
    /**
     * @NUSA_INIT_FLAG_PRIMARY_CPU: Call init hook when booting primary CPU.
     */
    NUSA_INIT_FLAG_PRIMARY_CPU     = 0x1,

    /**
     * @NUSA_INIT_FLAG_SECONDARY_CPUS: Call init hook when booting secondary CPUs.
     */
    NUSA_INIT_FLAG_SECONDARY_CPUS  = 0x2,

    /**
     * @NUSA_INIT_FLAG_ALL_CPUS: Call init hook when booting any CPU.
     */
    NUSA_INIT_FLAG_ALL_CPUS        = NUSA_INIT_FLAG_PRIMARY_CPU | NUSA_INIT_FLAG_SECONDARY_CPUS,

    /**
     * @NUSA_INIT_FLAG_CPU_ENTER_IDLE: Call init hook before a CPU enters idle.
     *
     * The CPU may lose state after this, but it should respond to interrupts.
     */
    NUSA_INIT_FLAG_CPU_ENTER_IDLE  = 0x4,

    /**
     * @NUSA_INIT_FLAG_CPU_OFF: Call init hook before a CPU goes offline.
     *
     * The CPU may lose state after this, and it should not respond to
     * interrupts.
     */
    NUSA_INIT_FLAG_CPU_OFF         = 0x8,

    /**
     * @NUSA_INIT_FLAG_CPU_SUSPEND: Call init hook before a CPU loses state.
     *
     * Alias to call hook for both NUSA_INIT_FLAG_CPU_ENTER_IDLE and
     * NUSA_INIT_FLAG_CPU_OFF events.
     */
    NUSA_INIT_FLAG_CPU_SUSPEND     = NUSA_INIT_FLAG_CPU_ENTER_IDLE | NUSA_INIT_FLAG_CPU_OFF,

    /**
     * @NUSA_INIT_FLAG_CPU_EXIT_IDLE: Call init hook after a CPU exits idle.
     *
     * NUSA_INIT_FLAG_CPU_ENTER_IDLE should have been called before this.
     */
    NUSA_INIT_FLAG_CPU_EXIT_IDLE   = 0x10,

    /**
     * @NUSA_INIT_FLAG_CPU_ON: Call init hook after a CPU turns on.
     *
     * NUSA_INIT_FLAG_CPU_OFF should have been called before this. The first time
     * a CPU turns on NUSA_INIT_FLAG_PRIMARY_CPU or NUSA_INIT_FLAG_SECONDARY_CPUS
     * is called instead of this.
     */
    NUSA_INIT_FLAG_CPU_ON          = 0x20,

    /**
     * @NUSA_INIT_FLAG_CPU_RESUME: Call init hook after a CPU exits idle.
     *
     * Alias to call hook for both NUSA_INIT_FLAG_CPU_EXIT_IDLE and
     * NUSA_INIT_FLAG_CPU_ON events.
     */
    NUSA_INIT_FLAG_CPU_RESUME      = NUSA_INIT_FLAG_CPU_EXIT_IDLE | NUSA_INIT_FLAG_CPU_ON,

    // Legacy LK_ aliases for backward compatibility
    LK_INIT_FLAG_PRIMARY_CPU = NUSA_INIT_FLAG_PRIMARY_CPU,
    LK_INIT_FLAG_SECONDARY_CPUS = NUSA_INIT_FLAG_SECONDARY_CPUS,
    LK_INIT_FLAG_ALL_CPUS = NUSA_INIT_FLAG_ALL_CPUS,
    LK_INIT_FLAG_CPU_ENTER_IDLE = NUSA_INIT_FLAG_CPU_ENTER_IDLE,
    LK_INIT_FLAG_CPU_OFF = NUSA_INIT_FLAG_CPU_OFF,
    LK_INIT_FLAG_CPU_SUSPEND = NUSA_INIT_FLAG_CPU_SUSPEND,
    LK_INIT_FLAG_CPU_EXIT_IDLE = NUSA_INIT_FLAG_CPU_EXIT_IDLE,
    LK_INIT_FLAG_CPU_ON = NUSA_INIT_FLAG_CPU_ON,
    LK_INIT_FLAG_CPU_RESUME = NUSA_INIT_FLAG_CPU_RESUME,
};

// Run init hooks between start_level (inclusive) and stop_level (exclusive) that match the required_flags.
void nusa_init_level(enum nusa_init_flags required_flags, uint16_t start_level, uint16_t stop_level);

static inline void nusa_primary_cpu_init_level(uint16_t start_level, uint16_t stop_level) {
    nusa_init_level(NUSA_INIT_FLAG_PRIMARY_CPU, start_level, stop_level);
}

static inline void nusa_init_level_all(enum nusa_init_flags flags) {
    nusa_init_level(flags, NUSA_INIT_LEVEL_EARLIEST, NUSA_INIT_LEVEL_LAST);
}

struct nusa_init_struct {
    uint16_t level;
    uint16_t flags;
    nusa_init_hook hook;
    const char *name;
};

// Define an init hook with specific flags, level, and name.
#define NUSA_INIT_HOOK_FLAGS(_name, _hook, _level, _flags) \
    static const struct nusa_init_struct _init_struct_##_name __ALIGNED(sizeof(void *)) __SECTION("nusa_init") = { \
        .level = (_level), \
        .flags = (_flags), \
        .hook = (_hook), \
        .name = #_name, \
    };

// Shortcut for defining an init hook with primary CPU flag.
#define NUSA_INIT_HOOK(_name, _hook, _level) \
    NUSA_INIT_HOOK_FLAGS(_name, _hook, _level, NUSA_INIT_FLAG_PRIMARY_CPU)

// Legacy LK_ aliases for backward compatibility
#define LK_INIT_HOOK_FLAGS NUSA_INIT_HOOK_FLAGS
#define LK_INIT_HOOK NUSA_INIT_HOOK

__END_CDECLS