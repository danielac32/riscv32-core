



kernel:
	make -C kernel 
riscv : 
	gcc -o riscv mini-rv32ima/mini-rv32ima.c 

run2:
	./riscv baremetal/baremetal.bin

run:
	./riscv kernel/compile.bin



#riscv64-unknown-elf-objdump -D   kernel/compile.elf