#!/bin/bash

unset CFLAGS CXXFLAGS LDFLAGS CPPFLAGS

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_DIR="$SCRIPT_DIR/toolchain"

export PATH="$TOOLCHAIN_DIR/x86_64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/i386-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/riscv64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/riscv32-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/aarch64-elf-15.2.0-Linux-x86_64/bin:$TOOLCHAIN_DIR/arm-eabi-15.2.0-Linux-x86_64/bin:$PATH"

if [ ! -d "$TOOLCHAIN_DIR" ]; then
    echo "Toolchain not found. Downloading..."
    python3 scripts/fetch-toolchains.py --prefix i386-elf x86_64-elf riscv32-elf riscv64-elf aarch64-elf arm-eabi
fi

PROJECT="${1:-all}"

build_project() {
    local proj="$1"
    echo "--- Building $proj ---"
    make "$proj" 2>&1
    if [ $? -eq 0 ]; then
        echo "SUCCESS: $proj"
    else
        echo "FAILED: $proj"
        return 1
    fi
}

if [ "$PROJECT" = "all" ]; then
    echo "=== Building all supported projects ==="
    echo ""
    echo "Supported architectures: x86, x86-64, ARM32, ARM64, RISC-V 32, RISC-V 64"
    echo ""
    build_project pc-x86-test
    build_project pc-x86-64-test
    build_project qemu-virt-arm32-test
    build_project qemu-virt-arm64-test
    build_project qemu-virt-riscv32-test
    build_project qemu-virt-riscv64-test
    echo ""
    echo "=== Build complete ==="
elif [ "$PROJECT" = "list" ]; then
    echo "Available projects:"
    echo "  pc-x86-test"
    echo "  pc-x86-64-test"
    echo "  pc-x86-legacy-test"
    echo "  qemu-virt-arm32-test"
    echo "  qemu-virt-arm64-test"
    echo "  qemu-virt-riscv32-test"
    echo "  qemu-virt-riscv64-test"
    echo "  qemu-virt-riscv64-supervisor-test"
else
    build_project "$PROJECT"
fi
