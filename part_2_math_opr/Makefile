# Compiler dan assembler
ASM=nasm
ASMFLAGS=-f elf64 -g -F dwarf
CC=gcc
CFLAGS=-m64 -g
LDFLAGS=-shared -fPIC

# Struktur folder
ASM_DIR=asm_library
BUILD_DIR=build

# Nama file output
LIBNAME=libio.so
EXE=interpreter

# Ambil semua file .asm dari folder asm_library
ASM_SRCS=$(wildcard $(ASM_DIR)/*.asm)
ASM_OBJS=$(patsubst $(ASM_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))

.PHONY: all clean

# Target utama
all: $(BUILD_DIR) $(LIBNAME) $(EXE)

# Buat folder build jika belum ada
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile tiap .asm jadi .o di build/
$(BUILD_DIR)/%.o: $(ASM_DIR)/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Link semua .o jadi libio.so di root folder
$(LIBNAME): $(ASM_OBJS)
	$(CC) $(LDFLAGS) -o $(LIBNAME) $^

# Compile interpreter.c dan link dengan libio.so
$(EXE): interpreter.c $(LIBNAME)
	$(CC) $(CFLAGS) interpreter.c -L. -lio -Wl,-rpath='$$ORIGIN' -o $(EXE)
# Hapus semua hasil kompilasi
clean:
	rm -rf $(BUILD_DIR) *.so $(EXE)
