/*
 * Copyright (c) 2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <lib/bootargs.h>

#include <nusa/err.h>
#include <nusa/init.h>
#include <nusa/main.h>
#include <nusa/trace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if WITH_KERNEL_VM
#include <kernel/vm.h>
#endif

#define LOCAL_TRACE 0

static bool boot_args_valid = false;
static struct nusa_boot_arg *boot_args;

#define LK_BOOT_ARG_MAGIC 'lkbt'

/* nusa style boot args are spread across the 4 incoming argument pointers as follows:
 *
 * [0] = LK_BOOT_ARG_MAGIC
 * [1] = pointer to nusa_boot_arg list
 * [2] = unused
 * [3] = xor of ULONG_MAX and the previous 3
 */

#define LK_BOOT_ARG_TYPE_INITIAL 'args'
#define LK_BOOT_ARG_TYPE_COMMAND_LINE 'cmdl'
#define LK_BOOT_ARG_TYPE_BOOTIMAGE 'bimg'
#define LK_BOOT_ARG_TYPE_END 0

struct nusa_boot_arg {
    uint32_t type;
    uint32_t len;

    uint8_t  data[];
};

struct nusa_boot_arg_bootimage {
    uint64_t offset;
    size_t len;

    char device[];
};

static void *find_end(void *buf, size_t buf_len) {
    uint8_t *ptr = (uint8_t *)buf;
    struct nusa_boot_arg *arg = (struct nusa_boot_arg *)ptr;

    while (arg->type != LK_BOOT_ARG_TYPE_END) {
        ptr += sizeof(struct nusa_boot_arg) + arg->len;

        if ((uintptr_t)ptr - (uintptr_t)buf > buf_len)
            return NULL;

        arg = (struct nusa_boot_arg *)ptr;
    }

    return ptr;
}

status_t bootargs_start(void *buf, size_t buf_len) {
    if (buf_len < sizeof(struct nusa_boot_arg))
        return ERR_NO_MEMORY;

    memset(buf, 0, buf_len);

    struct nusa_boot_arg *arg = (struct nusa_boot_arg *)buf;
    arg->type = LK_BOOT_ARG_TYPE_INITIAL;
    arg->len = 0;

    return NO_ERROR;
}

status_t bootargs_add_command_line(void *buf, size_t buf_len, const char *str) {
    struct nusa_boot_arg *arg = find_end(buf, buf_len);
    if (!arg)
        return ERR_NO_MEMORY;

    arg->type = LK_BOOT_ARG_TYPE_COMMAND_LINE;
    arg->len = ROUNDUP(strlen(str) + 1, 4);
    memset(arg->data, 0, arg->len);
    strcpy((char *)arg->data, str);

    return NO_ERROR;
}

status_t bootargs_add_bootimage_pointer(void *buf, size_t buf_len, const char *device, uint64_t offset, size_t len) {
    struct nusa_boot_arg *arg = find_end(buf, buf_len);
    if (!arg)
        return ERR_NO_MEMORY;

    arg->type = LK_BOOT_ARG_TYPE_BOOTIMAGE;
    size_t string_len = device ? ROUNDUP(strlen(device) + 1, 4) : 0;
    arg->len = string_len + sizeof(struct nusa_boot_arg_bootimage);

    struct nusa_boot_arg_bootimage *bi = (struct nusa_boot_arg_bootimage *)arg->data;

    bi->offset = offset;
    bi->len = len;
    if (device) {
        memset(bi->device, 0, string_len);
        memcpy(bi->device, device, strlen(device));
    }

    return NO_ERROR;
}

void bootargs_generate_nusa_arg_values(ulong buf, ulong args[4]) {
    args[0] = LK_BOOT_ARG_MAGIC;
    args[1] = buf;
    args[2] = 0;
    args[3] = ULONG_MAX ^ args[0] ^ args[1] ^ args[2];
}

static void bootargs_init_hook(uint level) {
    LTRACE_ENTRY;

    /* see if there are any nusa style boot arguments here */
    if (nusa_boot_args[0] != LK_BOOT_ARG_MAGIC) {
        LTRACEF("failed magic check\n");
        return;
    }

    if (nusa_boot_args[3] != (ULONG_MAX ^ nusa_boot_args[0] ^ nusa_boot_args[1] ^ nusa_boot_args[2])) {
        LTRACEF("failed checksum\n");
        return;
    }

    /* parse the boot arg pointer */
#if WITH_KERNEL_VM
    boot_args = paddr_to_kvaddr(nusa_boot_args[1]);
#else
    boot_args = (void *)nusa_boot_args[1];
#endif

    if (!boot_args) {
        LTRACEF("null or invalid boot pointer\n");
        return;
    }

    /* see if the initial entry is the right one */
    if (boot_args[0].type != LK_BOOT_ARG_TYPE_INITIAL) {
        LTRACEF("bad initial arg\n");
        return;
    }

    /* looks good */
    boot_args_valid = true;

    LTRACEF("valid args found\n");
}

bool bootargs_are_valid(void) {
    return boot_args_valid;
}

static struct nusa_boot_arg *find_tag(uint32_t tag) {
    if (!boot_args_valid)
        return NULL;

    struct nusa_boot_arg *arg = boot_args;

    while (arg->type != LK_BOOT_ARG_TYPE_END) {
        if (arg->type == tag)
            return arg;

        arg = (struct nusa_boot_arg *)((uintptr_t)arg + sizeof(struct nusa_boot_arg) + arg->len);
    }

    return NULL;
}

const char *bootargs_get_command_line(void) {
    struct nusa_boot_arg *arg = find_tag(LK_BOOT_ARG_TYPE_COMMAND_LINE);
    if (!arg)
        return NULL;

    // XXX validate it

    return (const char *)arg->data;
}

status_t bootargs_get_bootimage_pointer(uint64_t *offset, size_t *len, const char **device) {
    struct nusa_boot_arg *arg = find_tag(LK_BOOT_ARG_TYPE_BOOTIMAGE);
    if (!arg)
        return ERR_NOT_FOUND;

    // XXX validate it

    struct nusa_boot_arg_bootimage *bi = (struct nusa_boot_arg_bootimage *)arg->data;

    if (device) {
        if (arg->len != sizeof(struct nusa_boot_arg_bootimage)) {
            /* string is present */
            *device = bi->device;
        } else {
            *device = NULL;
        }
    }

    if (offset)
        *offset = bi->offset;
    if (len)
        *len = bi->len;

    return NO_ERROR;
}

LK_INIT_HOOK(bootargs, bootargs_init_hook, LK_INIT_LEVEL_THREADING);

