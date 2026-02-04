# x86-32 toolchain
ifeq ($(SUBARCH),x86-32)
ifndef ARCH_x86_TOOLCHAIN_INCLUDED
ARCH_x86_TOOLCHAIN_INCLUDED := 1

# Allow override from local.mk or command line
ifeq ($(origin ARCH_x86_TOOLCHAIN_PREFIX),undefined)
FOUNDTOOL=$(shell which i386-elf-gcc 2>/dev/null)
ifeq ($(FOUNDTOOL),)
$(warning cannot find i386-elf-gcc, using native gcc with -m32)
ARCH_x86_TOOLCHAIN_PREFIX :=
else
ARCH_x86_TOOLCHAIN_PREFIX := i386-elf-
endif
endif

endif
endif

# x86-64 toolchain
ifeq ($(SUBARCH),x86-64)
ifndef ARCH_x86_64_TOOLCHAIN_INCLUDED
ARCH_x86_64_TOOLCHAIN_INCLUDED := 1

# Allow override from local.mk or command line
ifeq ($(origin ARCH_x86_64_TOOLCHAIN_PREFIX),undefined)
FOUNDTOOL=$(shell which x86_64-elf-gcc 2>/dev/null)
ifeq ($(FOUNDTOOL),)
$(warning cannot find x86_64-elf-gcc, using native gcc)
ARCH_x86_64_TOOLCHAIN_PREFIX :=
else
ARCH_x86_64_TOOLCHAIN_PREFIX := x86_64-elf-
endif
endif

endif
endif
