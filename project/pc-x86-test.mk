# project/pc-x86-test.mk

ARCH := x86
SUBARCH := x86-32
TARGET := pc
PLATFORM := pc

# Karena file-file ini dipindah ke project/virtual/
# Kita harus include secara manual path-nya
include project/virtual/fs.mk
include project/virtual/minip.mk
include project/virtual/test.mk

# Tambahan module standar jika diperlukan
MODULES += app/shell