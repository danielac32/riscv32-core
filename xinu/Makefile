

#PREFIX:=riscv64-unknown-elf-
#CFLAGS:=-I/usr/include
#export PATH=/opt/riscv/bin:$PATH


 
TOOLSET     ?= riscv32-unknown-elf-
CC           = $(TOOLSET)gcc
LD           = $(TOOLSET)ld
AR           = $(TOOLSET)gcc-ar
OBJCOPY      = $(TOOLSET)objcopy
OPTFLAGS    ?= -Og
 

RM = rm -f
fixpath = $(strip $1)

 
CFLAGS      ?= -march=rv32ima -mabi=ilp32 -ffreestanding -nostdlib -nostartfiles -fno-builtin -mcmodel=medany
LDFLAGS      =  -I include  
INCLUDES     =   -I include 
LDSCRIPT     =  linker.ld

OBJDIR       = obj
START      =   $(wildcard system/boot.S) $(wildcard system/intr.S) $(wildcard system/ctxsw.S)
STARTOBJ      = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(START)))))
SOURCES      =  $(wildcard system/*.c) 
OBJECTS      = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(SOURCES)))))
SRCLIB         = $(wildcard lib/*.c)
LIBOBJ         = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(SRCLIB)))))

DOUT         = kernel


SRCPATH = $(sort $(dir $(START) $(SOURCES) $(SRCLIB) ))
vpath %.c $(SRCPATH)
vpath %.S $(SRCPATH)


 
$(OBJDIR):
	@mkdir $@


kernel: $(DOUT).bin
		riscv32-unknown-elf-objdump -d kernel.elf > kernel.list


$(DOUT).bin : $(DOUT).elf
	@echo building $@
	@$(OBJCOPY) -O binary $< $@



$(DOUT).elf : $(OBJDIR) $(STARTOBJ) $(LIBOBJ) $(OBJECTS)
	@echo building $@
	@$(CC) $(CFLAGS) $(LDFLAGS) -T $(LDSCRIPT) $(STARTOBJ) $(LIBOBJ) $(OBJECTS) -o $@

	
clean: $(OBJDIR)
	$(MAKE) --version
	@$(RM) $(DOUT).*
	@$(RM) $(call fixpath, $(OBJDIR)/*.*)


$(OBJDIR)/%.o: %.S
	@echo assembling $<
	@$(CC) $(CFLAGS)  $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS)  $(INCLUDES) -c $< -o $@