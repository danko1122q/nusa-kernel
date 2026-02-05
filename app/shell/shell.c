/*
 * Copyright (c) 2009 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app.h>
#include <nusa/debug.h>
#include <lib/console.h>
#include <lib/fs.h>
#include <nusa/err.h>
#include <kernel/thread.h>

// ERR_ALREADY_MOUNTED biasanya bernilai -18
#ifndef ERR_ALREADY_MOUNTED
#define ERR_ALREADY_MOUNTED (-18)
#endif

static void shell_entry(const struct app_descriptor *app, void *args) {
    // Delay sedikit untuk menghindari race condition dengan app lain
    thread_sleep(50);
    
    // Coba mount memfs di root
    status_t err = fs_mount("/", "memfs", NULL);
    
    if (err == NO_ERROR) {
        printf("✓ Successfully mounted memfs at /\n");
    } else if (err == ERR_ALREADY_MOUNTED) {
        // Sudah ter-mount, cek apakah benar-benar berfungsi
        printf("! Filesystem reports already mounted, verifying...\n");
        
        // Coba buka root directory untuk verify
        dirhandle *dh;
        status_t test = fs_open_dir("/", &dh);
        if (test == NO_ERROR) {
            fs_close_dir(dh);
            printf("✓ Filesystem is working correctly\n");
        } else {
            printf("✗ ERROR: Filesystem mount table corrupted (error %d)\n", test);
            printf("  Try rebooting or manual recovery\n");
        }
    } else {
        printf("✗ ERROR: Failed to mount memfs at /: error %d\n", err);
        printf("  Filesystem commands will not work!\n");
        printf("  Try manually: mount / memfs\n");
    }

    console_t *con = console_create(true);
    if (!con)
        return;

    console_start(con);

    // TODO: destroy console and free resources
}

APP_START(shell)
.entry = shell_entry,
APP_END