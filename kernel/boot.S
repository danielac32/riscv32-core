
#include <rv.h>   
.section .text
.global _start
.global _trap_setup
.global _trap
.global soft_trap
.global enable
.global disable
.global restore
.option arch, +zicsr

.align 4
_start:
    li a0, MSTATUS_MIE | MSTATUS_SIE | MSTATUS_MPIE | MSTATUS_SPIE
    csrc mstatus, a0
    //csrw mstatus, x0
    csrw mie, x0
    csrw mip, x0    

    la gp, __global_pointer
    la sp, _sstack
    jal ra, _trap_setup 
    mv  ra,zero
    jal zero, main

             

.macro push reg
    addi sp, sp, -4
    sw \reg, 0(sp)
.endm

.macro pop reg
    lw \reg, 0(sp)
    addi sp, sp, 4
.endm

 
.global syscall

syscall:
    mv a7,a0
    mv a0,a1
    mv a1,a2
    mv a2,a3
    mv a3,a4
    mv a4,a5
    ecall
    ret

    
_trap_setup:
    la t0, _trap 
    csrrw   zero, mtvec, t0 # setup machine trap vector
    ret

 
 


_trap:
addi sp, sp, -(35*4)

    // Save registers
    sw x31, (4*34)(sp)
    sw x30, (4*33)(sp)
    sw x29, (4*32)(sp)
    sw x28, (4*31)(sp)
    sw x27, (4*30)(sp)
    sw x26, (4*29)(sp)
    sw x25, (4*28)(sp)
    sw x24, (4*27)(sp)
    sw x23, (4*26)(sp)
    sw x22, (4*25)(sp)
    sw x21, (4*24)(sp)
    sw x20, (4*23)(sp)
    sw x19, (4*22)(sp)
    sw x18, (4*21)(sp)
    sw x17, (4*20)(sp)
    sw x16, (4*19)(sp)
    sw x15, (4*18)(sp)
    sw x14, (4*17)(sp)
    sw x13, (4*16)(sp)
    sw x12, (4*15)(sp)
    sw x11, (4*14)(sp)
    sw x10, (4*13)(sp)
    sw x9,  (4*12)(sp)
    sw x8,  (4*11)(sp)
    sw x7,  (4*10)(sp)
    sw x6,  (4* 9)(sp)
    sw x5,  (4* 8)(sp)
    sw x4,  (4* 7)(sp)
    sw x3,  (4* 6)(sp)
    sw x2,  (4* 5)(sp) // SP
    sw x1,  (4* 4)(sp) // RA
    //sw x0,  (4* 3)(sp) // ZERO

    csrr s0, mcause
    sw s0,  (4* 2)(sp)

    csrr s0, mstatus
    sw s0,  (4* 1)(sp)

    csrr s0, mepc
    sw s0,  (4* 0)(sp)

    // Call ISR handler
    mv a0, sp
    
    jal exceptionHandler

exception_return:
    mv sp, a0

    // Restore registers
    lw s0,  (4* 0)(sp)
    csrw mepc, s0

    lw s0,  (4* 1)(sp)
    csrw mstatus, s0

    //lw s0,  (4* 2)(sp)
    //csrw mcause, s0

    // lw(HOLE): x0 / ZERO
    lw x1,  (4* 4)(sp)
    // lw(HOLE): x2 / SP
    lw x3,  (4* 6)(sp)
    lw x4,  (4* 7)(sp)
    lw x5,  (4* 8)(sp)
    lw x6,  (4* 9)(sp)
    lw x7,  (4*10)(sp)
    lw x8,  (4*11)(sp)
    lw x9,  (4*12)(sp)
    lw x10, (4*13)(sp)
    lw x11, (4*14)(sp)
    lw x12, (4*15)(sp)
    lw x13, (4*16)(sp)
    lw x14, (4*17)(sp)
    lw x15, (4*18)(sp)
    lw x16, (4*19)(sp)
    lw x17, (4*20)(sp)
    lw x18, (4*21)(sp)
    lw x19, (4*22)(sp)
    lw x20, (4*23)(sp)
    lw x21, (4*24)(sp)
    lw x22, (4*25)(sp)
    lw x23, (4*26)(sp)
    lw x24, (4*27)(sp)
    lw x25, (4*28)(sp)
    lw x26, (4*29)(sp)
    lw x27, (4*30)(sp)
    lw x28, (4*31)(sp)
    lw x29, (4*32)(sp)
    lw x30, (4*33)(sp)
    lw x31, (4*34)(sp)

    addi sp, sp, (35*4)
    mret


soft_trap:
    csrsi mip, 0x8
    ret
    .end

enable:
    .func enable
    csrrsi a0, mstatus, MSTATUS_MIE
    ret
    .endfunc

disable:
    .func disable
    csrrci a0, mstatus, MSTATUS_MIE
    not a0,a0
    andi a0,a0,MSTATUS_MIE
        ret
    .endfunc

restore:
    .func restore
    // branch if is enable
    beq a0,x0, restore_set_mie
    csrrci a0, mstatus, MSTATUS_MIE
    j restore_return
restore_set_mie:
    csrrsi a0, mstatus, MSTATUS_MIE
restore_return:
    not  a0, a0
    andi a0, a0, MSTATUS_MIE
    ret
    .endfunc

