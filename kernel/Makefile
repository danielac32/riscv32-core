

#PREFIX:=riscv64-unknown-elf-
#CFLAGS:=-I/usr/include
#export PATH=/opt/riscv/bin:$PATH

PREFIX := riscv32-unknown-elf-
CFLAGS := -march=rv32ima -mabi=ilp32
LDFLAGS := -T linker.ld  

 
all:
	clear
	$(PREFIX)gcc -c -o boot.o  $(CFLAGS)  -I . boot.S
	$(PREFIX)gcc -c -o sys.o  $(CFLAGS)  -I . sys.S
	$(PREFIX)gcc -c -o main.o   $(CFLAGS) -I . main.c
	$(PREFIX)ld -m elf32lriscv boot.o sys.o main.o $(LDFLAGS) -o compile.elf
	$(PREFIX)objcopy compile.elf  -O binary compile.bin
