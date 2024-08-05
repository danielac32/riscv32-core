
#include <rv.h>   
.section .text
.global disable
.global restore
.global enable
.option arch, +zicsr

 

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