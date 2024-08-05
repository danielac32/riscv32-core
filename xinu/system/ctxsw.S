
#include <rv.h>

.globl ctxsw
.option arch, +zicsr
ctxsw:
    .func ctxsw
    // reserve space
    addi sp, sp, -4*23
    // save registers
    sw a7, 4*22(sp)
    sw a6, 4*21(sp)
    sw a5, 4*20(sp)
    sw a4, 4*19(sp)
    sw a3, 4*18(sp)
    sw a2, 4*17(sp)
    sw a1, 4*16(sp)
    sw a0, 4*15(sp)
    sw s11, 4*14(sp)
    sw s10, 4*13(sp)
    sw s9, 4*12(sp)
    sw s8, 4*11(sp)
    sw s7, 4*10(sp)
    sw s6, 4*9(sp)
    sw s5, 4*8(sp)
    sw s4, 4*7(sp)
    sw s3, 4*6(sp)
    sw s2, 4*5(sp)
    sw s1, 4*4(sp)
    sw s0, 4*3(sp)
    // save mstatus
    csrr t2, mstatus
    sw t2, 4*2(sp)
    // save ra
    sw ra, 4*1(sp)
    // save sp at oldthr->stkptr
    sw sp, 0(a0)
    // get saved sp from newthrd->stkptr
    lw sp, 0(a1)

    // restore t0 <- saved jump address
    lw t0, 0(sp)
    lw ra, 4*1(sp)
    // t2 <- saved mstatus
    lw t2, 4*2(sp)

    // restore saved registers
    lw s0, 4*3(sp)
    lw s1, 4*4(sp)
    lw s2, 4*5(sp)
    lw s3, 4*6(sp)
    lw s4, 4*7(sp)
    lw s5, 4*8(sp)
    lw s6, 4*9(sp)
    lw s7, 4*10(sp)
    lw s8, 4*11(sp)
    lw s9, 4*12(sp)
    lw s10, 4*13(sp)
    lw s11, 4*14(sp)

    lw a0, 4*15(sp)
    lw a1, 4*16(sp)
    lw a2, 4*17(sp)
    lw a3, 4*18(sp)
    lw a4, 4*19(sp)
    lw a5, 4*20(sp)
    lw a6, 4*21(sp)
    lw a7, 4*22(sp)

    // restore sp
    addi sp, sp, 4*23

    // check if interrupts are disabled in saved mstatus
    andi t3, t2, MSTATUS_MIE
    beq t3, x0, end_ctxsw
    // interrupts enabled (new thread)
    // now tricky part: we restore mstatus as it was saved
    // in the context _except_ the interrupt enable bit:
    // t2 <- t2 & ~MSTATUS_MIE
    andi t2, t2, ~MSTATUS_MIE
    csrw mstatus, t2
    // load jump address to mepc
    csrw mepc, t0
    // mret will enable interrupts and
    // jump to the newly created thread
    mret

end_ctxsw:
    // interrupts disabled: return to old thread
    // pc is restored, next address is put into
    // t1 and ignored
    jalr x0, t0, 0
    .endfunc
