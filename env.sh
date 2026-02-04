#!/bin/bash

unset CFLAGS CXXFLAGS LDFLAGS CPPFLAGS

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_DIR="$SCRIPT_DIR/toolchain"

export PATH="$TOOLCHAIN_DIR/x86_64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/i386-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/riscv64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/riscv32-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/aarch64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/arm-eabi-15.2.0-Linux-x86_64/bin:$PATH"

echo "Environment configured. Toolchains added to PATH."
echo "You can now run: make <project>"
