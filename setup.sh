#!/bin/bash

echo "=== NUSA (Little Kernel) Setup ==="
echo ""

if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "Detected OS: $NAME"
else
    echo "Unknown OS"
fi

install_deps_debian() {
    echo "Installing dependencies for Debian/Ubuntu..."
    sudo apt-get update
    sudo apt-get install -y build-essential python3 qemu-system-arm qemu-system-x86 qemu-system-misc
}

install_deps_fedora() {
    echo "Installing dependencies for Fedora..."
    sudo dnf install -y gcc make python3 qemu-system-arm qemu-system-x86
}

install_deps_arch() {
    echo "Installing dependencies for Arch..."
    sudo pacman -Sy --noconfirm base-devel python qemu-system-arm qemu-system-x86
}

case "$ID" in
    debian|ubuntu|linuxmint)
        install_deps_debian
        ;;
    fedora|rhel|centos)
        install_deps_fedora
        ;;
    arch|manjaro)
        install_deps_arch
        ;;
    *)
        echo "Unsupported distro. Please install manually: build-essential python3 qemu"
        ;;
esac

echo ""
echo "Downloading cross-compilation toolchains..."
python3 scripts/fetch-toolchains.py --prefix i386-elf x86_64-elf riscv32-elf riscv64-elf aarch64-elf arm-eabi

echo ""
echo "=== Setup Complete ==="
echo ""
echo "Available commands:"
echo "  ./build.sh list          - List available projects"
echo "  ./build.sh all           - Build all projects"
echo "  ./build.sh <project>     - Build specific project"
echo ""
echo "Example projects:"
echo "  pc-x86-test              - x86 32-bit"
echo "  pc-x86-64-test           - x86 64-bit"
echo "  qemu-virt-arm32-test     - ARM 32-bit"
echo "  qemu-virt-arm64-test     - ARM 64-bit"
echo "  qemu-virt-riscv32-test   - RISC-V 32-bit"
echo "  qemu-virt-riscv64-test   - RISC-V 64-bit"
