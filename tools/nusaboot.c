/*
 * Copyright (c) 2014 Brian Swetland
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include "libnusaboot.h"

void usage(void) {
    fprintf(stderr,
            "usage: nusaboot <hostname> <command> ...\n"
            "\n"
            "       nusaboot <hostname> flash <partition> <filename>\n"
            "       nusaboot <hostname> erase <partition>\n"
            "       nusaboot <hostname> remove <partition>\n"
            "       nusaboot <hostname> fpga <bitfile>\n"
            "       nusaboot <hostname> boot <binary>\n"
            "       nusaboot <hostname> getsysparam <name>\n"
            "       nusaboot <hostname> reboot\n"
            "       nusaboot <hostname> :<commandname> [ <arg>* ]\n"
            "\n"
            "NOTE: If <hostname> is 'jtag', nusaboot will attempt to use\n"
            "       a tool 'zynq-dcc' to communicate with the device.\n"
            "       Make sure it is in your path.\n"
            "\n"
           );
    exit(1);
}

void printsysparam(void *data, int len) {
    unsigned char *x = data;
    int i;
    for (i = 0; i < len; i++) {
        if ((x[i] < ' ') || (x[1] > 127)) goto printhex;
    }
    write(STDERR_FILENO, "\"", 1);
    write(STDERR_FILENO, data, len);
    write(STDERR_FILENO, "\"\n", 2);
    return;
printhex:
    fprintf(stderr, "[");
    for (i = 0; i < len; i++) fprintf(stderr, " %02x", x[i]);
    fprintf(stderr, " ]\n");
}

int main(int argc, char **argv) {
    const char *host = argv[1];
    const char *cmd = argv[2];
    const char *args = argv[3];
    const char *fn = NULL;
    int fd = -1;

    if (argc == 3) {
        if (!strcmp(cmd, "reboot")) {
            return nusaboot_txn(host, cmd, fd, "");
        } else if (cmd[0] == ':') {
            return nusaboot_txn(host, cmd + 1, fd, "");
        } else {
            usage();
        }
    }

    if (argc < 4) usage();

    if (!strcmp(cmd, "flash")) {
        if (argc < 5) usage();
        fn = argv[4];
    } else if (!strcmp(cmd, "fpga")) {
        fn = args;
        args = "";
    } else if (!strcmp(cmd, "boot")) {
        fn = args;
        args = "";
    } else if (!strcmp(cmd, "erase")) {
    } else if (!strcmp(cmd, "remove")) {
    } else if (!strcmp(cmd, "getsysparam")) {
        if (nusaboot_txn(host, cmd, -1, args) == 0) {
            void *rbuf = NULL;
            printsysparam(rbuf, nusaboot_get_reply(&rbuf));
            return 0;
        } else {
            return -1;
        }
    } else if (cmd[0] == ':') {
        return nusaboot_txn(host, cmd + 1, -1, args);
    } else {
        usage();
    }
    if (fn) {
        if ((fd = open(fn, O_RDONLY)) < 0) {
            fprintf(stderr, "error; cannot open '%s'\n", fn);
            return -1;
        }
    }
    return nusaboot_txn(host, cmd, fd, args);
}
